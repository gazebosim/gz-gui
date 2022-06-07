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

#include <string.h>

#include <iostream>

#include <gz/common/Console.hh>

#include "gz/gui/Application.hh"
#include "gz/gui/config.hh"
#include "gz/gui/Export.hh"
#include "gz/gui/gz.hh"
#include "gz/gui/MainWindow.hh"

int g_argc = 1;
char* g_argv[] =
{
  reinterpret_cast<char*>(const_cast<char*>("./ignition")),
};

//////////////////////////////////////////////////
void startConsoleLog()
{
  std::string home;
  gz::common::env(GZ_HOMEDIR, home);

  std::string logPathMod = gz::common::joinPaths(home,
      ".ignition", "gui", "log",
      gz::common::timeToIso(GZ_SYSTEM_TIME()));
  gzLogInit(logPathMod, "console.log");
}

//////////////////////////////////////////////////
extern "C" GZ_GUI_VISIBLE char *gzVersion()
{
  return strdup(GZ_GUI_VERSION_FULL);
}

extern "C" GZ_GUI_VISIBLE char GZ_DEPRECATED(7) *ignitionVersion()
{
  return gzVersion();
}

//////////////////////////////////////////////////
extern "C" GZ_GUI_VISIBLE void cmdPluginList()
{
  startConsoleLog();

  gz::gui::Application app(g_argc, g_argv);

  auto pluginsList = app.PluginList();
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

//////////////////////////////////////////////////
extern "C" GZ_GUI_VISIBLE void cmdStandalone(const char *_filename)
{
  startConsoleLog();

  gz::gui::Application app(g_argc, g_argv,
      gz::gui::WindowType::kDialog);

  if (!app.LoadPlugin(_filename))
  {
    return;
  }

  app.exec();
}

//////////////////////////////////////////////////
extern "C" GZ_GUI_VISIBLE void cmdConfig(const char *_config)
{
  startConsoleLog();

  gz::gui::Application app(g_argc, g_argv);

  if (!app.findChild<gz::gui::MainWindow *>())
  {
    return;
  }

  if (!app.LoadConfig(std::string(_config)))
  {
    return;
  }

  app.exec();
}

//////////////////////////////////////////////////
extern "C" GZ_GUI_VISIBLE void cmdVerbose(const char *_verbosity)
{
  gz::common::Console::SetVerbosity(std::atoi(_verbosity));
}

//////////////////////////////////////////////////
extern "C" GZ_GUI_VISIBLE void cmdEmptyWindow()
{
  startConsoleLog();

  gz::gui::Application app(g_argc, g_argv);

  if (!app.findChild<gz::gui::MainWindow *>())
  {
    return;
  }

  app.LoadDefaultConfig();

  app.exec();
}

//////////////////////////////////////////////////
extern "C" GZ_GUI_VISIBLE void cmdSetStyleFromFile(
    const char * /*_filename*/)
{
//  gz::gui::setStyleFromFile(std::string(_filename));
}
