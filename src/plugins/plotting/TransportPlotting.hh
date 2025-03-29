/*
 * Copyright (C) 2020 Open Source Robotics Foundation
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

#ifndef GZ_GUI_PLUGINS_TRANSPORTPLOTTING_HH_
#define GZ_GUI_PLUGINS_TRANSPORTPLOTTING_HH_

#include <gz/gui/Plugin.hh>
#include <gz/utils/ImplPtr.hh>

namespace gz::gui::plugins
{
/// \brief Plots fields from Gazebo Transport topics.
/// Fields can be dragged from the Topic Viewer or the Component Inspector.
class TransportPlotting : public gz::gui::Plugin
{
  Q_OBJECT

  /// \brief Constructor
  public: TransportPlotting();

  /// \brief Destructor
  public: ~TransportPlotting() override;

  // Documentation inherited
  public: void LoadConfig(const tinyxml2::XMLElement *) override;

  /// \brief Private data member.
  private: GZ_UTILS_UNIQUE_IMPL_PTR(dataPtr)
};
}  // namespace gz::gui::plugins

#endif  // GZ_GUI_PLUGINS_TRANSPORTPLOTTING_HH_
