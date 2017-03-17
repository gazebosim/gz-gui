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

#include <iostream>
#include <signal.h>
#include <stdio.h>
#include <tinyxml2.h>

#include "ignition/gui/qt.h"
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

QApplication *g_app;
MainWindow *g_main_win = nullptr;
std::vector<std::shared_ptr<GUIPlugin>> g_plugins;

/////////////////////////////////////////////////
bool ignition::gui::load()
{
  std::cout << "Load Iface" << std::endl;

  // Create app
  g_app = new QApplication(g_argc, g_argv);

  // Create main window
  g_main_win = new MainWindow();

  // Install signal handler for graceful shutdown
  installSignalHandler();

  return true;
}

/////////////////////////////////////////////////
bool ignition::gui::run(int /*_argc*/, char ** /*_argv*/)
{
  std::cout << "Run Iface" << std::endl;
  if (!ignition::gui::load())
    return false;

  // Execute app
  g_app->exec();

  stop();

  return true;
}

/////////////////////////////////////////////////
bool ignition::gui::installSignalHandler()
{
#ifndef _WIN32
  // Install a signal handler to allow for graceful shutdown on Ctrl-C.

  if (!g_app)
  {
    std::cerr << "Please create an app before installing the signal handler"
              << std::endl;
    return false;
  }

  auto handler = [](int)
      {
        stop();
      };

  struct sigaction sigact;
  sigact.sa_flags = 0;
  sigact.sa_handler = handler;

  if (sigemptyset(&sigact.sa_mask) != 0)
  {
    std::cerr << "sigemptyset failed while setting up for SIGINT" << std::endl;
    return false;
  }

  if (sigaction(SIGINT, &sigact, nullptr))
  {
    std::cerr << "signal(2) failed while setting up for SIGINT" << std::endl;
    return false;
  }
#endif

  return true;
}

/////////////////////////////////////////////////
void ignition::gui::stop()
{
  std::cout << "Stop" << std::endl;

  g_plugins.clear();
  g_app->quit();
}

/////////////////////////////////////////////////
bool ignition::gui::standalonePlugin(const std::string &_filename)
{
  std::cout << "Loading standalone plugin" << std::endl;

  if (_filename.empty())
  {
    std::cerr << "Missing plugin filename" << std::endl;
    return false;
  }

  g_app = new QApplication(g_argc, g_argv);

  installSignalHandler();

  auto plugin = ignition::gui::GUIPlugin::Create(_filename, "hello_plugin");

  if (!plugin)
  {
    std::cerr << "Failed to load plugin [" << _filename << "]" << std::endl;
    return false;
  }

  auto layout = new QVBoxLayout();
  layout->addWidget(plugin.get());

  auto dialog = new QDialog();
  dialog->setLayout(layout);

  dialog->exec();

  return true;
}

/////////////////////////////////////////////////
bool ignition::gui::loadConfig(const std::string &_config)
{
  std::cout << "Loading config file [" << _config << "]" << std::endl;

  if (_config.empty())
  {
    std::cerr << "Missing config file" << std::endl;
    return false;
  }

  // Open file
  auto file = fopen(_config.c_str() , "r");
  if (!file)
  {
    std::cout << "Failed to open file [" << _config << "]" << std::endl;
    return false;
  }

  // Use tinyxml to read config
  tinyxml2::XMLDocument doc;
  auto success = !doc.LoadFile(file);
  if (!success)
  {
    std::cout << "Failed to load file [" << _config << "]: XMLError"
              << std::endl;
    return false;
  }

  // Start application
  load();

  // Process each plugin
  for(auto pluginElem = doc.FirstChildElement("plugin"); pluginElem != nullptr;
      pluginElem = pluginElem->NextSiblingElement("plugin"))
  {
    // Load plugin
    auto filename = pluginElem->Attribute("filename");

    auto plugin = ignition::gui::GUIPlugin::Create(filename, "hello_plugin");
    if (!plugin)
    {
      std::cerr << "Failed to load plugin [" << filename << "]" << std::endl;
      return false;
    }

    // Create dock widget
    auto name = pluginElem->Attribute("name");

    auto dock = new QDockWidget(name, g_main_win);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea |
                          Qt::RightDockWidgetArea);
    dock->setWidget(&*plugin);
    g_main_win->addDockWidget(Qt::LeftDockWidgetArea, dock);

    // Store plugin in list
    g_plugins.push_back(plugin);
  }

  g_app->exec();

  stop();

  return true;
}

