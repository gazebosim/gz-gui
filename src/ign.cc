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

#include "ignition/gui/Iface.hh"
#include "ignition/gui/ign.hh"
#include "ignition/gui/System.hh"

//////////////////////////////////////////////////
extern "C" IGNITION_GUI_VISIBLE char *ignitionVersion()
{
  return strdup("0.1.0");
}

//////////////////////////////////////////////////
extern "C" IGNITION_GUI_VISIBLE void cmdPluginList()
{
  ignition::gui::listPlugins();
}

//////////////////////////////////////////////////
extern "C" IGNITION_GUI_VISIBLE void cmdStandalone(const char *_filename)
{
  ignition::gui::runStandalone(std::string(_filename));
}

//////////////////////////////////////////////////
extern "C" IGNITION_GUI_VISIBLE void cmdConfig(const char *_config)
{
  ignition::gui::runConfig(std::string(_config));
}

//////////////////////////////////////////////////
extern "C" IGNITION_GUI_VISIBLE void cmdVerbose(const char *_verbosity)
{
  ignition::gui::setVerbosity(std::atoi(_verbosity));
}

//////////////////////////////////////////////////
extern "C" IGNITION_GUI_VISIBLE void cmdEmptyWindow()
{
  ignition::gui::runEmptyWindow();
}

//////////////////////////////////////////////////
extern "C" IGNITION_GUI_VISIBLE void cmdStyleSheet(const char *_filename)
{
  ignition::gui::setQssFile(std::string(_filename));
}
