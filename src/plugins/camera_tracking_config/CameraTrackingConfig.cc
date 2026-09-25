/*
 * Copyright (C) 2024 CogniPilot Foundation
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
#include <memory>
#include <string>

#include <gz/msgs/cameratrack.pb.h>
#include <gz/msgs/double.pb.h>
#include <gz/msgs/entity.pb.h>
#include <gz/msgs/vector3d.pb.h>

#include <gz/common/Console.hh>
#include <gz/math/Vector3.hh>
#include <gz/msgs/Utility.hh>
#include <gz/plugin/Register.hh>

#include "gz/gui/Application.hh"
#include "gz/gui/Conversions.hh"
#include "gz/gui/GuiEvents.hh"
#include "gz/gui/MainWindow.hh"

#include <gz/transport/Node.hh>

#include "CameraTrackingConfig.hh"

/// \brief Private data class for CameraTrackingConfig
class gz::gui::plugins::CameraTrackingConfigPrivate
{

  /// \brief Topic for track message
  public: std::string cameraTrackingTopic;

  /// \brief Tracking Mode
  public: int trackMode = gz::msgs::CameraTrack::NONE;

  /// \brief Follow Target
  public: std::string selectedFollowTarget;

  /// \brief Track Target
  public: std::string selectedTrackTarget;

  /// \brief Offset of camera from target being followed
  public: math::Vector3d followOffset{math::Vector3d(-3.0, 0.0, -2.0)};

  /// \brief tracking offset
  public: math::Vector3d trackOffset{math::Vector3d(0.0, 0.0, 0.0)};

  /// \brief Follow P gain
  public: double followPGain{0.01};

  /// \brief track P gain
  public: double trackPGain{0.01};

  public: transport::Node node;

  /// \brief Process updated track
  public: void UpdateTracking();

  /// \brief flag for updating
  public: bool newTrackingUpdate = false;

  /// \brief track publisher
  public: transport::Node::Publisher trackingPub;
};

using namespace gz;
using namespace gui;
using namespace plugins;

/////////////////////////////////////////////////
CameraTrackingConfig::CameraTrackingConfig()
  : gz::gui::Plugin(), dataPtr(std::make_unique<CameraTrackingConfigPrivate>())
{
}

/////////////////////////////////////////////////
CameraTrackingConfig::~CameraTrackingConfig() = default;

/////////////////////////////////////////////////
void CameraTrackingConfig::LoadConfig(const tinyxml2::XMLElement *_pluginElem)
{
  if (this->title.empty())
    this->title = "Camera Tracking Config";

  if (_pluginElem)
  {
    if (auto elem = _pluginElem->FirstChildElement("camera_follow"))
    {
      if (auto trackModeElem = _pluginElem->FirstChildElement("track_mode"))
      {
        std::string trackModeStr = common::lowercase(trackModeElem->GetText());
        if (trackModeStr == "none")
        {
          this->dataPtr->trackMode = gz::msgs::CameraTrack::NONE;
        }
        else if (trackModeStr == "track")
        {
          this->dataPtr->trackMode = gz::msgs::CameraTrack::TRACK;
        }
        else if (trackModeStr == "follow")
        {
          this->dataPtr->trackMode = gz::msgs::CameraTrack::FOLLOW;
        }
        else if (trackModeStr == "follow_free_look")
        {
          this->dataPtr->trackMode = gz::msgs::CameraTrack::FOLLOW_FREE_LOOK;
        }
        else if (trackModeStr == "follow_look_at")
        {
          this->dataPtr->trackMode = gz::msgs::CameraTrack::FOLLOW_LOOK_AT;
        }
        else if (trackModeStr == "use_last")
        {
          this->dataPtr->trackMode = gz::msgs::CameraTrack::USE_LAST;
        }
        else
        {
          gzerr << "CameraTrackingConfig: Unknown track mode [" << trackModeStr << "]" << std::endl;
        }
      }

      if (auto followTargetElem = _pluginElem->FirstChildElement("follow_target"))
      {
        this->dataPtr->selectedFollowTarget = followTargetElem->GetText();
      } 
      else 
      {
        gzerr << "CameraTrackingConfig: No follow target specified in config." << std::endl;
      }

      if (auto trackTargetElem = _pluginElem->FirstChildElement("track_target"))
      {
        this->dataPtr->selectedTrackTarget = trackTargetElem->GetText();
      } 
      else 
      {
        gzerr << "CameraTrackingConfig: No track target specified in config." << std::endl;
      }

      if (auto followOffsetElem = _pluginElem->FirstChildElement("follow_offset"))
      {
        std::stringstream followOffsetStr;
        followOffsetStr << std::string(followOffsetElem->GetText());
        followOffsetStr >> this->dataPtr->followOffset;
      } 
      else 
      {
        gzerr << "CameraTrackingConfig: No follow offset specified in config." << std::endl;
      }

      if (auto trackOffsetElem = _pluginElem->FirstChildElement("track_offset"))
      {
        std::stringstream trackOffsetStr;
        trackOffsetStr << std::string(trackOffsetElem->GetText());
        trackOffsetStr >> this->dataPtr->trackOffset;
      } 
      else 
      {
        gzerr << "CameraTrackingConfig: No track offset specified in config." << std::endl;
      }

      if (auto followPGainElem = _pluginElem->FirstChildElement("follow_pgain"))
      {
        double followGain = std::stod(
              std::string(followPGainElem->GetText()));
        if (followGain >= 0 && followGain <= 1.0)
        {
          this->dataPtr->followPGain = followGain;
        }
        else
        {
          gzerr << "Camera follow p gain outside of range [0, 1]" << std::endl;
        }
      } 
      else 
      {
        gzerr << "CameraTrackingConfig: No follow P gain specified in config." << std::endl;
      }

      if (auto trackPGainElem = _pluginElem->FirstChildElement("track_pgain"))
      {
        double trackGain = std::stod(
              std::string(trackPGainElem->GetText()));
        if (trackGain >= 0 && trackGain <= 1.0)
        {
          this->dataPtr->trackPGain = trackGain;
        }
        else
        {
          gzerr << "Camera track p gain outside of range [0, 1]" << std::endl;
        }
      } 
      else 
      {
        gzerr << "CameraTrackingConfig: No track P gain specified in config." << std::endl;
      }
    }
  }

  // Track target pose service
  this->dataPtr->cameraTrackingTopic = "/gui/track";
  this->dataPtr->trackingPub =
    this->dataPtr->node.Advertise<msgs::CameraTrack>(
          this->dataPtr->cameraTrackingTopic);
  gzmsg << "CameraTrackingConfig: Tracking topic publisher advertised on ["
         << this->dataPtr->cameraTrackingTopic << "]" << std::endl;

  if (auto app = gz::gui::App()) {
    if (auto mainWindow = app->findChild<gz::gui::MainWindow *>()) {
      mainWindow->installEventFilter(this);
    }
  }
}

/////////////////////////////////////////////////
bool CameraTrackingConfig::eventFilter(QObject *_obj, QEvent *_event)
{
  if (_event->type() == events::Render::kType)
  {
    if (this->dataPtr->newTrackingUpdate)
    {
      this->dataPtr->UpdateTracking();
    }
  }

  // Standard event processing
  return QObject::eventFilter(_obj, _event);
}

/////////////////////////////////////////////////
void CameraTrackingConfig::SetTracking(
          double _tx, double _ty, double _tz, double _tp,
          double _fx, double _fy, double _fz, double _fp)
{
  if (!this->dataPtr->newTrackingUpdate)
  {
    this->dataPtr->trackOffset = math::Vector3d(
      _tx, _ty, _tz);
    this->dataPtr->followOffset = math::Vector3d(
      _fx, _fy, _fz);
    this->dataPtr->trackPGain = _tp;
    this->dataPtr->followPGain = _fp;
    gzmsg << "CameraTrackingConfig: Track: Offset("
          << this->dataPtr->trackOffset << "), PGain("
          << this->dataPtr->trackPGain << ")" << std::endl;
    gzmsg << "CameraTrackingConfig: Follow: Offset("
          << this->dataPtr->followOffset << "), PGain("
          << this->dataPtr->followPGain << ")" << std::endl;
    this->dataPtr->newTrackingUpdate = true;
  }
}

/////////////////////////////////////////////////
void CameraTrackingConfigPrivate::UpdateTracking()
{
  // Track
  msgs::CameraTrack trackingMsg;
  trackingMsg.set_track_mode(msgs::CameraTrack::USE_LAST);
  trackingMsg.mutable_track_offset()->set_x(this->trackOffset.X());
  trackingMsg.mutable_track_offset()->set_y(this->trackOffset.Y());
  trackingMsg.mutable_track_offset()->set_z(this->trackOffset.Z());
  trackingMsg.mutable_follow_offset()->set_x(this->followOffset.X());
  trackingMsg.mutable_follow_offset()->set_y(this->followOffset.Y());
  trackingMsg.mutable_follow_offset()->set_z(this->followOffset.Z());
  trackingMsg.set_follow_pgain(this->followPGain);
  trackingMsg.set_track_pgain(this->trackPGain);

  this->trackingPub.Publish(trackingMsg);
  gzmsg << "CameraTrackingConfig: Publishing message." << std::endl;
  this->newTrackingUpdate = false;
}

// Register this plugin
GZ_ADD_PLUGIN(CameraTrackingConfig,
              gui::Plugin)
