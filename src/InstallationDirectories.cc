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

#include <gz/gui/config.hh>
#include <gz/gui/InstallationDirectories.hh>

#include <gz/common/Filesystem.hh>

namespace gz
{
namespace gui
{
inline namespace GZ_GUI_VERSION_NAMESPACE {

std::string getPluginInstallDir()
{
  return gz::common::joinPaths(
      getInstallPrefix(), GZ_GUI_PLUGIN_RELATIVE_INSTALL_DIR);
}

}
}
}