/*
 * Copyright (C) 2023 Open Source Robotics Foundation
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

#include <gz/msgs/stringmsg.pb.h>

#include <gz/common/Console.hh>
#include <gz/msgs/Utility.hh>
#include <gz/plugin/Register.hh>

#include "gz/gui/Application.hh"
#include "gz/gui/Conversions.hh"
#include "gz/gui/GuiEvents.hh"
#include "gz/gui/MainWindow.hh"

#include <gz/transport/Node.hh>

#include "CameraConfig.hh"

/// \brief Private data class for CameraConfig
class gz::gui::plugins::CameraConfigPrivate
{
  public: std::string followService;

  public: std::string followTarget;

  public: transport::Node node;

  public: void UpdateTarget();

  public: bool newFollowTarget = false;

  public: void OnNewFollowTarget(const gz::msgs::StringMsg &_msg);
};

using namespace gz;
using namespace gui;
using namespace plugins;

/////////////////////////////////////////////////
CameraConfig::CameraConfig()
  : gz::gui::Plugin(), dataPtr(std::make_unique<CameraConfigPrivate>())
{
}

/////////////////////////////////////////////////
CameraConfig::~CameraConfig() = default;

/////////////////////////////////////////////////
void CameraConfig::LoadConfig(const tinyxml2::XMLElement *_pluginElem)
{
  if (this->title.empty())
    this->title = "Camera config";

  // Follow service
  this->dataPtr->followService = "/gui/follow";

  // Read configuration
  if (_pluginElem)
  {
    if (auto topicElem = _pluginElem->FirstChildElement("follow_target"))
    {
      this->dataPtr->followTarget = topicElem->GetText();
      this->dataPtr->newFollowTarget = true;
      this->FollowTargetNameChanged();
    }
  }

  gui::App()->findChild<
      MainWindow *>()->installEventFilter(this);
}

/////////////////////////////////////////////////
bool CameraConfig::eventFilter(QObject *_obj, QEvent *_event)
{
  if (_event->type() == events::Render::kType)
  {
    if (this->dataPtr->newFollowTarget)
    {
      this->dataPtr->UpdateTarget();
    }
  }

  // Standard event processing
  return QObject::eventFilter(_obj, _event);
}

/////////////////////////////////////////////////
void CameraConfig::SetFollowTargetName(const QString &_targetName)
{
  this->dataPtr->followTarget = _targetName.toStdString();
  this->dataPtr->newFollowTarget = true;
}

/////////////////////////////////////////////////
QString CameraConfig::FollowTargetName() const
{
  return QString::fromStdString(this->dataPtr->followTarget);
}

/////////////////////////////////////////////////
void CameraConfigPrivate::UpdateTarget()
{
  std::function<void(const msgs::Boolean &, const bool)> cb =
    [&](const msgs::Boolean &/*_rep*/, const bool _result)
  {
    if (!_result)
      gzerr << "Error sending follow target" << std::endl;

    this->newFollowTarget = false;
  };

  msgs::StringMsg req;
  req.set_data(this->followTarget);
  node.Request(this->followService, req, cb);
}

// Register this plugin
GZ_ADD_PLUGIN(CameraConfig,
              gui::Plugin)
