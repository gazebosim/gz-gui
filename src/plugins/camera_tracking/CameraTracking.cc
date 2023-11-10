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

#include <gz/utils/ImplPtr.hh>
#include <mutex>
#include <string>

#include <gz/msgs/boolean.pb.h>
#include <gz/msgs/gui_camera.pb.h>
#include <gz/msgs/stringmsg.pb.h>
#include <gz/msgs/vector3d.pb.h>

#include <gz/common/Console.hh>
#include <gz/common/Profiler.hh>
#include <gz/msgs/Utility.hh>
#include <gz/plugin/Register.hh>
#include <gz/rendering/Camera.hh>
#include <gz/rendering/MoveToHelper.hh>
#include <gz/rendering/RenderingIface.hh>
#include <gz/rendering/Scene.hh>

#include "gz/gui/Application.hh"
#include "gz/gui/Conversions.hh"
#include "gz/gui/GuiEvents.hh"
#include "gz/gui/MainWindow.hh"

#include <gz/transport/Node.hh>

#include "CameraTracking.hh"

namespace gz::gui::plugins
{
/// \brief Private data class for CameraTracking
class CameraTracking::Implementation
{
  /// \brief Perform rendering calls in the rendering thread.
  public: void OnRender();

  /// \brief Initialize transport
  public: void Initialize();

  /// \brief Callback for a move to request
  /// \param[in] _msg Request message to set the target to move to.
  /// \param[in] _res Response data
  /// \return True if the request is received
  public: bool OnMoveTo(const msgs::StringMsg &_msg,
      msgs::Boolean &_res);

  /// \brief Callback for a follow request
  /// \param[in] _msg Request message to set the target to follow.
  /// \param[in] _res Response data
  /// \return True if the request is received
  public: bool OnFollow(const msgs::StringMsg &_msg,
      msgs::Boolean &_res);

  /// \brief Callback for a move to pose request.
  /// \param[in] _msg GUICamera request message.
  /// \param[in] _res Response data
  /// \return True if the request is received
  public: bool OnMoveToPose(const msgs::GUICamera &_msg,
               msgs::Boolean &_res);

  /// \brief Callback for a follow offset request
  /// \param[in] _msg Request message to set the camera's follow offset.
  /// \param[in] _res Response data
  /// \return True if the request is received
  public: bool OnFollowOffset(const msgs::Vector3d &_msg,
               msgs::Boolean &_res);

  /// \brief Callback when a move to animation is complete
  private: void OnMoveToComplete();

  /// \brief Callback when a move to  pose animation is complete
  private: void OnMoveToPoseComplete();

  /// \brief Process key releases
  /// \param[in] _e Key release event
  public: void HandleKeyRelease(events::KeyReleaseOnScene *_e);

  /// \brief Protects variable changed through services.
  public: std::mutex mutex;

  //// \brief Pointer to the rendering scene
  public: rendering::ScenePtr scene = nullptr;

  /// \brief Target to follow
  public: std::string followTarget;

  /// \brief Wait for follow target
  public: bool followTargetWait = false;

  /// \brief Offset of camera from target being followed
  public: math::Vector3d followOffset = math::Vector3d(-5, 0, 3);

  /// \brief Flag to indicate the follow offset needs to be updated
  public: bool followOffsetDirty = false;

  /// \brief Flag to indicate the follow offset has been updated
  public: bool newFollowOffset = true;

  /// \brief Follow P gain
  public: double followPGain = 0.01;

  /// \brief True follow the target at an offset that is in world frame,
  /// false to follow in target's local frame
  public: bool followWorldFrame = false;

  /// \brief Last move to animation time
  public: std::chrono::time_point<std::chrono::system_clock> prevMoveToTime;

  /// \brief User camera
  public: rendering::CameraPtr camera{nullptr};

  /// \brief Target to move the user camera to
  public: std::string moveToTarget;

  /// \brief Helper object to move user camera
  public: gz::rendering::MoveToHelper moveToHelper;

  /// \brief Transport node
  public: transport::Node node;

  /// \brief Move to service
  public: std::string moveToService;

  /// \brief The pose set from the move to pose service.
  public: std::optional<math::Pose3d> moveToPoseValue;

  /// \brief Follow service
  public: std::string followService;

  /// \brief Follow offset service
  public: std::string followOffsetService;

  /// \brief Camera pose topic
  public: std::string cameraPoseTopic;

  /// \brief Move to pose service
  public: std::string moveToPoseService;

  /// \brief Camera pose publisher
  public: transport::Node::Publisher cameraPosePub;

