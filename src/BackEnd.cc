/*
 * Copyright (C) 2017 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
*/

#include <signal.h>
#include <stdio.h>
#include <tinyxml2.h>

#include <iostream>
#include <queue>
#include <string>
#include <vector>

#include <ignition/common/Console.hh>
#include <ignition/common/Filesystem.hh>
#include <ignition/common/Plugin.hh>
#include <ignition/common/PluginLoader.hh>
#include <ignition/common/SignalHandler.hh>
#include <ignition/common/StringUtils.hh>
#include <ignition/common/SystemPaths.hh>
#include <ignition/common/Util.hh>

#include "ignition/gui/qt.h"
#include "ignition/gui/config.hh"

#include "ignition/gui/Application.hh"
#include "ignition/gui/BackEnd.hh"
#include "ignition/gui/Dialog.hh"
#include "ignition/gui/MainWindow.hh"
#include "ignition/gui/Plugin.hh"

namespace ignition
{
  namespace gui
  {
    class BackEndPrivate
    {
public:
      // These are needed by Qt. They need to stay valid during the entire
      // lifetime of the application, and argc > 0 and argv must contain one
      // valid character string
      int argc = 1;
      char **argv = new char *[argc];
      /// \brief Pointer to main window
      MainWindow *mainWin{nullptr};

      /// \brief Vector of pointers to dialogs
      std::vector<Dialog *> dialogs;

      /// \brief Queue of plugins which should be added to the window
      std::queue<std::shared_ptr<Plugin>> pluginsToAdd;

      /// \brief Vector of pointers to plugins already added, we hang on to these
      /// until it is ok to unload the plugin's shared library.
      std::vector<std::shared_ptr<Plugin>> pluginsAdded;

      /// \brief Environment variable which holds paths to look for plugins
      std::string pluginPathEnv = "IGN_GUI_PLUGIN_PATH";

      /// \brief Vector of paths to look for plugins
      std::vector<std::string> pluginPaths;

      /// \brief Holds a configuration which may be applied to mainWin once it
      /// is created by calling applyConfig(). It's no longer needed and should not
      /// be used after that.
      WindowConfig windowConfig;

      /// \brief The path containing the default configuration file.
      std::string defaultConfigPath;

      common::SignalHandler signalHandler;

    };
  }
}

using namespace ignition;
using namespace gui;

/////////////////////////////////////////////////
// BackEnd::BackEnd(int &_argc, char **_argv) : dataPtr(new BackEndPrivate)
BackEnd::BackEnd() : dataPtr(new BackEndPrivate)
{
  if (qGuiApp)
  {
    std::cerr << "Failed to initialize backend, "
              << "there's already a QGuiApplication running" << std::endl;
    return;
  }

  // Configure console
  common::Console::SetPrefix("[GUI] ");

  // Create app
//  new Application(_argc, _argv);
  new Application(this->dataPtr->argc, this->dataPtr->argv);

  // Install signal handler for graceful shutdown
  this->InstallSignalHandler();

  // Handle qt console messages
  qInstallMessageHandler(this->MessageHandler);

  this->dataPtr->defaultConfigPath = common::joinPaths(
        this->HomePath(), ".ignition", "gui", "default.config");
}

/////////////////////////////////////////////////
BackEnd::~BackEnd()
{
  igndbg << "Stop" << std::endl;

  if (this->dataPtr->mainWin && this->dataPtr->mainWin->QuickWindow())
  {
    // Detach widget from main window and leave libraries for ign-common
    auto plugins = this->dataPtr->mainWin->findChildren<Plugin *>();
    for (auto plugin : plugins)
    {
      auto pluginName = plugin->CardItem()->objectName();
      this->RemovePlugin(pluginName.toStdString());
    }
    if (this->dataPtr->mainWin->QuickWindow()->isVisible())
      this->dataPtr->mainWin->QuickWindow()->close();
    delete this->dataPtr->mainWin;
    this->dataPtr->mainWin = nullptr;
  }

  for (auto dialog : this->dataPtr->dialogs)
  {
    if (dialog->QuickWindow())
      dialog->QuickWindow()->close();
    dialog->deleteLater();
  }
  this->dataPtr->dialogs.clear();

  if (qGuiApp)
  {
    qGuiApp->quit();
    delete qGuiApp;
  }

  std::queue<std::shared_ptr<Plugin>> empty;
  std::swap(this->dataPtr->pluginsToAdd, empty);
  this->dataPtr->pluginsAdded.clear();
  this->dataPtr->pluginPaths.clear();
  this->dataPtr->pluginPathEnv = "IGN_GUI_PLUGIN_PATH";
}

