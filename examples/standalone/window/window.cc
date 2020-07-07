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

#ifndef Q_MOC_RUN
  #include <ignition/gui/qt.h>
  #include <ignition/gui/Application.hh>
  #include <ignition/gui/MainWindow.hh>
#endif

//////////////////////////////////////////////////
int main(int _argc, char **_argv)
{
  std::cout << "Hello, GUI!" << std::endl;

  // Increase verboosity so we see all messages
  ignition::common::Console::SetVerbosity(4);

  // Create app
  ignition::gui::Application app(_argc, _argv);

  // Load plugins / config
  if (!app.LoadPlugin("Scene3D"))
  {
    return 1;
  }

  // Customize main window
  auto win = app.findChild<ignition::gui::MainWindow *>()->QuickWindow();
  win->setProperty("title", "Hello Window!");

  // Run window
  app.exec();

  std::cout << "After run" << std::endl;

  return 0;
}

