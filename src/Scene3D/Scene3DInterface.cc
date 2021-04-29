/*
 * Copyright (C) 2017 Open Source Robotics Foundation
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

#include "ignition/gui/Scene3DInterface.hh"
#include "RenderWindow.hh"

#include <ignition/common/Console.hh>

#include <ignition/transport/Node.hh>

#include "ignition/gui/MainWindow.hh"

namespace ignition
{
namespace gui
{
  /// \brief Private data class for Scene3D
  class Scene3DInterfacePrivate
  {
    /// \brief Pointer to item generated with plugin's QML
    public: QQuickItem *pluginItem{nullptr};

    public: RenderWindowItem *renderWindow;

    /// \brief Follow service
    public: std::string followService;

    /// \brief Move to service
    public: std::string moveToService;

    /// \brief Transport node
    public: transport::Node node;
  };
}
}

using namespace ignition;
using namespace gui;

/////////////////////////////////////////////////
Scene3DInterface::Scene3DInterface()
  : dataPtr(new Scene3DInterfacePrivate)
{
  qmlRegisterType<RenderWindowItem>("RenderWindow", 1, 0, "RenderWindow");
}

/////////////////////////////////////////////////
Scene3DInterface::~Scene3DInterface()
{
}

void Scene3DInterface::SetPluginItem(QQuickItem * pluginItem)
{
  this->dataPtr->pluginItem = pluginItem;

  this->dataPtr->renderWindow =
      this->dataPtr->pluginItem->findChild<RenderWindowItem *>();
  if (!this->dataPtr->renderWindow)
  {
    ignerr << "Unable to find Render Window item. "
           << "Render window will not be created" << std::endl;
    return;
  }
  this->dataPtr->renderWindow->forceActiveFocus();

  // move to
  this->dataPtr->moveToService = "/gui/move_to";
  this->dataPtr->node.Advertise(this->dataPtr->moveToService,
      &Scene3DInterface::OnMoveTo, this);
  ignmsg << "Move to service on ["
         << this->dataPtr->moveToService << "]" << std::endl;

  // follow
  this->dataPtr->followService = "/gui/follow";
  this->dataPtr->node.Advertise(this->dataPtr->followService,
      &Scene3DInterface::OnFollow, this);
  ignmsg << "Follow service on ["
         << this->dataPtr->followService << "]" << std::endl;
}

/////////////////////////////////////////////////
bool Scene3DInterface::OnMoveTo(const msgs::StringMsg &_msg,
  msgs::Boolean &_res)
{
  this->dataPtr->renderWindow->SetMoveTo(_msg.data());
  _res.set_data(true);
  return true;
}

/////////////////////////////////////////////////
bool Scene3DInterface::OnFollow(const msgs::StringMsg &_msg,
  msgs::Boolean &_res)
{
  ignerr << "On Follow" << std::endl;
  this->dataPtr->renderWindow->SetFollowTarget(_msg.data(), false);

  _res.set_data(true);
  return true;
}


void Scene3DInterface::SetFullScreen(bool _fullscreen)
{
  if(_fullscreen)
  {
    ignition::gui::App()->findChild
      <ignition::gui::MainWindow *>()->QuickWindow()->showFullScreen();
  }
}

/////////////////////////////////////////////////
void Scene3DInterface::SetVisibilityMask(uint32_t _mask)
{
  this->dataPtr->renderWindow->SetVisibilityMask(_mask);
}

/////////////////////////////////////////////////
void Scene3DInterface::SetFollowWorldFrame(bool _worldFrame)
{
  this->dataPtr->renderWindow->SetFollowWorldFrame(_worldFrame);
}

/////////////////////////////////////////////////
void Scene3DInterface::SetFollowOffset(const math::Vector3d &_offset)
{
  this->dataPtr->renderWindow->SetFollowOffset(_offset);
}

void Scene3DInterface::SetFollowTarget(const std::string &_target,
    bool _waitForTarget)
{
  this->dataPtr->renderWindow->SetFollowTarget(_target, _waitForTarget);
}

void Scene3DInterface::SetSkyEnabled(const bool _sky)
{
  this->dataPtr->renderWindow->SetSkyEnabled(_sky);
}

void Scene3DInterface::SetEngineName(const std::string _name)
{
  this->dataPtr->renderWindow->SetEngineName(_name);
}

void Scene3DInterface::SetSceneName(const std::string _name)
{
  this->dataPtr->renderWindow->SetSceneName(_name);
}

void Scene3DInterface::SetAmbientLight(const math::Color ambient)
{
  this->dataPtr->renderWindow->SetAmbientLight(ambient);
}

void Scene3DInterface::SetBackgroundColor(const math::Color bgColor)
{
  this->dataPtr->renderWindow->SetBackgroundColor(bgColor);
}

void Scene3DInterface::SetCameraPose(const math::Pose3d pose)
{
  this->dataPtr->renderWindow->SetCameraPose(pose);
}

void Scene3DInterface::SetSceneService(const std::string service)
{
  this->dataPtr->renderWindow->SetSceneService(service);
}

void Scene3DInterface::SetPoseTopic(const std::string topic)
{
  this->dataPtr->renderWindow->SetPoseTopic(topic);
}

void Scene3DInterface::SetDeletionTopic(const std::string topic)
{
  this->dataPtr->renderWindow->SetDeletionTopic(topic);
}

void Scene3DInterface::SetSceneTopic(const std::string topic)
{
  this->dataPtr->renderWindow->SetSceneTopic(topic);
}

void Scene3DInterface::SetFollowPGain(const double _gain)
{
  this->dataPtr->renderWindow->SetFollowPGain(_gain);
}