  /// \brief Timer to keep publishing camera poses.
  public: QTimer *timer{nullptr};
};

/////////////////////////////////////////////////
void CameraTracking::Implementation::Initialize()
{
  // Attach to the first camera we find
  for (unsigned int i = 0; i < scene->NodeCount(); ++i)
  {
    auto cam = std::dynamic_pointer_cast<rendering::Camera>(
      scene->NodeByIndex(i));
    if (cam)
    {
      this->camera = cam;
      gzdbg << "CameraTracking plugin is moving camera ["
             << this->camera->Name() << "]" << std::endl;
      break;
    }
  }
  if (!this->camera)
  {
    gzerr << "Camera is not available" << std::endl;
    return;
  }

  // move to
  this->moveToService = "/gui/move_to";
  this->node.Advertise(this->moveToService,
      &Implementation::OnMoveTo, this);
  gzmsg << "Move to service on ["
         << this->moveToService << "]" << std::endl;

  // follow
  this->followService = "/gui/follow";
  this->node.Advertise(this->followService,
      &Implementation::OnFollow, this);
  gzmsg << "Follow service on ["
         << this->followService << "]" << std::endl;

  // move to pose service
  this->moveToPoseService =
      "/gui/move_to/pose";
  this->node.Advertise(this->moveToPoseService,
      &Implementation::OnMoveToPose, this);
  gzmsg << "Move to pose service on ["
         << this->moveToPoseService << "]" << std::endl;

  // camera position topic
  this->cameraPoseTopic = "/gui/camera/pose";
  this->cameraPosePub =
    this->node.Advertise<msgs::Pose>(this->cameraPoseTopic);
  gzmsg << "Camera pose topic advertised on ["
         << this->cameraPoseTopic << "]" << std::endl;

  // follow offset
  this->followOffsetService = "/gui/follow/offset";
  this->node.Advertise(this->followOffsetService,
      &Implementation::OnFollowOffset, this);
  gzmsg << "Follow offset service on ["
       << this->followOffsetService << "]" << std::endl;
}

/////////////////////////////////////////////////
bool CameraTracking::Implementation::OnMoveTo(const msgs::StringMsg &_msg,
  msgs::Boolean &_res)
{
  std::lock_guard<std::mutex> lock(this->mutex);
  this->moveToTarget = _msg.data();

  _res.set_data(true);
  return true;
}

/////////////////////////////////////////////////
bool CameraTracking::Implementation::OnFollow(const msgs::StringMsg &_msg,
  msgs::Boolean &_res)
{
  std::lock_guard<std::mutex> lock(this->mutex);
  this->followTarget = _msg.data();

  _res.set_data(true);
  return true;
}

/////////////////////////////////////////////////
void CameraTracking::Implementation::OnMoveToComplete()
{
  this->moveToTarget.clear();
}

/////////////////////////////////////////////////
void CameraTracking::Implementation::OnMoveToPoseComplete()
{
  this->moveToPoseValue.reset();
}

/////////////////////////////////////////////////
bool CameraTracking::Implementation::OnFollowOffset(const msgs::Vector3d &_msg,
  msgs::Boolean &_res)
{
  std::lock_guard<std::mutex> lock(this->mutex);
  if (!this->followTarget.empty())
  {
    this->newFollowOffset = true;
    this->followOffset = msgs::Convert(_msg);
  }

  _res.set_data(true);
  return true;
}

/////////////////////////////////////////////////
bool CameraTracking::Implementation::OnMoveToPose(const msgs::GUICamera &_msg,
  msgs::Boolean &_res)
{
  std::lock_guard<std::mutex> lock(this->mutex);
  math::Pose3d pose = msgs::Convert(_msg.pose());

  // If there is no orientation in the message, then set a Rot value in the
  // math::Pose3d object to infinite. This will prevent the orientation from
  // being used when positioning the camera.
  // See the MoveToHelper::MoveTo function
  if (!_msg.pose().has_orientation())
    pose.Rot().X() = math::INF_D;

  // If there is no position in the message, then set a Pos value in the
  // math::Pose3d object to infinite. This will prevent the orientation from
  // being used when positioning the camera.
  // See the MoveToHelper::MoveTo function
  if (!_msg.pose().has_position())
    pose.Pos().X() = math::INF_D;

  this->moveToPoseValue = pose;

  _res.set_data(true);
  return true;
}

/////////////////////////////////////////////////
void CameraTracking::Implementation::OnRender()
{
  std::lock_guard<std::mutex> lock(this->mutex);

  if (nullptr == this->scene)
  {
    this->scene = rendering::sceneFromFirstRenderEngine();
    if (nullptr == this->scene)
      return;

    this->Initialize();
  }

  if (!this->camera)
    return;

  // Move To
  {
    GZ_PROFILE("CameraTracking::Implementation::OnRender MoveTo");
    if (!this->moveToTarget.empty())
    {
      if (this->moveToHelper.Idle())
      {
        rendering::NodePtr target = scene->NodeByName(
            this->moveToTarget);
        if (target)
        {
          this->moveToHelper.MoveTo(this->camera, target, 0.5,
              std::bind(&Implementation::OnMoveToComplete, this));
          this->prevMoveToTime = std::chrono::system_clock::now();
        }
        else
        {
          gzerr << "Unable to move to target. Target: '"
                 << this->moveToTarget << "' not found" << std::endl;
          this->moveToTarget.clear();
        }
      }
      else
      {
        auto now = std::chrono::system_clock::now();
        std::chrono::duration<double> dt = now - this->prevMoveToTime;
        this->moveToHelper.AddTime(dt.count());
        this->prevMoveToTime = now;
      }
    }
  }

  // Move to pose
  {
    GZ_PROFILE("CameraTracking::Implementation::OnRender MoveToPose");
    if (this->moveToPoseValue)
    {
      if (this->moveToHelper.Idle())
      {
        this->moveToHelper.MoveTo(this->camera,
            *(this->moveToPoseValue),
            0.5, std::bind(&Implementation::OnMoveToPoseComplete, this));
        this->prevMoveToTime = std::chrono::system_clock::now();
      }
      else
      {
        auto now = std::chrono::system_clock::now();
        std::chrono::duration<double> dt = now - this->prevMoveToTime;
        this->moveToHelper.AddTime(dt.count());
        this->prevMoveToTime = now;
      }
    }
  }

  // Follow
  {
    GZ_PROFILE("CameraTracking::Implementation::OnRender Follow");
    // reset follow mode if target node got removed
    if (!this->followTarget.empty())
    {
      rendering::NodePtr target = this->scene->NodeByName(this->followTarget);
      if (!target && !this->followTargetWait)
      {
        this->camera->SetFollowTarget(nullptr);
        this->camera->SetTrackTarget(nullptr);
        this->followTarget.clear();
      }
    }

    if (!this->moveToTarget.empty())
      return;
    rendering::NodePtr followTargetTmp = this->camera->FollowTarget();
    if (!this->followTarget.empty())
    {
      rendering::NodePtr target = scene->NodeByName(
          this->followTarget);
      if (target)
      {
        if (!followTargetTmp || target != followTargetTmp
              || this->newFollowOffset)
        {
          this->camera->SetFollowTarget(target,
              this->followOffset,
              this->followWorldFrame);
          this->camera->SetFollowPGain(this->followPGain);

          this->camera->SetTrackTarget(target);
          // found target, no need to wait anymore
          this->newFollowOffset = false;
          this->followTargetWait = false;
        }
        else if (this->followOffsetDirty)
        {
          math::Vector3d offset =
              this->camera->WorldPosition() - target->WorldPosition();
          if (!this->followWorldFrame)
          {
            offset = target->WorldRotation().RotateVectorReverse(offset);
          }
          this->camera->SetFollowOffset(offset);
          this->followOffsetDirty = false;
        }
      }
      else if (!this->followTargetWait)
      {
        gzerr << "Unable to follow target. Target: '"
               << this->followTarget << "' not found" << std::endl;
        this->followTarget.clear();
      }
    }
    else if (followTargetTmp)
    {
      this->camera->SetFollowTarget(nullptr);
      this->camera->SetTrackTarget(nullptr);
    }
  }
}

/////////////////////////////////////////////////
CameraTracking::CameraTracking()
  : dataPtr(gz::utils::MakeUniqueImpl<Implementation>())
{
  this->dataPtr->timer = new QTimer(this);
  this->connect(this->dataPtr->timer, &QTimer::timeout, [=]()
  {
    std::lock_guard<std::mutex> lock(this->dataPtr->mutex);
    if (!this->dataPtr->camera)
     return;
    if (this->dataPtr->cameraPosePub.HasConnections())
    {
      auto poseMsg = msgs::Convert(this->dataPtr->camera->WorldPose());
      this->dataPtr->cameraPosePub.Publish(poseMsg);
    }
  });
  this->dataPtr->timer->setInterval(1000.0 / 50.0);
  this->dataPtr->timer->start();
}

/////////////////////////////////////////////////
CameraTracking::~CameraTracking() = default;

/////////////////////////////////////////////////
void CameraTracking::LoadConfig(const tinyxml2::XMLElement *)
{
  if (this->title.empty())
    this->title = "Camera tracking";

  App()->findChild<MainWindow *>()->installEventFilter(this);
}

/////////////////////////////////////////////////
void CameraTracking::Implementation::HandleKeyRelease(
  events::KeyReleaseOnScene *_e)
{
  if (_e->Key().Key() == Qt::Key_Escape)
  {
    if (!this->followTarget.empty())
    {
      this->followTarget = std::string();

      _e->accept();
    }
  }
}

/////////////////////////////////////////////////
bool CameraTracking::eventFilter(QObject *_obj, QEvent *_event)
{
  if (_event->type() == events::Render::kType)
  {
    this->dataPtr->OnRender();
  }
  else if (_event->type() == events::KeyReleaseOnScene::kType)
  {
    events::KeyReleaseOnScene *keyEvent =
      static_cast<events::KeyReleaseOnScene*>(_event);
    if (keyEvent)
    {
      this->dataPtr->HandleKeyRelease(keyEvent);
    }
  }
  // Standard event processing
  return QObject::eventFilter(_obj, _event);
}
}  // namespace gz::gui::plugins

// Register this plugin
GZ_ADD_PLUGIN(gz::gui::plugins::CameraTracking,
              gz::gui::Plugin)
