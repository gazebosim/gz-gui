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
#include <ignition/common/StringUtils.hh>
#include <ignition/common/SystemPaths.hh>
#include <ignition/common/Util.hh>

#include "ignition/gui/qt.h"
#include "ignition/gui/config.hh"
#include "ignition/gui/Dialog.hh"
#include "ignition/gui/Dock.hh"
#include "ignition/gui/Iface.hh"
#include "ignition/gui/MainWindow.hh"
#include "ignition/gui/Plugin.hh"

// These are needed by QT. They need to stay valid during the entire
// lifetime of the application, and argc > 0 and argv must contain one valid
// character string
int g_argc = 1;
char **g_argv;

using namespace ignition;
using namespace gui;

// Forward declarations.
std::string homePath();

/// \brief Pointer to application
QApplication *g_app;

/// \brief Pointer to main window
MainWindow *g_mainWin = nullptr;

/// \brief Vector of pointers to dialogs
std::vector<Dialog *> g_dialogs;

/// \brief Queue of plugins which should be added to the window
std::queue<std::shared_ptr<Plugin>> g_pluginsToAdd;

/// \brief Vector of pointers to plugins already added, we hang on to these
/// until it is ok to unload the plugin's shared library.
std::vector<std::shared_ptr<Plugin>> g_pluginsAdded;

/// \brief Environment variable which holds paths to look for plugins
std::string g_pluginPathEnv = "IGN_GUI_PLUGIN_PATH";

/// \brief Vector of paths to look for plugins
std::vector<std::string> g_pluginPaths;

/// \brief Holds a configuration which may be applied to g_mainWin once it
/// is created by calling applyConfig(). It's no longer needed and should not
/// be used after that.
WindowConfig g_windowConfig;

/// \brief The path containing the default configuration file.
std::string g_defaultConfigPath = ignition::common::joinPaths(
  homePath(), ".ignition", "gui", "default.config");

/////////////////////////////////////////////////
// Check whether the app has been initialized
bool checkApp()
{
  if (!g_app)
  {
    ignerr <<
        "Application not initialized. Have you called initApp() yet?"
        << std::endl;
  }

  return g_app;
}

/////////////////////////////////////////////////
// Install a signal handler to allow graceful shutdown on Ctrl-C.
bool installSignalHandler()
{
  if (!checkApp())
    return false;

#ifndef _WIN32
  auto handler = [](int)  // NOLINT(readability/casting)
      {
        // Note: Don't call stop() for the main window, we close it and let the
        // program pick it up from there
        if (g_mainWin)
          g_mainWin->close();
        else
          stop();
      };

  struct sigaction sigact;
  sigact.sa_flags = 0;
  sigact.sa_handler = handler;

  if (sigemptyset(&sigact.sa_mask) != 0)
  {
    ignerr << "sigemptyset failed while setting up for SIGINT"
              << std::endl;
    return false;
  }

  if (sigaction(SIGINT, &sigact, nullptr))
  {
    ignerr << "signal(2) failed while setting up for SIGINT"
              << std::endl;
    return false;
  }
#endif

  return true;
}

//////////////////////////////////////////////////
// QT message handler that pipes qt messages into our console system.
void messageHandler(QtMsgType _type, const QMessageLogContext &_context,
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
/// \brief Get home directory.
/// \return Home directory or empty string if home wasn't found.
/// \ToDo: Move this function to ignition::common::Filesystem
std::string homePath()
{
  std::string homePath;
#ifndef _WIN32
  ignition::common::env("HOME", homePath);
#else
  ignition::common::env("HOMEPATH", homePath);
#endif

  return homePath;
}

/////////////////////////////////////////////////
void removeAddedPlugin(std::shared_ptr<Plugin> _plugin)
{
  // If parent is a dialog, remove that too
  auto dialog = qobject_cast<Dialog *>(_plugin->parent());
  if (dialog)
  {
    g_dialogs.erase(std::remove(
        g_dialogs.begin(),
        g_dialogs.end(), dialog),
        g_dialogs.end());
  }

  g_pluginsAdded.erase(std::remove(
      g_pluginsAdded.begin(),
      g_pluginsAdded.end(), _plugin),
      g_pluginsAdded.end());
}

/////////////////////////////////////////////////
bool ignition::gui::runConfig(const std::string &_config)
{
  igndbg << "Loading config file [" << _config << "]" << std::endl;

  if (_config.empty())
  {
    ignerr << "Missing config filename" << std::endl;
    return false;
  }

  initApp();

  if (!loadConfig(_config))
  {
    stop();
    return false;
  }

  createMainWindow();
  runMainWindow();
  stop();

  return true;
}

/////////////////////////////////////////////////
bool ignition::gui::runEmptyWindow()
{
  igndbg << "Loading default window" << std::endl;

  initApp();

  loadDefaultConfig();

  createMainWindow();
  runMainWindow();

  stop();

  return true;
}

/////////////////////////////////////////////////
bool ignition::gui::runStandalone(const std::string &_filename)
{
  igndbg << "Loading standalone plugin [" << _filename << "]" << std::endl;

  if (_filename.empty())
  {
    ignerr << "Missing plugin filename" << std::endl;
    return false;
  }

  initApp();

  if (!loadPlugin(_filename))
  {
    stop();
    return false;
  }

  runDialogs();

  auto openDialogs = g_dialogs.size();
  auto closedDialogs = 0u;

  for (auto const &dialog : g_dialogs)
  {
    dialog->connect(dialog, &QDialog::finished, dialog, [&](){
      igndbg << "Dialog [" << dialog->windowTitle().toStdString() << "] closed."
             << std::endl;
      closedDialogs++;
    });
  }

  // Wait until all dialogs are closed
  while (closedDialogs < openDialogs)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(2));
    QCoreApplication::processEvents();
  }

  stop();

  return true;
}