/////////////////////////////////////////////////
bool BackEnd::RemovePlugin(const std::string &_pluginName)
{
  bool found{false};
  for (auto plugin : this->dataPtr->pluginsAdded)
  {
    auto cardItem = plugin->CardItem();
    if (!cardItem)
      continue;

    if (cardItem->objectName().toStdString() == _pluginName)
    {
      // Remove on QML
      cardItem->deleteLater();

      // Unload shared library
      this->RemoveAddedPlugin(plugin);

      found = true;
      break;
    }
  }

  return found;
}

/////////////////////////////////////////////////
bool BackEnd::RunConfig(const std::string &_config)
{
  igndbg << "Loading config file [" << _config << "]" << std::endl;

  if (_config.empty())
  {
    ignerr << "Missing config filename" << std::endl;
    return false;
  }

  if (!this->LoadConfig(_config))
  {
    return false;
  }

  this->CreateMainWindow();
  this->RunMainWindow();

  return true;
}

/////////////////////////////////////////////////
bool BackEnd::RunEmptyWindow()
{
  igndbg << "Loading default window" << std::endl;

  this->LoadDefaultConfig();

  this->CreateMainWindow();
  this->RunMainWindow();

  return true;
}

/////////////////////////////////////////////////
bool BackEnd::RunStandalone(const std::string &_filename)
{
  igndbg << "Loading standalone plugin [" << _filename << "]" << std::endl;

  if (_filename.empty())
  {
    ignerr << "Missing plugin filename" << std::endl;
    return false;
  }

  if (!this->LoadPlugin(_filename))
  {
    return false;
  }

  this->RunDialogs();

  return true;
}

/////////////////////////////////////////////////
bool BackEnd::LoadConfig(const std::string &_config)
{
  if (!this->CheckApp())
    return false;

  if (_config.empty())
  {
    ignerr << "Missing config file" << std::endl;
    return false;
  }

  // Use tinyxml to read config
  tinyxml2::XMLDocument doc;
  auto success = !doc.LoadFile(_config.c_str());
  if (!success)
  {
    // We do not show an error message if the default config path doesn't exist
    // yet. It's expected behavior, it will be created the first time the user
    // presses "Save configuration".
    if (_config != this->DefaultConfigPath())
    {
      ignerr << "Failed to load file [" << _config << "]: XMLError"
             << std::endl;
    }

    return false;
  }

  ignmsg << "Loading config [" << _config << "]" << std::endl;

  // Clear all previous plugins
  auto plugins = this->Window()->findChildren<Plugin *>();
  for (auto plugin : plugins)
  {
    auto pluginName = plugin->CardItem()->objectName();
    this->RemovePlugin(pluginName.toStdString());
  }
  if (this->dataPtr->pluginsAdded.size() > 0)
  {
    ignerr << "The plugin list was not properly cleaned up." << std::endl;
  }
  this->dataPtr->pluginsAdded.clear();

  // Process each plugin
  for (auto pluginElem = doc.FirstChildElement("plugin"); pluginElem != nullptr;
      pluginElem = pluginElem->NextSiblingElement("plugin"))
  {
    auto filename = pluginElem->Attribute("filename");
    this->LoadPlugin(filename, pluginElem);
  }

  // Process window properties
  if (auto winElem = doc.FirstChildElement("window"))
  {
    igndbg << "Loading window config" << std::endl;

    tinyxml2::XMLPrinter printer;
    if (!winElem->Accept(&printer))
    {
      ignwarn << "There was an error parsing the <window> element"
              << std::endl;
      return false;
    }
    this->dataPtr->windowConfig.MergeFromXML(std::string(printer.CStr()));
  }

  return true;
}

/////////////////////////////////////////////////
bool BackEnd::LoadDefaultConfig()
{
  return this->LoadConfig(this->dataPtr->defaultConfigPath);
}

