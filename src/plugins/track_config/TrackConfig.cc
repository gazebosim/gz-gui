/*
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
#include <string>

#include <gz/msgs/double.pb.h>
#include <gz/msgs/cameratrack.pb.h>
#include <gz/msgs/vector3d.pb.h>

#include <gz/common/Console.hh>
#include <gz/msgs/Utility.hh>
#include <gz/plugin/Register.hh>

#include "gz/gui/Application.hh"
#include "gz/gui/Conversions.hh"
#include "gz/gui/GuiEvents.hh"
#include "gz/gui/MainWindow.hh"

#include <gz/transport/Node.hh>

#include "TrackConfig.hh"

/// \brief Private data class for TrackConfig
class gz::gui::plugins::TrackConfigPrivate
{

  /// \brief Topic for track message
  public: std::string trackTopic;

  /// \brief Pose of camera for tracking target
  public: math::Vector3d trackPose{math::Vector3d(0.0, 0.0, 0.0)};

  /// \brief track P gain
  public: double trackPGain{0.01};

  public: transport::Node node;

  /// \brief Process updated track
  public: void UpdateTrack();

  /// \brief flag for updating
  public: bool newTrackUpdate = false;

  /// \brief track publisher
  public: transport::Node::Publisher trackPub;
};

using namespace gz;
using namespace gui;
using namespace plugins;

/////////////////////////////////////////////////
TrackConfig::TrackConfig()
  : gz::gui::Plugin(), dataPtr(std::make_unique<TrackConfigPrivate>())
{
}

/////////////////////////////////////////////////
TrackConfig::~TrackConfig() = default;

/////////////////////////////////////////////////
void TrackConfig::LoadConfig(const tinyxml2::XMLElement *)
{
  if (this->title.empty())
    this->title = "Track Config";

  // Track target pose service
  this->dataPtr->trackTopic = "/gui/track";
  this->dataPtr->trackPub =
    this->dataPtr->node.Advertise<msgs::CameraTrack>(this->dataPtr->trackTopic);
  gzmsg << "TrackConfig: Track topic publisher advertised on ["
         << this->dataPtr->trackTopic << "]" << std::endl;

  gui::App()->findChild<
      MainWindow *>()->installEventFilter(this);
}

/////////////////////////////////////////////////
bool TrackConfig::eventFilter(QObject *_obj, QEvent *_event)
{
  if (_event->type() == events::Render::kType)
  {
    if (this->dataPtr->newTrackUpdate)
    {
      this->dataPtr->UpdateTrack();
    }
  }

  // Standard event processing
  return QObject::eventFilter(_obj, _event);
}

/////////////////////////////////////////////////
void TrackConfig::SetTrack(double _x,
          double _y, double _z, double _p)
{
  if (!this->dataPtr->newTrackUpdate)
  {
    this->dataPtr->trackPose = math::Vector3d(
      _x, _y, _z);
    this->dataPtr->trackPGain = _p;
    gzmsg << "TrackConfig: Track Pose("
          << this->dataPtr->trackPose << "), PGain("
          << this->dataPtr->trackPGain << ")" << std::endl;
    this->dataPtr->newTrackUpdate = true;
  }
}

/////////////////////////////////////////////////
void TrackConfigPrivate::UpdateTrack()
{
  // Track
  msgs::CameraTrack trackMsg;
  trackMsg.set_track_mode(msgs::CameraTrack::TRACK);
  trackMsg.mutable_track_pose()->set_x(this->trackPose.X());
  trackMsg.mutable_track_pose()->set_y(this->trackPose.Y());
  trackMsg.mutable_track_pose()->set_z(this->trackPose.Z());
  trackMsg.set_pgain(this->trackPGain);

  this->trackPub.Publish(trackMsg);
  gzmsg << "TrackConfig: Publishing message." << std::endl;
  this->newTrackUpdate = false;
}

// Register this plugin
GZ_ADD_PLUGIN(TrackConfig,
              gui::Plugin)
