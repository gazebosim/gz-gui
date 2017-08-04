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
#include <iostream>

#include <ignition/common/Console.hh>
#include <ignition/common/PluginLoader.hh>
#include <ignition/common/SystemPaths.hh>

#include "ignition/gui/qt.h"
#include "ignition/gui/config.hh"
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

struct WindowConfig
{
  int pos_x;
  int pos_y;
  int width;
  int height;
  QByteArray state;
};

QApplication *g_app;
MainWindow *g_main_win = nullptr;
std::vector<QDialog *> g_dialogs;
std::vector<std::unique_ptr<Plugin>> g_plugins;
std::string g_pluginPathEnv = "IGN_GUI_PLUGIN_PATH";
std::vector<std::string> g_pluginPaths;
WindowConfig g_windowConfig;

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
        if (g_main_win)
          g_main_win->close();
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

/////////////////////////////////////////////////
// Get home directory
std::string homePath()
{
  char *homePath = getenv("HOME");
  std::string home;
  if (homePath)
    home = homePath;

  return home;
}

/////////////////////////////////////////////////
bool ignition::gui::runConfig(const std::string &_config)
{
  ignmsg << "Loading config file [" << _config << "]" << std::endl;

  if (_config.empty())
  {
    ignerr << "Missing config filename" << std::endl;
    return false;
  }

  initApp();

  if (!loadConfig(_config))
    return false;

  createMainWindow();
  runMainWindow();
  stop();

  return true;
}