/////////////////////////////////////////////////
bool BackEnd::SetStyleFromFile(const std::string &_qssFile)
{
  if (!this->CheckApp())
    return false;

  QFile file(QString::fromStdString(_qssFile));
  if (!file.open(QFile::ReadOnly))
  {
    ignerr << "Failed to open [" << _qssFile << "]: ";
    if (_qssFile.empty())
    {
      std::cout << "file path is empty." << std::endl;
    }
    else if (!file.exists())
    {
      std::cout << "file doesn't exist." << std::endl;
    }
    else
    {
      std::cout << "potentially invalid permissions." << std::endl;
    }
    return false;
  }

  ignmsg << "Applying stylesheet [" << _qssFile << "]" << std::endl;

  QString styleStr = QLatin1String(file.readAll());
  return this->SetStyleFromString(styleStr.toStdString());
}

/////////////////////////////////////////////////
bool BackEnd::SetStyleFromString(const std::string &_style)
{
  if (!this->CheckApp())
    return false;

  this->dataPtr->windowConfig.styleSheet = _style;
  // this->dataPtr->app->setStyleSheet(QString::fromStdString(this->dataPtr->windowConfig.styleSheet));

  // \todo Return false if sheet is can't be correctly parsed.
  return true;
}

/////////////////////////////////////////////////
void BackEnd::SetDefaultConfigPath(const std::string &_path)
{
  this->dataPtr->defaultConfigPath = _path;
}

/////////////////////////////////////////////////
std::string BackEnd::DefaultConfigPath()
{
  return this->dataPtr->defaultConfigPath;
}

/////////////////////////////////////////////////
bool BackEnd::LoadPlugin(const std::string &_filename,
    const tinyxml2::XMLElement *_pluginElem)
{
  if (!this->CheckApp())
    return false;

  ignmsg << "Loading plugin [" << _filename << "]" << std::endl;

  // Get full path
  auto home = this->HomePath();

  common::SystemPaths systemPaths;
  systemPaths.SetPluginPathEnv(this->dataPtr->pluginPathEnv);

  for (const auto &path : this->dataPtr->pluginPaths)
    systemPaths.AddPluginPaths(path);

  // Add default folder and install folder
  systemPaths.AddPluginPaths(home + "/.ignition/gui/plugins:" +
                             IGN_GUI_PLUGIN_INSTALL_DIR);

  auto pathToLib = systemPaths.FindSharedLibrary(_filename);
  if (pathToLib.empty())
  {
    ignerr << "Failed to load plugin [" << _filename <<
              "] : couldn't find shared library." << std::endl;
    return false;
  }

  // Load plugin
  common::PluginLoader pluginLoader;

  auto pluginNames = pluginLoader.LoadLibrary(pathToLib);
  if (pluginNames.empty())
  {
    ignerr << "Failed to load plugin [" << _filename <<
              "] : couldn't load library on path [" << pathToLib <<
              "]." << std::endl;
    return false;
  }

  auto pluginName = *pluginNames.begin();
  if (pluginName.empty())
  {
    ignerr << "Failed to load plugin [" << _filename <<
              "] : couldn't load library on path [" << pathToLib <<
              "]." << std::endl;
    return false;
  }

  auto commonPlugin = pluginLoader.Instantiate(pluginName);
  if (!commonPlugin)
  {
    ignerr << "Failed to load plugin [" << _filename <<
              "] : couldn't instantiate plugin on path [" << pathToLib <<
              "]." << std::endl;
    return false;
  }

  auto plugin = commonPlugin->QueryInterfaceSharedPtr<ignition::gui::Plugin>();
  if (!plugin)
  {
    ignerr << "Failed to load plugin [" << _filename <<
              "] : couldn't get interface [" << pluginName <<
              "]." << std::endl;
    return false;
  }

  // Basic config in case there is none
  if (!_pluginElem)
  {
    std::string pluginStr = "<plugin filename=\"" + _filename + "\"></plugin>";

    tinyxml2::XMLDocument pluginDoc;
    pluginDoc.Parse(pluginStr.c_str());

    plugin->Load(pluginDoc.FirstChildElement("plugin"));
  }
  else
    plugin->Load(_pluginElem);

  // Store plugin in queue to be added to the window
  this->dataPtr->pluginsToAdd.push(plugin);

  return true;
}

/////////////////////////////////////////////////
bool BackEnd::CreateMainWindow()
{
  if (!this->CheckApp())
    return false;

  igndbg << "Create main window" << std::endl;

  this->dataPtr->mainWin = new MainWindow();

  return this->AddPluginsToWindow() && this->ApplyConfig();
}

