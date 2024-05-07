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

#include <mutex>
#include <string>

#include <gz/msgs/boolean.pb.h>
#include <gz/msgs/gui_camera.pb.h>
#include <gz/msgs/stringmsg.pb.h>
#include <gz/msgs/vector3d.pb.h>
#include <gz/msgs/cameratrack.pb.h>

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
class CameraTrackingPrivate
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

  /// \brief Callback for a track message
  /// \param[in] _msg Message consistes of the target to track, type of tracking, offset and pgain.
  public: void OnTrackSub(const msgs::CameraTrack &_msg);

  /// \brief Callback for a move to pose request.
  /// \param[in] _msg GUICamera request message.
  /// \param[in] _res Response data
  /// \return True if the request is received
  public: bool OnMoveToPose(const msgs::GUICamera &_msg,
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

  /// \brief Target to track
  public: std::string selectedTarget;

  /// \brief Wait for target to track
  public: bool selectedTargetWait = false;

  /// \brief Offset of camera from target being tracked
  public: math::Vector3d trackOffset = math::Vector3d(-5, 0, 3);

  public: gz::msgs::CameraTrack trackMsg;

  /// \brief Flag to indicate the tracking offset needs to be updated
  public: bool trackOffsetDirty = false;

  /// \brief Flag to indicate new tracking
  public: bool newTrack = true;

  /// \brief Track P gain
  public: double trackPGain = 0.01;

  /// \brief True track the target at an offset that is in world frame,
  /// false to track in target's local frame
  public: bool trackWorldFrame = false;

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

  /// \brief Track topic
  public: std::string trackTopic;

  /// \brief Track status topic
  public: std::string trackStatusTopic;

  /// \brief Camera pose topic
  public: std::string cameraPoseTopic;

  /// \brief Move to pose service
  public: std::string moveToPoseService;

  /// \brief Camera pose publisher
  public: transport::Node::Publisher trackStatusPub;

  /// \brief Camera pose publisher
  public: transport::Node::Publisher cameraPosePub;

  /// \brief Timer to keep publishing camera poses.
  public: QTimer *timer{nullptr};
};