/////////////////////////////////////////////////
bool ignition::gui::runStandalone(const std::string &_filename)
{
  ignmsg << "Loading standalone plugin [" << _filename << "]" << std::endl;

  if (_filename.empty())
  {
    ignerr << "Missing plugin filename" << std::endl;
    return false;
  }

  initApp();

  if (!loadPlugin(_filename))
    return false;

  runDialogs();

  auto openDialogs = g_dialogs.size();
  auto closedDialogs = 0u;

  for (auto const &dialog : g_dialogs)
  {
    dialog->connect(dialog, &QDialog::finished, dialog, [&](){
      ignmsg << "Dialog [" << dialog->windowTitle().toStdString() << "] closed."
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

  ignmsg << "Init app" << std::endl;

  // Create app
  g_app = new QApplication(g_argc, g_argv);

  // Set style
  QFile file(":/style.qss");
  file.open(QFile::ReadOnly);
  QString styleSheet = QLatin1String(file.readAll());
  g_app->setStyleSheet(styleSheet);

  // Install signal handler for graceful shutdown
  installSignalHandler();

  return true;
}

/////////////////////////////////////////////////
bool ignition::gui::stop()
{
  ignmsg << "Stop" << std::endl;

  if (g_main_win)
  {
    g_main_win->close();
    delete g_main_win;
    g_main_win = nullptr;
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

  g_plugins.clear();

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
    ignmsg << "Failed to load file [" << _config << "]: XMLError"
              << std::endl;
    return false;
  }

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
    if (auto xElem = winElem->FirstChildElement("position_x"))
      xElem->QueryIntText(&g_windowConfig.pos_x);

    if (auto yElem = winElem->FirstChildElement("position_y"))
      yElem->QueryIntText(&g_windowConfig.pos_y);

    if (auto widthElem = winElem->FirstChildElement("width"))
      widthElem->QueryIntText(&g_windowConfig.width);

    if (auto heightElem = winElem->FirstChildElement("height"))
      heightElem->QueryIntText(&g_windowConfig.height);

    if (auto stateElem = winElem->FirstChildElement("state"))
    {
      auto text = stateElem->GetText();
      g_windowConfig.state = QByteArray::fromBase64(text);
    }
  }

//  _config = g_windowConfig;
//  _plugins = g_plugins;

  return true;
}

/////////////////////////////////////////////////
bool ignition::gui::loadPlugin(const std::string &_filename,
    const tinyxml2::XMLElement *_pluginElem)
{
  if (!checkApp())
    return false;

  // Get full path
  auto home = homePath();

  ignition::common::SystemPaths systemPaths;
  systemPaths.SetPluginPathEnv(g_pluginPathEnv);

  for (const auto &path : g_pluginPaths)
    systemPaths.AddPluginPaths(path);

  // Add default folder and install folder
  systemPaths.AddPluginPaths(home + "/.ignition/gui/plugins:" +
                             IGN_GUI_PLUGIN_INSTALL_PATH);

  auto pathToLib = systemPaths.FindSharedLibrary(_filename);
  if (pathToLib.empty())
  {
    ignerr << "Failed to load plugin [" << _filename <<
              "] : couldn't find shared library." << std::endl;
    return false;
  }

  // Load plugin
  ignition::common::PluginLoader pluginLoader;

  auto pluginName = pluginLoader.LoadLibrary(pathToLib);
  if (pluginName.empty())
  {
    ignerr << "Failed to load plugin [" << _filename <<
              "] : couldn't load library on path [" << pathToLib <<
              "]." << std::endl;
    return false;
  }

  auto plugin = pluginLoader.Instantiate<ignition::gui::Plugin>(pluginName);
  if (!plugin)
  {
    ignerr << "Failed to load plugin [" << _filename <<
              "] : couldn't instantiate plugin on path [" << pathToLib <<
              "]." << std::endl;
    return false;
  }
  plugin->Load(_pluginElem);

  // Store plugin in list
  g_plugins.push_back(std::move(plugin));

  return true;
}

/////////////////////////////////////////////////
bool ignition::gui::createMainWindow()
{
  if (!checkApp())
    return false;

  ignmsg << "Create main window" << std::endl;

  g_main_win = new MainWindow();

  return addPluginsToWindow();
}

/////////////////////////////////////////////////
bool ignition::gui::addPluginsToWindow()
{
  ignmsg << "Add plugins to main window" << std::endl;

  // Create a widget for each plugin
  auto count = 0;
  for (auto &plugin : g_plugins)
  {
    auto title = QString::fromStdString(plugin->Title());
    auto dock = new QDockWidget(title, g_main_win);
    dock->setObjectName(title);
    dock->setAllowedAreas(Qt::TopDockWidgetArea);
    dock->setWidget(&*plugin);
    if (!plugin->HasTitlebar())
      dock->setTitleBarWidget(new QWidget());

    if (count % 2 == 0)
      g_main_win->addDockWidget(Qt::TopDockWidgetArea, dock, Qt::Horizontal);
    else
      g_main_win->addDockWidget(Qt::TopDockWidgetArea, dock, Qt::Vertical);

    // Qt steals the ownership of the plugin (QWidget)
    // Remove it from the smart pointer without calling the destructor
    plugin.release();

    count++;
  }
  g_plugins.clear();

  if (g_windowConfig.pos_x != 0 && g_windowConfig.pos_y != 0)
    g_main_win->move(g_windowConfig.pos_x, g_windowConfig.pos_y);

  if (g_windowConfig.width != 0 && g_windowConfig.height != 0)
    g_main_win->resize(g_windowConfig.width, g_windowConfig.height);

  if (!g_windowConfig.state.isEmpty())
    g_main_win->restoreState(g_windowConfig.state);

  return true;
}

/////////////////////////////////////////////////
ignition::gui::MainWindow *ignition::gui::mainWindow()
{
  return g_main_win;
}

/////////////////////////////////////////////////
std::vector<QDialog *> ignition::gui::dialogs()
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

  ignmsg << "Run main window" << std::endl;

  g_main_win->show();

  // Execute app
  g_app->exec();

  return true;
}

/////////////////////////////////////////////////
bool ignition::gui::runDialogs()
{
  if (!checkApp())
    return false;

  ignmsg << "Run dialogs" << std::endl;

  for (auto &plugin : g_plugins)
  {
    auto title = QString::fromStdString(plugin->Title());

    auto layout = new QVBoxLayout();
    layout->addWidget(plugin.get());

    // Qt steals the ownership of the plugin (QWidget)
    // Remove it from the smart pointer without calling the destructor
    plugin.release();

    auto dialog = new QDialog();
    dialog->setLayout(layout);
    dialog->setWindowTitle(title);
    dialog->setWindowModality(Qt::NonModal);
    dialog->setAttribute(Qt::WA_DeleteOnClose, true);

    g_dialogs.push_back(dialog);

    dialog->show();
    ignmsg << "Showing dialog [" << title.toStdString() << "]" << std::endl;
  }
  g_plugins.clear();

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

