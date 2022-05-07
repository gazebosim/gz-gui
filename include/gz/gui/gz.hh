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

#ifndef IGNITION_GUI_IGN_HH_
#define IGNITION_GUI_IGN_HH_

#include "ignition/gui/Export.hh"

/// \brief External hook to read the library version.
/// \return C-string representing the version. Ex.: 0.1.2
extern "C" IGNITION_GUI_VISIBLE char *ignitionVersion();

/// \brief External hook to execute 'ign gui -l' from the command line.
extern "C" IGNITION_GUI_VISIBLE void cmdPluginList();

/// \brief External hook to execute 'ign gui -s' from the command line.
/// \param[in] _filename Name of a plugin file.
extern "C" IGNITION_GUI_VISIBLE void cmdStandalone(const char *_filename);

/// \brief External hook to execute 'ign gui -c' from the command line.
/// \param[in] _config Path to a config file.
extern "C" IGNITION_GUI_VISIBLE void cmdConfig(const char *_config);

/// \brief External hook to execute 'ign gui' from the command line.
extern "C" IGNITION_GUI_VISIBLE void cmdEmptyWindow();

/// \brief External hook when executing 'ign gui -t' from the command line.
/// \param[in] _filename Path to a QSS file.
extern "C" IGNITION_GUI_VISIBLE void cmdSetStyleFromFile(const char *_filename);

#endif
