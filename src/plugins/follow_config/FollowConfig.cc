/*
 * Copyright (C) 2023 Rudis Laboratories LLC
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

#include "FollowConfig.hh"

/// \brief Private data class for FollowConfig
class gz::gui::plugins::FollowConfigPrivate
{

  /// \brief Topic for follow message
  public: std::string followTopic;

  /// \brief Offset of camera from target being followed
  public: math::Vector3d followOffset{math::Vector3d(-5.0, 0.0, 3.0)};

  /// \brief Follow P gain
  public: double followPGain{0.01};

  public: transport::Node node;

  /// \brief Process updated follow
  public: void UpdateFollow();

  /// \brief flag for updating
  public: bool newFollowUpdate = false;

  /// \brief follow publisher
  public: transport::Node::Publisher followPub;
};

using namespace gz;
using namespace gui;
using namespace plugins;

/////////////////////////////////////////////////
FollowConfig::FollowConfig()
  : gz::gui::Plugin(), dataPtr(std::make_unique<FollowConfigPrivate>())
{
}

/////////////////////////////////////////////////
FollowConfig::~FollowConfig() = default;

/////////////////////////////////////////////////
void FollowConfig::LoadConfig(const tinyxml2::XMLElement *)
{
  if (this->title.empty())
    this->title = "Follow Config";

  // Follow target offset service
  this->dataPtr->followTopic = "/gui/track";
  this->dataPtr->followPub =
    this->dataPtr->node.Advertise<msgs::CameraTrack>(this->dataPtr->followTopic);
  gzmsg << "FollowConfig: Follow topic publisher advertised on ["
         << this->dataPtr->followTopic << "]" << std::endl;

  gui::App()->findChild<
      MainWindow *>()->installEventFilter(this);
}

/////////////////////////////////////////////////
bool FollowConfig::eventFilter(QObject *_obj, QEvent *_event)
{
  if (_event->type() == events::Render::kType)
  {
    if (this->dataPtr->newFollowUpdate)
    {
      this->dataPtr->UpdateFollow();
    }
  }

  // Standard event processing
  return QObject::eventFilter(_obj, _event);
}

/////////////////////////////////////////////////
void FollowConfig::SetFollow(double _x,
          double _y, double _z, double _p)
{
  if (!this->dataPtr->newFollowUpdate)
  {
    this->dataPtr->followOffset = math::Vector3d(
      _x, _y, _z);
    this->dataPtr->followPGain = _p;
    gzmsg << "FollowConfig: Offset("
          << this->dataPtr->followOffset << "), PGain("
          << this->dataPtr->followPGain << ")" << std::endl;
    this->dataPtr->newFollowUpdate = true;
  }
}


/////////////////////////////////////////////////
void FollowConfigPrivate::UpdateFollow()
{
  // Follow
  msgs::CameraTrack followMsg;
  followMsg.mutable_offset()->set_x(this->followOffset.X());
  followMsg.mutable_offset()->set_y(this->followOffset.Y());
  followMsg.mutable_offset()->set_z(this->followOffset.Z());
  followMsg.set_pgain(this->followPGain);
  this->followPub.Publish(followMsg);
  gzmsg << "FollowConfig: Publishing message." << std::endl;
  this->newFollowUpdate = false;
}

// Register this plugin
GZ_ADD_PLUGIN(FollowConfig,
              gui::Plugin)