/////////////////////////////////////////////////
bool ignition::gui::initApp()
{
  if (g_app)
    return false;

  // Configure console
  ignition::common::Console::SetPrefix("[GUI] ");

  igndbg << "Init app" << std::endl;

  // Create app
  g_app = new QApplication(g_argc, g_argv);

  // Apply Ignition GUI's default stylesheet
  setStyleFromFile(":/style.qss");

  // Install signal handler for graceful shutdown
  installSignalHandler();

  // Handle qt console messages
  qInstallMessageHandler(messageHandler);

  return true;
}

/////////////////////////////////////////////////
bool ignition::gui::stop()
{
  igndbg << "Stop" << std::endl;

  if (g_mainWin)
  {
    g_mainWin->CloseAllDocks();
    if (g_mainWin->isVisible())
      g_mainWin->close();
    delete g_mainWin;
    g_mainWin = nullptr;
  }

  for (auto dialog : g_dialogs)
  {
    dialog->close();
    dialog->deleteLater();
  }
  g_dialogs.clear();

  if (g_app)
  {
    g_app->quit();
    delete g_app;
    g_app = nullptr;
  }

  std::queue<std::shared_ptr<Plugin>> empty;
  std::swap(g_pluginsToAdd, empty);
  g_pluginsAdded.clear();

  return true;
}

/////////////////////////////////////////////////
bool ignition::gui::loadConfig(const std::string &_config)
{
  if (!checkApp())
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
    if (_config != defaultConfigPath())
    {
      ignerr << "Failed to load file [" << _config << "]: XMLError"
             << std::endl;
    }

    return false;
  }

  ignmsg << "Loading config [" << _config << "]" << std::endl;

  // Clear all previous plugins
  g_pluginsAdded.clear();

  // Process each plugin
  for (auto pluginElem = doc.FirstChildElement("plugin"); pluginElem != nullptr;
      pluginElem = pluginElem->NextSiblingElement("plugin"))
  {
    auto filename = pluginElem->Attribute("filename");
    loadPlugin(filename, pluginElem);
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
    g_windowConfig.MergeFromXML(std::string(printer.CStr()));
  }

  return true;
}

/////////////////////////////////////////////////
bool ignition::gui::loadDefaultConfig()
{
  return loadConfig(g_defaultConfigPath);
}

/////////////////////////////////////////////////
bool ignition::gui::setStyleFromFile(const std::string &_qssFile)
{
  if (!checkApp())
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
  return setStyleFromString(styleStr.toStdString());
}

/////////////////////////////////////////////////
bool ignition::gui::setStyleFromString(const std::string &_style)
{
  if (!checkApp())
    return false;

  g_windowConfig.styleSheet = _style;
  g_app->setStyleSheet(QString::fromStdString(g_windowConfig.styleSheet));

  // \todo Return false if sheet is can't be correctly parsed.
  return true;
}

/////////////////////////////////////////////////
void ignition::gui::setDefaultConfigPath(const std::string &_path)
{
  g_defaultConfigPath = _path;
}

/////////////////////////////////////////////////
std::string ignition::gui::defaultConfigPath()
{
  return g_defaultConfigPath;
}

/////////////////////////////////////////////////
bool ignition::gui::loadPlugin(const std::string &_filename,
    const tinyxml2::XMLElement *_pluginElem)
{
  if (!checkApp())
    return false;

  ignmsg << "Loading plugin [" << _filename << "]" << std::endl;

  // Get full path
  auto home = homePath();

  ignition::common::SystemPaths systemPaths;
  systemPaths.SetPluginPathEnv(g_pluginPathEnv);

  for (const auto &path : g_pluginPaths)
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
  ignition::common::PluginLoader pluginLoader;

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
  g_pluginsToAdd.push(plugin);

  return true;
}

/////////////////////////////////////////////////
bool ignition::gui::createMainWindow()
{
  if (!checkApp())
    return false;

  igndbg << "Create main window" << std::endl;

  g_mainWin = new MainWindow();

  return addPluginsToWindow() && applyConfig();
}

