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
#ifndef IGNITION_GUI_SCENE3DINTERFACE_HH_
#define IGNITION_GUI_SCENE3DINTERFACE_HH_

#include <memory>
#include <string>

#include "ignition/gui/Export.hh"

#include <ignition/math/Pose3.hh>
#include <ignition/math/Color.hh>

#include <ignition/msgs/boolean.pb.h>
#include <ignition/msgs/stringmsg.pb.h>

#include <QQuickItem>

namespace ignition
{
namespace gui
{
  class Scene3DInterfacePrivate;
  /// \brief Creates a new ignition rendering scene or adds a user-camera to an
  /// existing scene. It is possible to orbit the camera around the scene with
  /// the mouse. Use other plugins to manage objects in the scene.
  ///
  /// ## Configuration
  ///
  /// * \<engine\> : Optional render engine name, defaults to 'ogre'.
  /// * \<scene\> : Optional scene name, defaults to 'scene'. The plugin will
  ///               create a scene with this name if there isn't one yet. If
  ///               there is already one, a new camera is added to it.
  /// * \<ambient_light\> : Optional color for ambient light, defaults to
  ///                       (0.3, 0.3, 0.3, 1.0)
  /// * \<background_color\> : Optional background color, defaults to
  ///                          (0.3, 0.3, 0.3, 1.0)
  /// * \<camera_pose\> : Optional starting pose for the camera, defaults to
  ///                     (0, 0, 5, 0, 0, 0)
  class IGNITION_GUI_VISIBLE Scene3DInterface
  {
    /// \brief Constructor
    public: explicit Scene3DInterface();

    /// \brief Destructor
    public: ~Scene3DInterface();

    public: void SetPluginItem(QQuickItem * pluginItem);
    public: void SetFullScreen(bool _fullscreen);
    public: void SetEngineName(const std::string _name);
    public: void SetSceneName(const std::string _name);
    public: void SetAmbientLight(const math::Color ambient);
    public: void SetBackgroundColor(const math::Color bgColor);
    public: void SetCameraPose(const math::Pose3d pose);
    public: void SetSceneService(const std::string service);
    public: void SetPoseTopic(const std::string topic);
    public: void SetDeletionTopic(const std::string topic);
    public: void SetSceneTopic(const std::string topic);
    public: void SetSkyEnabled(const bool sky);
    public: void SetFollowPGain(const double gain);
    public: void SetFollowTarget(const std::string &_target,
        bool _waitForTarget = false);

    /// \brief True to set the camera to follow the target in world frame,
    /// false to follow in target's local frame
    /// \param[in] _gain Camera follow p gain.
    public: void SetFollowWorldFrame(bool _worldFrame);

    /// \brief Set the camera follow offset position
    /// \param[in] _offset Camera follow offset position.
    public: void SetFollowOffset(const math::Vector3d &_offset);

    /// \brief Set the user camera visibility mask
    /// \param[in] _mask Visibility mask to set to
    public: void SetVisibilityMask(uint32_t _mask);

    /// \brief Callback for a move to request
    /// \param[in] _msg Request message to set the target to move to.
    /// \param[in] _res Response data
    /// \return True if the request is received
    private: bool OnMoveTo(const msgs::StringMsg &_msg,
        msgs::Boolean &_res);

    /// \brief Callback for a follow request
    /// \param[in] _msg Request message to set the target to follow.
    /// \param[in] _res Response data
    /// \return True if the request is received
    private: bool OnFollow(const msgs::StringMsg &_msg,
        msgs::Boolean &_res);

    /// \internal
    /// \brief Pointer to private data.
    private: std::unique_ptr<Scene3DInterfacePrivate> dataPtr;
  };
}
}

#endif
