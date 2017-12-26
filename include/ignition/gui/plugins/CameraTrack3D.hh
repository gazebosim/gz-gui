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

#ifndef IGNITION_GUI_PLUGINS_CAMERATRACK3D_HH_
#define IGNITION_GUI_PLUGINS_CAMERATRACK3D_HH_

#include <memory>
#include <ignition/math/Vector2.hh>
#include <ignition/math/Vector3.hh>

#include "ignition/gui/qt.h"
#include "ignition/gui/Plugin.hh"

namespace ignition
{
namespace gui
{
namespace plugins
{
  class CameraTrack3DPrivate;

  /// \brief Provides an interface to set tracking and following settings for
  /// any camera in a given scene.
  ///
  /// ## Configuration
  ///
  /// \<engine\> : Optional render engine name, defaults to 'ogre'.
  /// \<scene\> : Optional scene name, defaults to 'scene'.
  class CameraTrack3D : public Plugin
  {
    Q_OBJECT

    /// \brief Constructor
    public: CameraTrack3D();

    /// \brief Destructor
    public: virtual ~CameraTrack3D();

    // Documentation inherited
    public: virtual void LoadConfig(const tinyxml2::XMLElement *_pluginElem)
        override;

    /// \brief Called when a new camera is chosen
    /// \param[in] _value New camera name
    private slots: void OnCameraChange(const QVariant &_value);

    /// \brief Called when a new track option is chosen
    /// \param[in] _value New option
    private slots: void OnTrackChange(const QVariant &_value);

    /// \brief Called when a new follow option is chosen
    /// \param[in] _value New option
    private slots: void OnFollowChange(const QVariant &_value);

    // Documentation inherited
    private: bool eventFilter(QObject *_obj, QEvent *_e) override;

    /// \brief Refresh the camera dropdown
    private: void RefreshCameraList();

    /// \brief Refresh the target dropdowns
    private: void RefreshTargetLists();

    /// \internal
    /// \brief Pointer to private data.
    private: std::unique_ptr<CameraTrack3DPrivate> dataPtr;
  };
}
}
}

#endif
