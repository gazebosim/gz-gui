/*
 * Copyright (C) 2018 Open Source Robotics Foundation
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

#include <ignition/common/Console.hh>
#include <ignition/common/PluginMacros.hh>

#include "TestDisplayPlugin.hh"

using namespace ignition;
using namespace gui;

/////////////////////////////////////////////////
TestDisplayPlugin::TestDisplayPlugin()
  : DisplayPlugin()
{
}

/////////////////////////////////////////////////
TestDisplayPlugin::~TestDisplayPlugin()
{
}

/////////////////////////////////////////////////
void TestDisplayPlugin::Initialize(const tinyxml2::XMLElement */*_pluginElem*/)
{
  if (auto scenePtr = this->Scene().lock())
  {
    this->Visual()->AddGeometry(scenePtr->CreateBox());
  }
  else
  {
    ignerr << "Scene invalid. TestDisplayPlugin not initialized." << std::endl;
  }
}


/////////////////////////////////////////////////
std::string TestDisplayPlugin::Type() const
{
  return "TestDisplayPlugin";
}

// Register this plugin
IGN_COMMON_REGISTER_SINGLE_PLUGIN(ignition::gui::TestDisplayPlugin,
                                  ignition::gui::DisplayPlugin)
