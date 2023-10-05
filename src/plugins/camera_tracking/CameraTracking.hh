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

#ifndef GZ_GUI_PLUGINS_CAMERATRACKING_HH_
#define GZ_GUI_PLUGINS_CAMERATRACKING_HH_

#include <memory>

#include "gz/gui/Plugin.hh"

namespace gz::gui::plugins
{
class CameraTrackingPrivate;

/// \brief This plugin provides camera tracking capabilities such as "move to"
/// and "follow".
///
/// Services:
/// * `/gui/move_to`: Move the user camera to look at a given target,
///                   identified by name.
/// * `/gui/move_to/pose`: Move the user camera to a given pose.
/// * `/gui/follow`: Set the user camera to follow a given target,
///                   identified by name.
/// * `/gui/follow/offset`: Set the offset for following.
///
/// Topics:
/// * `/gui/camera/pose`: Publishes the current user camera pose.
class CameraTracking : public Plugin
{
  Q_OBJECT

  /// \brief Constructor
  public: CameraTracking();

  /// \brief Destructor
  public: virtual ~CameraTracking();

  // Documentation inherited
  public: virtual void LoadConfig(const tinyxml2::XMLElement *_pluginElem)
      override;

  // Documentation inherited
  private: bool eventFilter(QObject *_obj, QEvent *_event) override;

  /// \internal
  /// \brief Pointer to private data.
  private: std::unique_ptr<CameraTrackingPrivate> dataPtr;
};
}  // namespace gz::gui::plugins
#endif  // GZ_GUI_PLUGINS_CAMERATRACKING_HH_