/////////////////////////////////////////////////
bool BackEnd::AddPluginsToWindow()
{
  // Get main window background item
  auto bgItem =
      this->dataPtr->mainWin->QuickWindow()->findChild<QQuickItem *>("background");
  if (!this->dataPtr->pluginsToAdd.empty() && !bgItem)
  {
    ignerr << "Null background QQuickItem!" << std::endl;
    return false;
  }

  // Create a widget for each plugin
  auto count = 0;
  while (!this->dataPtr->pluginsToAdd.empty())
  {
    auto plugin = this->dataPtr->pluginsToAdd.front();

    this->dataPtr->pluginsAdded.push_back(plugin);
    this->dataPtr->pluginsToAdd.pop();

    if (plugin->DeleteLaterRequested())
    {
      this->RemoveAddedPlugin(plugin);
      continue;
    }

    auto cardItem = plugin->CardItem();
    if (!cardItem)
      continue;

    // Add card to main window
    cardItem->setParentItem(bgItem);
//    cardItem->setParent(this->dataPtr->engine);
    plugin->setParent(this->Window());

    // Signals
    this->dataPtr->mainWin->connect(cardItem, SIGNAL(close()), this->dataPtr->mainWin,
        SLOT(OnPluginClose()));

    ignmsg << "Added plugin [" << plugin->Title() << "] to main window" <<
        std::endl;

    count++;
  }

  this->dataPtr->mainWin->SetPluginCount(this->dataPtr->pluginsAdded.size());

  return true;
}

/////////////////////////////////////////////////
bool BackEnd::ApplyConfig()
{
  igndbg << "Applying config" << std::endl;

  return this->dataPtr->mainWin->ApplyConfig(this->dataPtr->windowConfig);
}

/////////////////////////////////////////////////
ignition::gui::MainWindow *BackEnd::Window()
{
  return this->dataPtr->mainWin;
}

/////////////////////////////////////////////////
std::vector<Dialog *> BackEnd::Dialogs()
{
  return this->dataPtr->dialogs;
}

/////////////////////////////////////////////////
bool BackEnd::RunMainWindow()
{
  if (!this->CheckApp())
    return false;

  if (!this->Window() || !this->Window()->QuickWindow())
    return false;

  igndbg << "Run main window" << std::endl;

  // Execute app
  qGuiApp->exec();

  return true;
}

/////////////////////////////////////////////////
bool BackEnd::RunDialogs()
{
  if (!this->CheckApp())
    return false;

  igndbg << "Run dialogs" << std::endl;

  while (!this->dataPtr->pluginsToAdd.empty())
  {
    auto plugin = this->dataPtr->pluginsToAdd.front();
    this->dataPtr->pluginsToAdd.pop();

    // Create card
    auto cardItem = plugin->CardItem();
    if (!cardItem)
      continue;

    // Create dialog
    auto dialog = new Dialog();
    if (!dialog || !dialog->QuickWindow())
      continue;

    this->dataPtr->dialogs.push_back(dialog);

    cardItem->setParentItem(dialog->RootItem());

    // Configure card
    cardItem->setProperty("hasDockButton", false);
    cardItem->setProperty("hasCloseButton", false);

    // Configure dialog
    auto cardWidth = cardItem->property("width").toInt();
    auto cardHeight = cardItem->property("height").toInt();
    dialog->QuickWindow()->setProperty("width", cardWidth);
    dialog->QuickWindow()->setProperty("height", cardHeight);

    this->dataPtr->pluginsAdded.push_back(plugin);

    auto title = QString::fromStdString(plugin->Title());
    igndbg << "Showing dialog [" << title.toStdString() << "]" << std::endl;
  }

  if (this->dataPtr->pluginsAdded.empty())
    return false;

  // Run app - blocks
  App()->exec();

  return true;
}

/////////////////////////////////////////////////
void BackEnd::SetPluginPathEnv(const std::string &_env)
{
  this->dataPtr->pluginPathEnv = _env;
}

/////////////////////////////////////////////////
void BackEnd::AddPluginPath(const std::string &_path)
{
  this->dataPtr->pluginPaths.push_back(_path);
}

/////////////////////////////////////////////////
void BackEnd::SetVerbosity(const unsigned int _verbosity)
{
  common::Console::SetVerbosity(_verbosity);
}

/////////////////////////////////////////////////
void BackEnd::ListPlugins()
{
  auto pluginsList = this->PluginList();

  for (auto const &path : pluginsList)
  {
    std::cout << path.first << std::endl;

    for (unsigned int i = 0; i < path.second.size(); ++i)
    {
      if (i == path.second.size() - 1)
        std::cout << "└── " << path.second[i] << std::endl;
      else
        std::cout << "├── " << path.second[i] << std::endl;
    }

    if (path.second.empty())
      std::cout << "└── No plugins" << std::endl;
  }
}