/////////////////////////////////////////////////
bool ignition::gui::addPluginsToWindow()
{
  // Create a widget for each plugin
  auto count = 0;
  while (!g_pluginsToAdd.empty())
  {
    auto plugin = g_pluginsToAdd.front();

    g_pluginsAdded.push_back(plugin);
    g_pluginsToAdd.pop();

    if (plugin->DeleteLaterRequested())
    {
      removeAddedPlugin(plugin);
      continue;
    }

    auto title = QString::fromStdString(plugin->Title());
    auto dock = new Dock();
    dock->setParent(g_mainWin);
    dock->setWindowTitle(title);
    dock->setObjectName(title);
    dock->setAllowedAreas(Qt::TopDockWidgetArea);
    dock->setWidget(&*plugin);
    dock->setAttribute(Qt::WA_DeleteOnClose);
    if (!plugin->HasTitlebar())
      dock->setTitleBarWidget(new QWidget());

    if (count % 2 == 0)
      g_mainWin->addDockWidget(Qt::TopDockWidgetArea, dock, Qt::Horizontal);
    else
      g_mainWin->addDockWidget(Qt::TopDockWidgetArea, dock, Qt::Vertical);

    ignmsg << "Added plugin [" << plugin->Title() << "] to main window" <<
        std::endl;

    g_mainWin->connect(dock, &Dock::Closing, [plugin]
    {
      removeAddedPlugin(plugin);
    });

    count++;
  }

  return true;
}

/////////////////////////////////////////////////
bool ignition::gui::applyConfig()
{
  igndbg << "Applying config" << std::endl;

  return g_mainWin->ApplyConfig(g_windowConfig);
}

/////////////////////////////////////////////////
ignition::gui::MainWindow *ignition::gui::mainWindow()
{
  return g_mainWin;
}

/////////////////////////////////////////////////
std::vector<Dialog *> ignition::gui::dialogs()
{
  return g_dialogs;
}

/////////////////////////////////////////////////
bool ignition::gui::runMainWindow()
{
  if (!checkApp())
    return false;

  if (!mainWindow())
    return false;

  igndbg << "Run main window" << std::endl;

  g_mainWin->show();

  // Execute app
  g_app->exec();

  return true;
}

/////////////////////////////////////////////////
bool ignition::gui::runDialogs()
{
  if (!checkApp())
    return false;

  igndbg << "Run dialogs" << std::endl;

  while (!g_pluginsToAdd.empty())
  {
    auto plugin = g_pluginsToAdd.front();

    auto title = QString::fromStdString(plugin->Title());

    auto layout = new QVBoxLayout();
    layout->addWidget(plugin.get());

    auto dialog = new Dialog();
    dialog->setLayout(layout);
    dialog->setWindowTitle(title);
    dialog->setWindowModality(Qt::NonModal);
    dialog->setAttribute(Qt::WA_DeleteOnClose, true);

    g_dialogs.push_back(dialog);

    g_pluginsAdded.push_back(plugin);
    g_pluginsToAdd.pop();

    g_mainWin->connect(dialog, &Dialog::Closing, [plugin]
    {
      removeAddedPlugin(plugin);
    });

    dialog->show();
    igndbg << "Showing dialog [" << title.toStdString() << "]" << std::endl;
  }

  return true;
}

/////////////////////////////////////////////////
void ignition::gui::setPluginPathEnv(const std::string &_env)
{
  g_pluginPathEnv = _env;
}

/////////////////////////////////////////////////
void ignition::gui::addPluginPath(const std::string &_path)
{
  g_pluginPaths.push_back(_path);
}

/////////////////////////////////////////////////
void ignition::gui::setVerbosity(const unsigned int _verbosity)
{
  ignition::common::Console::SetVerbosity(_verbosity);
}

/////////////////////////////////////////////////
void ignition::gui::listPlugins()
{
  auto pluginsList = getPluginList();

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
    ignition::gui::getPluginList()
{
  // 1. Paths from env variable
  auto paths = ignition::common::SystemPaths::PathsFromEnv(g_pluginPathEnv);

  // 2. Paths added by calling addPluginPath
  for (auto const &path : g_pluginPaths)
    paths.push_back(path);

  // 3. ~/.ignition/gui/plugins
  auto home = homePath();
  paths.push_back(home + "/.ignition/gui/plugins");

  // 4. Install path
  paths.push_back(IGN_GUI_PLUGIN_INSTALL_DIR);

  // Populate map
  std::vector<std::pair<std::string, std::vector<std::string>>> plugins;

  for (auto const &path : paths)
  {
    std::vector<std::string> ps;

    ignition::common::DirIter endIter;
    for (ignition::common::DirIter dirIter(path);
        dirIter != endIter; ++dirIter)
    {
      auto plugin = ignition::common::basename(*dirIter);

      // All we verify is that the file starts with "lib", any further
      // checks would require loading the plugin.

      if (plugin.find("lib") == 0)
        ps.push_back(plugin);
    }

    plugins.push_back(std::make_pair(path, ps));
  }

  return plugins;
}

