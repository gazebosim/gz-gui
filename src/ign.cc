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

#include <ignition/common/Console.hh>

#include "ignition/gui/Application.hh"
#include "ignition/gui/config.hh"
#include "ignition/gui/Export.hh"
#include "ignition/gui/ign.hh"
#include "ignition/gui/MainWindow.hh"

int g_argc = 1;
char* g_argv[] =
{
  reinterpret_cast<char*>(const_cast<char*>("./ignition")),
};

//////////////////////////////////////////////////
extern "C" IGNITION_GUI_VISIBLE char *ignitionVersion()
{
  std::string home;
  ignition::common::env(IGN_HOMEDIR, home);

  std::string recordPathMod = ignition::common::joinPaths(home,
        ".ignition", "gazebo", "log",ignition::common::timeToIso(IGN_SYSTEM_TIME()));
  ignLogInit(recordPathMod, "gui_console.log");


  return strdup(IGNITION_GUI_VERSION_FULL);
}

//////////////////////////////////////////////////
extern "C" IGNITION_GUI_VISIBLE void cmdPluginList()
{
  ignition::gui::Application app(g_argc, g_argv);

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
extern "C" IGNITION_GUI_VISIBLE void cmdStandalone(const char *_filename)
{
  ignition::gui::Application app(g_argc, g_argv,
      ignition::gui::WindowType::kDialog);

  if (!app.LoadPlugin(_filename))
  {
    return;
  }

  app.exec();
}

//////////////////////////////////////////////////
extern "C" IGNITION_GUI_VISIBLE void cmdConfig(const char *_config)
{
  ignition::gui::Application app(g_argc, g_argv);

  if (!app.findChild<ignition::gui::MainWindow *>())
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
extern "C" IGNITION_GUI_VISIBLE void cmdVerbose(const char *_verbosity)
{
  ignition::common::Console::SetVerbosity(std::atoi(_verbosity));
}

//////////////////////////////////////////////////
extern "C" IGNITION_GUI_VISIBLE void cmdEmptyWindow()
{
  ignition::gui::Application app(g_argc, g_argv);

  if (!app.findChild<ignition::gui::MainWindow *>())
  {
    return;
  }

  app.LoadDefaultConfig();

  app.exec();
}

//////////////////////////////////////////////////
extern "C" IGNITION_GUI_VISIBLE void cmdSetStyleFromFile(
    const char * /*_filename*/)
{
//  ignition::gui::setStyleFromFile(std::string(_filename));
}