/////////////////////////////////////////////////
std::vector<std::pair<std::string, std::vector<std::string>>>
    BackEnd::PluginList()
{
  // 1. Paths from env variable
  auto paths = common::SystemPaths::PathsFromEnv(this->dataPtr->pluginPathEnv);

  // 2. Paths added by calling addPluginPath
  for (auto const &path : this->dataPtr->pluginPaths)
    paths.push_back(path);

  // 3. ~/.ignition/gui/plugins
  auto home = this->HomePath();
  paths.push_back(home + "/.ignition/gui/plugins");

  // 4. Install path
  paths.push_back(IGN_GUI_PLUGIN_INSTALL_DIR);

  // Populate map
  std::vector<std::pair<std::string, std::vector<std::string>>> plugins;

  for (auto const &path : paths)
  {
    std::vector<std::string> ps;

    common::DirIter endIter;
    for (common::DirIter dirIter(path);
        dirIter != endIter; ++dirIter)
    {
      auto plugin = common::basename(*dirIter);

      // All we verify is that the file starts with "lib", any further
      // checks would require loading the plugin.

      if (plugin.find("lib") == 0)
        ps.push_back(plugin);
    }

    plugins.push_back(std::make_pair(path, ps));
  }

  return plugins;
}

/////////////////////////////////////////////////
bool BackEnd::CheckApp()
{
  if (!qGuiApp)
  {
    ignerr <<
        "Application not initialized. Have you called initApp() yet?"
        << std::endl;
  }

  return qGuiApp;
}

/////////////////////////////////////////////////
bool BackEnd::InstallSignalHandler()
{
  if (!this->CheckApp())
    return false;

  auto handler = [this](int)  // NOLINT(readability/casting)
      {
        // Note: Don't call stop() if there are windows, we close them and let
        // the program pick it up from there
        if (this->dataPtr->mainWin && this->dataPtr->mainWin->QuickWindow())
        {
          this->dataPtr->mainWin->QuickWindow()->close();
        }
        else if (!this->dataPtr->dialogs.empty())
        {
          for (auto dialog : this->dataPtr->dialogs)
          {
            if (dialog->QuickWindow())
              dialog->QuickWindow()->close();
          }
        }
        else {}
        //  this->stop();
      };

  return this->dataPtr->signalHandler.AddCallback(handler);
}

//////////////////////////////////////////////////
void BackEnd::MessageHandler(QtMsgType _type, const QMessageLogContext &_context,
    const QString &_msg)
{
  std::string msg = "[QT] " + _msg.toStdString();
  if (_context.function)
    msg += std::string("(") + _context.function + ")";

  switch (_type)
  {
    case QtDebugMsg:
      igndbg << msg << std::endl;
      break;
    case QtInfoMsg:
      ignmsg << msg << std::endl;
      break;
    case QtWarningMsg:
      ignwarn << msg << std::endl;
      break;
    case QtFatalMsg:
    case QtCriticalMsg:
      ignerr << msg << std::endl;
      break;
    default:
      ignwarn << "Unknown QT Message type[" << _type << "]: "
        << msg << std::endl;
      break;
  }
}

/////////////////////////////////////////////////
std::string BackEnd::HomePath()
{
  std::string homePath;
#ifndef _WIN32
  common::env("HOME", homePath);
#else
  common::env("HOMEPATH", homePath);
#endif

  return homePath;
}

/////////////////////////////////////////////////
void BackEnd::RemoveAddedPlugin(std::shared_ptr<Plugin> _plugin)
{
  // If parent is a dialog, remove that too
//  auto dialog = qobject_cast<Dialog *>(_plugin->parent());
//  if (dialog)
//  {
//    this->dataPtr->dialogs.erase(std::remove(
//        this->dataPtr->dialogs.begin(),
//        this->dataPtr->dialogs.end(), dialog),
//        this->dataPtr->dialogs.end());
//  }

  this->dataPtr->pluginsAdded.erase(std::remove(
      this->dataPtr->pluginsAdded.begin(),
      this->dataPtr->pluginsAdded.end(), _plugin),
      this->dataPtr->pluginsAdded.end());

  this->dataPtr->mainWin->SetPluginCount(this->dataPtr->pluginsAdded.size());
}

