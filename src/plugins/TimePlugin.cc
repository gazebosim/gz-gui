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
#include <ignition/common/PluginMacros.hh>

#include "ignition/gui/plugins/TimePlugin.hh"
#include "ignition/gui/widgets/Time.hh"

using namespace ignition;
using namespace gui;

/////////////////////////////////////////////////
TimePlugin::TimePlugin()
  : Plugin()
{
//  auto widget = new widgets::Time();

//  this->layout()->addWidget(widget);
}

/////////////////////////////////////////////////
TimePlugin::~TimePlugin()
{
}

// Register this plugin
IGN_COMMON_REGISTER_SINGLE_PLUGIN(ignition::gui::TimePlugin,
                                  ignition::gui::Plugin)
