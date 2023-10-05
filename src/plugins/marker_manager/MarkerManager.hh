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

#ifndef GZ_GUI_PLUGINS_MARKERMANAGER_HH_
#define GZ_GUI_PLUGINS_MARKERMANAGER_HH_

#include <memory>

#include "gz/gui/Plugin.hh"

namespace gz::gui::plugins
{
class MarkerManagerPrivate;

/// \brief This plugin will be in charge of handling the markers in the
/// scene. It will allow to add, modify or remove markers.
///
/// ## Parameters
///
/// * `<topic_name>`: Optional. Name of topic for marker service. Defaults
/// to `/marker`.
/// * `<stats_topic>`: Optional. Name of topic to receive world stats.
/// Defaults to `/world/[world name]/stats`.
/// * `<warn_on_action_failure>`: True to display warnings if the user
/// attempts to perform an invalid action. Defaults to true.
class MarkerManager : public Plugin
{
  Q_OBJECT

  /// \brief Constructor
  public: MarkerManager();

  /// \brief Destructor
  public: virtual ~MarkerManager();

  // Documentation inherited
  public: virtual void LoadConfig(const tinyxml2::XMLElement *_pluginElem)
      override;

  // Documentation inherited
  private: bool eventFilter(QObject *_obj, QEvent *_event) override;

  /// \internal
  /// \brief Pointer to private data.
  private: std::unique_ptr<MarkerManagerPrivate> dataPtr;
};
}  // namespace gz::gui::plugins
#endif  // GZ_GUI_PLUGINS_MARKERMANAGER_HH_
