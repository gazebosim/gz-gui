/*
 * Copyright (C) 2021 Open Source Robotics Foundation
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

#include "ShutdownButton.hh"

#include <ignition/plugin/Register.hh>

#include "ignition/gui/Application.hh"
#include "ignition/gui/MainWindow.hh"

using namespace gz;
using namespace gui;
using namespace plugins;

/////////////////////////////////////////////////
ShutdownButton::ShutdownButton() : Plugin()
{
}

/////////////////////////////////////////////////
ShutdownButton::~ShutdownButton() = default;

/////////////////////////////////////////////////
void ShutdownButton::LoadConfig(const tinyxml2::XMLElement * /*_pluginElem*/)
{
  // Default name in case user didn't define one
  if (this->title.empty())
    this->title = "Shutdown";
}

/////////////////////////////////////////////////
void ShutdownButton::OnStop()
{
  gz::gui::App()->findChild<MainWindow *>()->QuickWindow()->close();
}

// Register this plugin
IGNITION_ADD_PLUGIN(gz::gui::plugins::ShutdownButton,
                    gz::gui::Plugin)
