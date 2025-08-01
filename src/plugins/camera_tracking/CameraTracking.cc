/*
 * Copyright (C) 2021 Open Source Robotics Foundation
 * Copyright (C) 2024 Rudis Laboratories LLC
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
#include <sstream>
#include <string>
#include <optional>

#include <gz/msgs/boolean.pb.h>
#include <gz/msgs/cameratrack.pb.h>
#include <gz/msgs/entity.pb.h>
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

  /// \brief Callback for a track message
  /// \param[in] _msg Message is of type CameraTrack.
  public: void OnTrackSub(const msgs::CameraTrack &_msg);

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

  /// \brief Target to track
  public: std::string selectedTrackTarget;

  /// \brief Target to follow
  public: std::string selectedFollowTarget;

  /// \brief Wait for target to track
  public: bool selectedTargetWait = false;

  /// \brief Offset of camera from target being followed
  public: math::Vector3d followOffset = math::Vector3d(-3, 0, 2);

  /// \brief Offset on target to be tracked
  public: math::Vector3d trackOffset = math::Vector3d(0, 0, 0);

  /// \brief Camera tracking message
  public: gz::msgs::CameraTrack trackMsg;

  /// \brief Flag to indicate new tracking
  public: bool newTrack = true;

  /// \brief Track P gain
  public: double trackPGain = 0.01;

  /// \brief Follow P gain
  public: double followPGain = 0.01;

  /// \brief Free Look P gain
  public: double freeLookPGain = 1.0;

  /// \brief Default track mode to None
  public: int trackMode = gz::msgs::CameraTrack::NONE;

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

  /// \brief Follow service (deprecated)
  public: std::string followService;

  /// \brief Follow offset service (deprecated)
  public: std::string followOffsetService;

  /// \brief The pose set from the move to pose service.
  public: std::optional<math::Pose3d> moveToPoseValue;

  /// \brief The motion duration set from the move to pose service.
  public: std::optional<double> moveToPoseDuration;

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
         << this->followService << "] (deprecated)" << std::endl;

  // track
  this->trackTopic = "/gui/track";
  this->node.Subscribe(this->trackTopic,
      &Implementation::OnTrackSub, this);
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
          << this->followOffsetService << "] (deprecated)" << std::endl;
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
  this->selectedFollowTarget = _msg.data();

  _res.set_data(true);

  this->trackMode = gz::msgs::CameraTrack::FOLLOW;

  this->newTrack = true;
  return true;
}

/////////////////////////////////////////////////
void CameraTracking::Implementation::OnTrackSub(const msgs::CameraTrack &_msg)
{
  std::lock_guard<std::mutex> lock(this->mutex);
  gzmsg << "Got new track message." << std::endl;

  if (_msg.track_mode() != gz::msgs::CameraTrack::USE_LAST)
  {
    this->trackMode = _msg.track_mode();
  }
  if (!_msg.follow_target().name().empty())
  {
    this->selectedFollowTarget = _msg.follow_target().name();
  }
  if (!_msg.track_target().name().empty())
  {
    this->selectedTrackTarget = _msg.track_target().name();
  }
  if (_msg.follow_target().name().empty() && _msg.track_target().name().empty()
        && _msg.track_mode() != gz::msgs::CameraTrack::USE_LAST)
  {
    gzmsg << "Track and Follow target names empty."<< std::endl;
  }
  if (_msg.has_follow_offset())
  {
    this->followOffset = msgs::Convert(_msg.follow_offset());
  }
  if (_msg.has_track_offset())
  {
    this->trackOffset = msgs::Convert(_msg.track_offset());
  }
  if (_msg.track_pgain() > 0.00001)
  {
    this->trackPGain = _msg.track_pgain();
  }
  if (_msg.follow_pgain() > 0.00001)
  {
    this->followPGain = _msg.follow_pgain();
  }

  this->newTrack = true;
  return;
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
  this->moveToPoseDuration.reset();
}

/////////////////////////////////////////////////
bool CameraTracking::Implementation::OnFollowOffset(const msgs::Vector3d &_msg,
  msgs::Boolean &_res)
{
  std::lock_guard<std::mutex> lock(this->mutex);
  if (!this->selectedFollowTarget.empty())
  {
    this->newTrack = true;
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

  if (_msg.duration() > 0)
  {
    this->moveToPoseDuration = _msg.duration();
  }
  else
  {
    this->moveToPoseDuration = 0.5;
  }

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
            *(this->moveToPoseDuration),
            std::bind(&Implementation::OnMoveToPoseComplete, this));
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
    GZ_PROFILE("CameraTracking::Implementation::OnRender Track");
    // reset track mode if target node got removed
    if (!this->selectedFollowTarget.empty())
    {
      rendering::NodePtr targetFollow = this->scene->NodeByName(
            this->selectedFollowTarget);
      if (!targetFollow && !this->selectedTargetWait)
      {
        this->camera->SetFollowTarget(nullptr);
        this->selectedFollowTarget.clear();
      }
    }
    if (!this->selectedTrackTarget.empty())
    {
      rendering::NodePtr targetTrack = this->scene->NodeByName(
            this->selectedTrackTarget);
      if (!targetTrack && !this->selectedTargetWait)
      {
        this->camera->SetTrackTarget(nullptr);
        this->selectedTrackTarget.clear();
      }
    }

    if (!this->moveToTarget.empty())
      return;
    rendering::NodePtr selectedFollowTargetTmp = this->camera->FollowTarget();
    rendering::NodePtr selectedTrackTargetTmp = this->camera->TrackTarget();
    if (!this->selectedTrackTarget.empty() ||
          !this->selectedFollowTarget.empty())
    {
      rendering::NodePtr targetFollow = this->scene->NodeByName(
            this->selectedFollowTarget);
      rendering::NodePtr targetTrack = this->scene->NodeByName(
            this->selectedTrackTarget);
      if (targetFollow || targetTrack)
      {
        if (this->trackMode == gz::msgs::CameraTrack::FOLLOW_FREE_LOOK ||
              this->trackMode == gz::msgs::CameraTrack::FOLLOW ||
              this->trackMode == gz::msgs::CameraTrack::FOLLOW_LOOK_AT )
        {
          if (!selectedFollowTargetTmp ||
                targetFollow != selectedFollowTargetTmp ||
                this->newTrack)
          {
            this->trackWorldFrame = false;
            this->camera->SetFollowTarget(targetFollow,
                this->followOffset,
                this->trackWorldFrame);
            if (this->trackMode == gz::msgs::CameraTrack::FOLLOW)
            {
              this->camera->SetTrackTarget(targetFollow);
              this->camera->SetTrackPGain(this->followPGain);
              this->camera->SetFollowPGain(this->trackPGain);
            }
            if (this->trackMode == gz::msgs::CameraTrack::FOLLOW_LOOK_AT)
            {
              this->camera->SetTrackTarget(targetTrack);
              this->camera->SetTrackPGain(this->followPGain);
              this->camera->SetFollowPGain(this->trackPGain);
            }
            if (this->trackMode == gz::msgs::CameraTrack::FOLLOW_FREE_LOOK)
            {
              this->camera->SetTrackTarget(nullptr);
              this->camera->SetFollowPGain(this->freeLookPGain);
            }
            this->newTrack = false;
            this->selectedTargetWait = false;
          }
        }
        if (this->trackMode == gz::msgs::CameraTrack::TRACK)
        {
          if (!selectedTrackTargetTmp ||
                targetTrack != selectedTrackTargetTmp ||
                this->newTrack)
          {
            this->trackWorldFrame = true;
            this->camera->SetFollowTarget(nullptr);
            this->camera->SetTrackTarget(targetTrack,
                this->trackOffset,
                this->trackWorldFrame);
            this->camera->SetTrackPGain(this->trackPGain);
            this->newTrack = false;
            this->selectedTargetWait = false;
          }
        }
      }
      else if (!this->selectedTargetWait)
      {
        gzerr << "Unable to track target. Target: '"
               << this->selectedTrackTarget << "' not found" << std::endl;
        gzerr << "Unable to follow target. Target: '"
               << this->selectedFollowTarget << "' not found" << std::endl;
        this->selectedFollowTarget.clear();
        this->selectedTrackTarget.clear();
      }
    }
    else
    {
      if (selectedFollowTargetTmp)
      {
        this->camera->SetFollowTarget(nullptr);
      }
      if (selectedTrackTargetTmp)
      {
        this->camera->SetTrackTarget(nullptr);
      }
    }
  }
}

/////////////////////////////////////////////////
CameraTracking::CameraTracking()
  : dataPtr(gz::utils::MakeUniqueImpl<Implementation>())
{
  this->dataPtr->timer = new QTimer(this);
  connect(this->dataPtr->timer, &QTimer::timeout, this->dataPtr->timer, [=]()
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
      if (this->dataPtr->trackMode == gz::msgs::CameraTrack::TRACK)
      {
        this->dataPtr->trackMsg.set_track_mode(gz::msgs::CameraTrack::TRACK);
        this->dataPtr->trackMsg.mutable_track_target()->set_name(
              this->dataPtr->selectedTrackTarget);
        this->dataPtr->trackMsg.mutable_track_offset()->set_x(
              this->dataPtr->trackOffset.X());
        this->dataPtr->trackMsg.mutable_track_offset()->set_y(
              this->dataPtr->trackOffset.Y());
        this->dataPtr->trackMsg.mutable_track_offset()->set_z(
              this->dataPtr->trackOffset.Z());
        this->dataPtr->trackMsg.set_track_pgain(
              this->dataPtr->trackPGain);
        this->dataPtr->trackMsg.clear_follow_target();
        this->dataPtr->trackMsg.clear_follow_offset();
        this->dataPtr->trackMsg.clear_follow_pgain();
      }
      else if (this->dataPtr->trackMode == gz::msgs::CameraTrack::FOLLOW)
      {
        this->dataPtr->trackMsg.set_track_mode(gz::msgs::CameraTrack::FOLLOW);
        this->dataPtr->trackMsg.mutable_follow_target()->set_name(
              this->dataPtr->selectedFollowTarget);
        this->dataPtr->trackMsg.mutable_follow_offset()->set_x(
              this->dataPtr->followOffset.X());
        this->dataPtr->trackMsg.mutable_follow_offset()->set_y(
              this->dataPtr->followOffset.Y());
        this->dataPtr->trackMsg.mutable_follow_offset()->set_z(
              this->dataPtr->followOffset.Z());
        this->dataPtr->trackMsg.set_follow_pgain(this->dataPtr->followPGain);
        this->dataPtr->trackMsg.clear_track_target();
        this->dataPtr->trackMsg.clear_track_offset();
        this->dataPtr->trackMsg.clear_track_pgain();
      }
      else if (this->dataPtr->trackMode ==
            gz::msgs::CameraTrack::FOLLOW_FREE_LOOK)
      {
        this->dataPtr->trackMsg.set_track_mode(
              gz::msgs::CameraTrack::FOLLOW_FREE_LOOK);
        this->dataPtr->trackMsg.mutable_follow_target()->set_name(
              this->dataPtr->selectedFollowTarget);
        this->dataPtr->trackMsg.mutable_follow_offset()->set_x(
              this->dataPtr->followOffset.X());
        this->dataPtr->trackMsg.mutable_follow_offset()->set_y(
              this->dataPtr->followOffset.Y());
        this->dataPtr->trackMsg.mutable_follow_offset()->set_z(
              this->dataPtr->followOffset.Z());
        this->dataPtr->trackMsg.set_follow_pgain(this->dataPtr->followPGain);
        this->dataPtr->trackMsg.clear_track_target();
        this->dataPtr->trackMsg.clear_track_offset();
        this->dataPtr->trackMsg.clear_track_pgain();
      }
      else if (this->dataPtr->trackMode ==
            gz::msgs::CameraTrack::FOLLOW_LOOK_AT)
      {
        this->dataPtr->trackMsg.set_track_mode(
              gz::msgs::CameraTrack::FOLLOW_LOOK_AT);
        this->dataPtr->trackMsg.mutable_follow_target()->set_name(
              this->dataPtr->selectedFollowTarget);
        this->dataPtr->trackMsg.mutable_track_target()->set_name(
              this->dataPtr->selectedTrackTarget);
        this->dataPtr->trackMsg.mutable_follow_offset()->set_x(
              this->dataPtr->followOffset.X());
        this->dataPtr->trackMsg.mutable_follow_offset()->set_y(
              this->dataPtr->followOffset.Y());
        this->dataPtr->trackMsg.mutable_follow_offset()->set_z(
              this->dataPtr->followOffset.Z());
        this->dataPtr->trackMsg.mutable_track_offset()->set_x(
              this->dataPtr->trackOffset.X());
        this->dataPtr->trackMsg.mutable_track_offset()->set_y(
              this->dataPtr->trackOffset.Y());
        this->dataPtr->trackMsg.mutable_track_offset()->set_z(
              this->dataPtr->trackOffset.Z());
        this->dataPtr->trackMsg.set_follow_pgain(this->dataPtr->followPGain);
        this->dataPtr->trackMsg.set_track_pgain(this->dataPtr->trackPGain);
      }
      else
      {
        this->dataPtr->trackMsg.set_track_mode(gz::msgs::CameraTrack::NONE);
        this->dataPtr->trackMsg.clear_track_target();
        this->dataPtr->trackMsg.clear_track_offset();
        this->dataPtr->trackMsg.clear_track_pgain();
        this->dataPtr->trackMsg.clear_follow_target();
        this->dataPtr->trackMsg.clear_follow_offset();
        this->dataPtr->trackMsg.clear_follow_pgain();
      }

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
    if (auto followTargetElem = _pluginElem->FirstChildElement("follow_target"))
    {
      this->dataPtr->selectedFollowTarget = followTargetElem->GetText();
      gzmsg << "CameraTracking: Loaded follow target from sdf ["
            << this->dataPtr->selectedFollowTarget << "]" << std::endl;
      this->dataPtr->selectedTargetWait = true;
    }
    if (auto followOffsetElem = _pluginElem->FirstChildElement("follow_offset"))
    {
      std::stringstream followOffsetStr;
      followOffsetStr << std::string(followOffsetElem->GetText());
      followOffsetStr >> this->dataPtr->followOffset;
      gzmsg << "CameraTracking: Loaded offset from sdf ["
            << this->dataPtr->followOffset << "]" << std::endl;
      this->dataPtr->newTrack = true;
    }
    if (auto followPGainElem = _pluginElem->FirstChildElement("follow_pgain"))
    {
      this->dataPtr->followPGain = std::stod(
            std::string(followPGainElem->GetText()));
      gzmsg << "CameraTracking: Loaded follow pgain from sdf ["
            << this->dataPtr->followPGain << "]" << std::endl;
      this->dataPtr->newTrack = true;
    }
  }

  if (auto app = gz::gui::App()) {
    if (auto mainWindow = app->findChild<gz::gui::MainWindow *>()) {
      mainWindow->installEventFilter(this);
    }
  }
}

/////////////////////////////////////////////////
void CameraTracking::Implementation::HandleKeyRelease(
  events::KeyReleaseOnScene *_e)
{
  if (_e->Key().Key() == Qt::Key_Escape)
  {
    this->trackMode = gz::msgs::CameraTrack::NONE;
    if (!this->selectedFollowTarget.empty() ||
          !this->selectedTrackTarget.empty())
    {
      this->selectedFollowTarget = std::string();
      this->selectedTrackTarget = std::string();

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
