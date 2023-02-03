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
#include <gz/msgs/stringmsg.pb.h>
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
  /// \brief Service request topic for follow name
  public: std::string followTargetNameService;

  /// \brief Service request topic for follow offset
  public: std::string followOffsetService;

  /// \brief Service request topic for follow p_gain
  public: std::string followPGainService;

  /// \brief Offset of camera from target being followed
  public: math::Vector3d followOffset{math::Vector3d(-5.0, 0.0, 3.0)};

  /// \brief Follow P gain
  public: double followPGain{0.01};

  /// \brief Follow target name from sdf
  public: std::string followTargetName;

  public: transport::Node node;

  /// \brief Process updated follow name from SDF
  public: void UpdateFollowTargetName();

  /// \brief Process updated follow offset
  public: void UpdateFollowOffset();

  /// \brief Process updated P Gain
  public: void UpdateFollowPGain();

  /// \brief flag for updating target name
  public: bool newFollowUpdateTargetName = false;

  /// \brief flag for updating P gain
  public: bool newFollowUpdatePGain = false;

  /// \brief flag for updating offset
  public: bool newFollowUpdateOffset = false;

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
void FollowConfig::LoadConfig(const tinyxml2::XMLElement *_pluginElem)
{
  if (this->title.empty())
    this->title = "Follow Config";

  // Follow target name service
  this->dataPtr->followTargetNameService = "/gui/follow";
  gzmsg << "FollowConfig: Follow target name service on ["
         << this->dataPtr->followTargetNameService << "]" << std::endl;

  // Follow target offset service
  this->dataPtr->followOffsetService = "/gui/follow/offset";
  gzmsg << "FollowConfig: Follow offset service on ["
         << this->dataPtr->followOffsetService << "]" << std::endl;

  // Follow target pgain service
  this->dataPtr->followPGainService = "/gui/follow/p_gain";
  gzmsg << "FollowConfig: Follow P gain service on ["
         << this->dataPtr->followPGainService << "]" << std::endl;


  // Read configuration
  if (_pluginElem)
  {
    if (auto nameElem = _pluginElem->FirstChildElement("follow_target"))
    {
      this->dataPtr->followTargetName = nameElem->GetText();
      gzmsg << "FollowConfig: Loaded follow_target from sdf ["
         << this->dataPtr->followTargetName << "]" << std::endl;
      this->dataPtr->newFollowUpdateTargetName = true;
    }
    if (auto offsetElem = _pluginElem->FirstChildElement("follow_offset"))
    {
      std::stringstream offsetStr;
      offsetStr << std::string(offsetElem->GetText());
      offsetStr >> this->dataPtr->followOffset;
      gzmsg << "FollowConfig: Loaded follow_offset from sdf ["
         << this->dataPtr->followOffset << "]" << std::endl;
      this->dataPtr->newFollowUpdateOffset = true;
    }
    if (auto pGainElem = _pluginElem->FirstChildElement("follow_pgain"))
    {
      this->dataPtr->followPGain = std::stod(std::string(pGainElem->GetText()));
      gzmsg << "FollowConfig: Loaded follow_pgain from sdf ["
         << this->dataPtr->followPGain << "]" << std::endl;
      this->dataPtr->newFollowUpdatePGain = true;
    }
  }

  gui::App()->findChild<
      MainWindow *>()->installEventFilter(this);
}

/////////////////////////////////////////////////
bool FollowConfig::eventFilter(QObject *_obj, QEvent *_event)
{
  if (_event->type() == events::Render::kType)
  {
    if (this->dataPtr->newFollowUpdateTargetName)
    {
      this->dataPtr->UpdateFollowTargetName();
    }
    if (this->dataPtr->newFollowUpdatePGain)
    {
      this->dataPtr->UpdateFollowPGain();
    }
    if (this->dataPtr->newFollowUpdateOffset)
    {
      this->dataPtr->UpdateFollowOffset();
    }
  }

  // Standard event processing
  return QObject::eventFilter(_obj, _event);
}

/////////////////////////////////////////////////
void FollowConfig::SetFollowOffset(double _x,
          double _y, double _z)
{
  if (!this->dataPtr->newFollowUpdateOffset)
  {
    this->dataPtr->followOffset = math::Vector3d(
      _x, _y, _z);
      gzmsg << "FollowConfig: SetFollowOffset("
        << this->dataPtr->followOffset << ")" << std::endl;
    this->dataPtr->newFollowUpdateOffset = true;
  }
}

/////////////////////////////////////////////////
void FollowConfig::SetFollowPGain(double _p)
{
  if (!this->dataPtr->newFollowUpdatePGain)
  {
    this->dataPtr->followPGain = _p;
    gzmsg << "FollowConfig: SetFollowPGain("
         << this->dataPtr->followPGain << ")" << std::endl;
    this->dataPtr->newFollowUpdatePGain = true;
  }
}

/////////////////////////////////////////////////
void FollowConfigPrivate::UpdateFollowTargetName()
{
  // Offset
  std::function<void(const msgs::Boolean &, const bool)> cbName =
    [&](const msgs::Boolean &/*_rep*/, const bool _resultName)
  {
    if (!_resultName)
    {
      gzerr << "FollowConfig: Error sending follow target name." << std::endl;
    } else {
      gzmsg << "FollowConfig: Request Target Name: "
            << this->followTargetName << " sent" << std::endl;
    }
  };

  msgs::StringMsg reqName;
  reqName.set_data(this->followTargetName);
  node.Request(this->followTargetNameService, reqName, cbName);
  this->newFollowUpdateTargetName = false;
}

/////////////////////////////////////////////////
void FollowConfigPrivate::UpdateFollowOffset()
{
  // Offset
  std::function<void(const msgs::Boolean &, const bool)> cbOffset =
    [&](const msgs::Boolean &/*_rep*/, const bool _resultOffset)
  {
    if (!_resultOffset)
    {
      gzerr << "FollowConfig: Error sending follow offset." << std::endl;
    } else {
      gzmsg << "FollowConfig: Request Offset: "
            << this->followOffset << " sent" << std::endl;
    }
  };

  msgs::Vector3d reqOffset;
  reqOffset.set_x(this->followOffset.X());
  reqOffset.set_y(this->followOffset.Y());
  reqOffset.set_z(this->followOffset.Z());
  node.Request(this->followOffsetService, reqOffset, cbOffset);
  this->newFollowUpdateOffset = false;
}

/////////////////////////////////////////////////
void FollowConfigPrivate::UpdateFollowPGain()
{
  // PGain
  std::function<void(const msgs::Boolean &, const bool)> cbPGain =
    [&](const msgs::Boolean &/*_rep*/, const bool _resultPGain)
  {
    if (!_resultPGain)
    {
      gzerr << "FollowConfig: Error sending follow pgain." << std::endl;
    } else {
      gzmsg << "FollowConfig: Request PGain: "
            << this->followPGain << " sent" << std::endl;
    }
  };

  msgs::Double reqPGain;
  reqPGain.set_data(this->followPGain);
  node.Request(this->followPGainService, reqPGain, cbPGain);
  this->newFollowUpdatePGain = false;
}

// Register this plugin
GZ_ADD_PLUGIN(FollowConfig,
              gui::Plugin)
