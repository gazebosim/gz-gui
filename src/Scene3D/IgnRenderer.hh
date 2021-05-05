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
#ifndef IGNITION_GUI_IGNRENDERER_HH_
#define IGNITION_GUI_IGNRENDERER_HH_

#include <string>
#include <memory>

#include <ignition/common/MouseEvent.hh>

#include <ignition/math/Color.hh>
#include <ignition/math/Pose3.hh>
#include <ignition/math/Vector2.hh>
#include <ignition/math/Vector3.hh>

#include "ignition/gui/Application.hh"
#include "ignition/gui/qt.h"

#ifdef _MSC_VER
#pragma warning(push, 0)
#endif
#include <ignition/msgs.hh>
#ifdef _MSC_VER
#pragma warning(pop)
#endif

namespace ignition
{
namespace gui
{
  class IgnRendererPrivate;
/// \brief Ign-rendering renderer.
/// All ign-rendering calls should be performed inside this class as it makes
/// sure that opengl calls in the underlying render engine do not interfere
/// with QtQuick's opengl render operations. The main Render function will
/// render to an offscreen texture and notify via signal and slots when it's
/// ready to be displayed.
class IgnRenderer : public QObject
{
  Q_OBJECT
  ///  \brief Constructor
  public: IgnRenderer();

  ///  \brief Destructor
  public: ~IgnRenderer();

  ///  \brief Main render function
  public: void Render();

  /// \brief Initialize the render engine
  public: void Initialize();

  /// \brief Destroy camera associated with this renderer
  public: void Destroy();

  /// \brief New mouse event triggered
  /// \param[in] _e New mouse event
  /// \param[in] _drag Mouse move distance
  public: void NewMouseEvent(const common::MouseEvent &_e,
      const math::Vector2d &_drag = math::Vector2d::Zero);

  /// \brief Handle mouse event for view control
  private: void HandleMouseEvent();

  private: void BroadcastHoverPos();
  private: void HandleModelPlacement();

  /// \brief Handle mouse event for view control
  private: void HandleMouseViewControl();

  public: void SetFollowPGain(double _gain);

  public: void SetMoveTo(const std::string &_target);

  /// \brief Callback when a move to animation is complete
  private: void OnMoveToComplete();

  public: std::string FollowTarget() const;

  public: void UpdatePoses(std::unordered_map<long unsigned int, math::Pose3d> &_poses);

  public: void SetFollowTarget(const std::string &_target,
      bool _waitForTarget);

  public: void SetShowGrid(bool _grid);
  public: void SetModel(const msgs::Model &_model);
  public: void SetScene(const msgs::Scene &_scene);

  /// \brief True to set the camera to follow the target in world frame,
  /// false to follow in target's local frame
  /// \param[in] _gain Camera follow p gain.
  public: void SetFollowWorldFrame(bool _worldFrame);

  /// \brief Set the camera follow offset position
  /// \param[in] _offset Camera follow offset position.
  public: void SetFollowOffset(const math::Vector3d &_offset);

  /// \brief Retrieve the first point on a surface in the 3D scene hit by a
  /// ray cast from the given 2D screen coordinates.
  /// \param[in] _screenPos 2D coordinates on the screen, in pixels.
  /// \return 3D coordinates of a point in the 3D scene.
  private: math::Vector3d ScreenToScene(const math::Vector2i &_screenPos)
      const;

  /// \brief Render texture id
  public: GLuint textureId = 0u;

  /// \brief Render engine to use
  public: std::string engineName = "ogre";

  /// \brief Unique scene name
  public: std::string sceneName = "scene";

  /// \brief Initial Camera pose
  public: math::Pose3d cameraPose = math::Pose3d(0, 0, 2, 0, 0.4, 0);

  /// \brief Scene background color
  public: math::Color backgroundColor = math::Color::Black;

  /// \brief Ambient color
  public: math::Color ambientLight = math::Color(0.3f, 0.3f, 0.3f, 1.0f);

  /// \brief Sky
  public: bool sky = false;

  /// \brief Camera visibility mask
  public: uint32_t visibilityMask = 0xFFFFFFFFu;

  /// \brief True if engine has been initialized;
  public: bool initialized = false;

  /// \brief Render texture size
  public: QSize textureSize = QSize(1024, 1024);

  /// \brief Flag to indicate texture size has changed.
  public: bool textureDirty = false;

  /// \brief Scene service. If not empty, a request will be made to get the
  /// scene information using this service and the renderer will populate the
  /// scene based on the response data
  public: std::string sceneService;

  /// \brief Scene pose topic. If not empty, a node will subcribe to this
  /// topic to get pose updates of objects in the scene
  public: std::string poseTopic;

  /// \brief Ign-transport deletion topic name
  public: std::string deletionTopic;

  /// \brief Ign-transport scene topic name
  /// New scene messages will be published to this topic when an entities are
  /// added
  public: std::string sceneTopic;

  /// \internal
  /// \brief Pointer to private data.
  private: std::unique_ptr<IgnRendererPrivate> dataPtr;

  /// \brief When fired, the follow target changed. May not be fired for
  /// every target change.
  /// \param[in] _target Target to follow
  /// \param[in] _waitForTarget True to continuously look for the target
  signals: void FollowTargetChanged(const std::string &_target,
      bool _waitForTarget);
};
}
}
#endif
