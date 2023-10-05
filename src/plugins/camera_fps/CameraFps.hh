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

#ifndef GZ_GUI_PLUGINS_CAMERAFPS_HH_
#define GZ_GUI_PLUGINS_CAMERAFPS_HH_

#include <memory>

#include "gz/gui/Plugin.hh"

namespace gz::gui::plugins
{
/// Forward declarations
class CameraFpsPrivate;

/// \brief This plugin displays the GUI camera's Framerate Per Second (FPS)
class CameraFps : public Plugin
{
  Q_OBJECT

  /// \brief Camera frames per second
  Q_PROPERTY(
    QString cameraFPSValue
    READ CameraFpsValue
    WRITE SetCameraFpsValue
    NOTIFY CameraFpsValueChanged
  )

  /// \brief Constructor
  public: CameraFps();

  /// \brief Destructor
  public: virtual ~CameraFps();

  // Documentation inherited
  public: virtual void LoadConfig(const tinyxml2::XMLElement *_pluginElem)
      override;

  /// \brief Set the camera FPS value string
  /// \param[in] _value Camera FPS value string
  public: Q_INVOKABLE void SetCameraFpsValue(const QString &_value);

  /// \brief Get the camera FPS value string
  /// \return Camera FPS value string
  public: Q_INVOKABLE QString CameraFpsValue() const;

  /// \brief Notify that camera FPS value has changed
  signals: void CameraFpsValueChanged();

  /// \brief Perform rendering calls in the rendering thread.
  private: void OnRender();

  // Documentation inherited
  private: bool eventFilter(QObject *_obj, QEvent *_event) override;

  /// \internal
  /// \brief Pointer to private data.
  private: std::unique_ptr<CameraFpsPrivate> dataPtr;
};
}  // namespace gz::gui::plugins
#endif  // GZ_GUI_PLUGINS_CAMERAFPS_HH_
