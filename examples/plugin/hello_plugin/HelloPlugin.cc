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
#include <ignition/plugin/Register.hh>

#include "HelloPlugin.hh"

using namespace ignition;
using namespace gui;

/////////////////////////////////////////////////
HelloPlugin::HelloPlugin()
  : Plugin()
{
}

/////////////////////////////////////////////////
HelloPlugin::~HelloPlugin()
{
}

/////////////////////////////////////////////////
void HelloPlugin::LoadConfig(const tinyxml2::XMLElement *_pluginElem)
{
  if (!_pluginElem)
    return;

  // Take parameters from XML at runtime
  auto messageElem = _pluginElem->FirstChildElement("message");
  if (nullptr != messageElem && nullptr != messageElem->GetText())
    this->message = messageElem->GetText();
}

/////////////////////////////////////////////////
void HelloPlugin::OnButton()
{
  std::cout << this->message << std::endl;
}

// Register this plugin
IGNITION_ADD_PLUGIN(ignition::gui::HelloPlugin,
                    ignition::gui::Plugin);
