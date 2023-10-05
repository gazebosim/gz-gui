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

#ifndef GZ_GUI_PLUGINS_INTERACTIVEVIEWCONTROL_HH_
#define GZ_GUI_PLUGINS_INTERACTIVEVIEWCONTROL_HH_

#include <memory>

#include "gz/gui/Plugin.hh"

namespace gz::gui::plugins
{
class InteractiveViewControlPrivate;

/// \brief This Plugin allows to control a user camera with the mouse:
///
/// * Drag left button to pan
/// * Drag middle button to orbit
/// * Drag right button or scroll wheel to zoom
///
/// This plugin also supports changing between perspective and orthographic
/// projections through the `/gui/camera/view_control` service. Perspective
/// projection is used by default. For example:
///
///     gz service -s /gui/camera/view_control
///         --reqtype gz.msgs.StringMsg
///         --reptype gz.msgs.Boolean
///         --timeout 2000 --req 'data: "ortho"'
///
/// Supported options are:
///
/// * `orbit`: perspective projection
/// * `ortho`: orthographic projection
class InteractiveViewControl : public Plugin
{
  Q_OBJECT

  /// \brief Constructor
  public: InteractiveViewControl();

  /// \brief Destructor
  public: virtual ~InteractiveViewControl();

  // Documentation inherited
  public: virtual void LoadConfig(const tinyxml2::XMLElement *_pluginElem)
      override;

  // Documentation inherited
  private: bool eventFilter(QObject *_obj, QEvent *_event) override;

  /// \internal
  /// \brief Pointer to private data.
  private: std::unique_ptr<InteractiveViewControlPrivate> dataPtr;
};
}  // namespace gz::gui::plugins
#endif  // GZ_GUI_PLUGINS_INTERACTIVEVIEWCONTROL_HH_