/////////////////////////////////////////////////
void CameraTrackingPrivate::Initialize()
{
  // Attach to the first camera we find
  for (unsigned int i = 0; i < scene->NodeCount(); ++i)
  {
    auto cam = std::dynamic_pointer_cast<rendering::Camera>(
      scene->NodeByIndex(i));
    if (cam)
    {
      this->camera = cam;
      gzdbg << "CameraTrackingPrivate plugin is moving camera ["
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
      &CameraTrackingPrivate::OnMoveTo, this);
  gzmsg << "Move to service on ["
         << this->moveToService << "]" << std::endl;

  // track
  this->trackTopic = "/gui/track";
  this->node.Subscribe(this->trackTopic, 
      &CameraTrackingPrivate::OnTrackSub, this);
  gzmsg << "Tracking topic on ["
         << this->trackTopic << "]" << std::endl;

  // tracking status
  this->trackStatusTopic = "/gui/currently_tracked";
  this->trackStatusPub =
    this->node.Advertise<msgs::CameraTrack>(this->trackStatusTopic);
  gzmsg << "Tracking status topic on ["
         << this->trackStatusTopic << "]" << std::endl;

  // move to pose service
  this->moveToPoseService =
      "/gui/move_to/pose";
  this->node.Advertise(this->moveToPoseService,
      &CameraTrackingPrivate::OnMoveToPose, this);
  gzmsg << "Move to pose service on ["
         << this->moveToPoseService << "]" << std::endl;

  // camera position topic
  this->cameraPoseTopic = "/gui/camera/pose";
  this->cameraPosePub =
    this->node.Advertise<msgs::Pose>(this->cameraPoseTopic);
  gzmsg << "Camera pose topic advertised on ["
         << this->cameraPoseTopic << "]" << std::endl;
}

/////////////////////////////////////////////////
bool CameraTrackingPrivate::OnMoveTo(const msgs::StringMsg &_msg,
  msgs::Boolean &_res)
{
  std::lock_guard<std::mutex> lock(this->mutex);
  this->moveToTarget = _msg.data();

  _res.set_data(true);
  return true;
}

/////////////////////////////////////////////////
void CameraTrackingPrivate::OnTrackSub(const msgs::CameraTrack &_msg)
{
  std::lock_guard<std::mutex> lock(this->mutex);
  gzmsg << "Got new track message."<< std::endl;
  if (!_msg.target().empty())
  {
    this->selectedTarget = _msg.target(); 
  }
  else
  {
    gzmsg << "Target name empty."<< std::endl;
  }
  if (_msg.has_offset())
  {
    this->trackOffset = msgs::Convert(_msg.offset());
  }

  if (_msg.pgain() > 0.00001)
  {
    this->trackPGain = _msg.pgain();
  }

  this->newTrack = true;
  return;
}

/////////////////////////////////////////////////
void CameraTrackingPrivate::OnMoveToComplete()
{
  this->moveToTarget.clear();
}

/////////////////////////////////////////////////
void CameraTrackingPrivate::OnMoveToPoseComplete()
{
  this->moveToPoseValue.reset();
}

/////////////////////////////////////////////////
bool CameraTrackingPrivate::OnMoveToPose(const msgs::GUICamera &_msg,
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
void CameraTrackingPrivate::OnRender()
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
    GZ_PROFILE("CameraTrackingPrivate::OnRender MoveTo");
    if (!this->moveToTarget.empty())
    {
      if (this->moveToHelper.Idle())
      {
        rendering::NodePtr target = scene->NodeByName(
            this->moveToTarget);
        if (target)
        {
          this->moveToHelper.MoveTo(this->camera, target, 0.5,
              std::bind(&CameraTrackingPrivate::OnMoveToComplete, this));
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
    GZ_PROFILE("CameraTrackingPrivate::OnRender MoveToPose");
    if (this->moveToPoseValue)
    {
      if (this->moveToHelper.Idle())
      {
        this->moveToHelper.MoveTo(this->camera,
            *(this->moveToPoseValue),
            0.5, std::bind(&CameraTrackingPrivate::OnMoveToPoseComplete, this));
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

  // Track
  {
    GZ_PROFILE("CameraTrackingPrivate::OnRender Track");
    // reset track mode if target node got removed
    if (!this->selectedTarget.empty())
    {
      rendering::NodePtr target = this->scene->NodeByName(this->selectedTarget);
      if (!target && !this->selectedTargetWait)
      {
        this->camera->SetFollowTarget(nullptr);
        this->camera->SetTrackTarget(nullptr);
        this->selectedTarget.clear();
      }
    }

    if (!this->moveToTarget.empty())
      return;
    rendering::NodePtr selectedTargetTmp = this->camera->FollowTarget();
    if (!this->selectedTarget.empty())
    {
      rendering::NodePtr target = scene->NodeByName(
          this->selectedTarget);
      if (target)
      {
        if (!selectedTargetTmp || target != selectedTargetTmp
              || this->newTrack)
        {
          this->camera->SetFollowTarget(target,
              this->trackOffset,
              this->trackWorldFrame);
          this->camera->SetFollowPGain(this->trackPGain);

          this->camera->SetTrackTarget(target);
          // found target, no need to wait anymore
          this->newTrack = false;
          this->selectedTargetWait = false;
        }
        else if (this->trackOffsetDirty)
        {
          math::Vector3d offset =
              this->camera->WorldPosition() - target->WorldPosition();
          if (!this->trackWorldFrame)
          {
            offset = target->WorldRotation().RotateVectorReverse(offset);
          }
          this->camera->SetFollowOffset(offset);
          this->trackOffsetDirty = false;
        }
      }
      else if (!this->selectedTargetWait)
      {
        gzerr << "Unable to track target. Target: '"
               << this->selectedTarget << "' not found" << std::endl;
        this->selectedTarget.clear();
      }
    }
    else if (selectedTargetTmp)
    {
      this->camera->SetFollowTarget(nullptr);
      this->camera->SetTrackTarget(nullptr);
    }
  }
}

/////////////////////////////////////////////////
CameraTracking::CameraTracking()
  : dataPtr(new CameraTrackingPrivate)
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
    if (this->dataPtr->trackStatusPub.HasConnections())
    {
      this->dataPtr->trackMsg.set_target(this->dataPtr->selectedTarget);
      this->dataPtr->trackMsg.mutable_offset()->set_x(this->dataPtr->trackOffset.X());
      this->dataPtr->trackMsg.mutable_offset()->set_y(this->dataPtr->trackOffset.Y());
      this->dataPtr->trackMsg.mutable_offset()->set_z(this->dataPtr->trackOffset.Z());
      this->dataPtr->trackMsg.set_pgain(this->dataPtr->trackPGain);
      this->dataPtr->trackStatusPub.Publish(this->dataPtr->trackMsg);
    }
  });
  this->dataPtr->timer->setInterval(1000.0 / 50.0);
  this->dataPtr->timer->start();
}

/////////////////////////////////////////////////
CameraTracking::~CameraTracking() = default;

/////////////////////////////////////////////////
void CameraTracking::LoadConfig(const tinyxml2::XMLElement *_pluginElem)
{
  if (this->title.empty())
    this->title = "Camera tracking";

  if (_pluginElem)
  {
    if (auto nameElem = _pluginElem->FirstChildElement("target"))
    {
      this->dataPtr->selectedTarget = nameElem->GetText();
      gzmsg << "CameraTracking: Loaded target from sdf ["
            << this->dataPtr->selectedTarget << "]" << std::endl;
      this->dataPtr->selectedTargetWait = true;
    }
    if (auto offsetElem = _pluginElem->FirstChildElement("offset"))
    {
      std::stringstream offsetStr;
      offsetStr << std::string(offsetElem->GetText());
      offsetStr >> this->dataPtr->trackOffset;
      gzmsg << "CameraTracking: Loaded offset from sdf ["
            << this->dataPtr->trackOffset << "]" << std::endl;
      this->dataPtr->newTrack = true;
    }
    if (auto pGainElem = _pluginElem->FirstChildElement("pgain"))
    {
      this->dataPtr->trackPGain = std::stod(std::string(pGainElem->GetText()));
      gzmsg << "CameraTracking: Loaded pgain from sdf ["
            << this->dataPtr->trackPGain << "]" << std::endl;
      this->dataPtr->newTrack = true;
    }
  }

  App()->findChild<MainWindow *>()->installEventFilter(this);
}

/////////////////////////////////////////////////
void CameraTrackingPrivate::HandleKeyRelease(events::KeyReleaseOnScene *_e)
{
  if (_e->Key().Key() == Qt::Key_Escape)
  {
    if (!this->selectedTarget.empty())
    {
      this->selectedTarget = std::string();

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
