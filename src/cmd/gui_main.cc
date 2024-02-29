/*
 * Copyright (C) 2023 Open Source Robotics Foundation
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

#include <gz/common/Console.hh>

#include <gz/gui/Application.hh>
#include <gz/gui/MainWindow.hh>

#include <gz/utils/cli/CLI.hpp>
#include <gz/utils/cli/GzFormatter.hpp>


//////////////////////////////////////////////////
/// \brief Enumeration of available commands
enum class GuiCommand
{
  kNone,
  kGuiStandalone,
};

//////////////////////////////////////////////////
struct GuiOptions
{
  GuiCommand command {GuiCommand::kNone};
};

//////////////////////////////////////////////////
void startConsoleLog()
{
  std::string home;
  gz::common::env(GZ_HOMEDIR, home);

  std::string logPathMod = gz::common::joinPaths(home,
      ".gz", "gui", "log",
      gz::common::timeToIso(GZ_SYSTEM_TIME()));
  gzLogInit(logPathMod, "console.log");

  gz::common::Console::SetVerbosity(4);
}

//////////////////////////////////////////////////
void runGuiCommand(const GuiOptions &_opt)
{
  startConsoleLog();

  int g_argc = 1;
  char* g_argv[] = {"./gz"};
  gz::gui::Application app(g_argc, g_argv);

  auto *mainWindow = app.findChild<gz::gui::MainWindow *>();
  // Set the render engine GUI name
  mainWindow->SetRenderEngine("ogre2");
  app.LoadPlugin("MinimalScene");

  gz::gui::Application::exec();
}

//////////////////////////////////////////////////
void addGuiFlags(CLI::App &_app)
{
  auto opt = std::make_shared<GuiOptions>();
  _app.callback([opt](){runGuiCommand(*opt); });
}

int main(int argc, char** argv)
{
  CLI::App app{"Gazebo GUI"};
  addGuiFlags(app);
  app.formatter(std::make_shared<GzFormatter>(&app));
  CLI11_PARSE(app, argc, argv);
}
