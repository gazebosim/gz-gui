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

#include <iostream>
#ifdef _MSC_VER
#pragma warning(push, 0)
#endif
#include <ignition/msgs/twist.pb.h>
#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include "Teleop.hh"

#include <string>

#include <ignition/plugin/Register.hh>

#include <ignition/gui/Application.hh>
#include <ignition/gui/MainWindow.hh>

namespace ignition
{
namespace gui
{
namespace plugins
{
  enum class KeyLinear{
    kForward,
    kBackward,
    kStop,
  };

  enum class KeyAngular{
    kLeft,
    kRight,
    kStop,
  };

  class TeleopPrivate
  {
    /// \brief Node for communication.
    public: ignition::transport::Node node;

    /// \brief Topic. Set '/cmd_vel' as default.
    public: std::string topic = "/cmd_vel";

    /// \brief Publisher.
    public: ignition::transport::Node::Publisher cmdVelPub;

    /// \brief Linear velocity.
    public: double maxLinearVel = 1.0;

    /// \brief Angular velocity.
    public: double maxAngularVel = 0.5;

    /// \brief Linear direction.
    public: int linearKeyDir = 0;

    /// \brief Angular direction.
    public: int angularKeyDir = 0;

    /// \brief Linear state setted by keyboard input.
    public: KeyLinear linearKeyState = KeyLinear::kStop;

    /// \brief Angular state setted by keyboard input.
    public: KeyAngular angularKeyState = KeyAngular::kStop;

    /// \brief Indicates if the keyboard is enabled or
    /// disabled.
    public: bool keyEnable = false;
  };
}
}
}

using namespace ignition;
using namespace gui;
using namespace plugins;

/////////////////////////////////////////////////
Teleop::Teleop(): Plugin(), dataPtr(std::make_unique<TeleopPrivate>())
{
  // Initialize publisher using default topic.
  this->dataPtr->cmdVelPub = ignition::transport::Node::Publisher();
  this->dataPtr->cmdVelPub =
      this->dataPtr->node.Advertise<ignition::msgs::Twist>
      (this->dataPtr->topic);
}

/////////////////////////////////////////////////
Teleop::~Teleop() = default;

/////////////////////////////////////////////////
void Teleop::LoadConfig(const tinyxml2::XMLElement *_pluginElem)
{
  if (this->title.empty())
    this->title = "Teleop";

  if (_pluginElem)
  {
    auto topicElem = _pluginElem->FirstChildElement("topic");
    if (nullptr != topicElem && nullptr != topicElem->GetText())
      this->SetTopic(topicElem->GetText());
  }

  ignition::gui::App()->findChild
    <ignition::gui::MainWindow *>()->QuickWindow()->installEventFilter(this);
}

/////////////////////////////////////////////////
void Teleop::OnTeleopTwist(double _linVel, double _angVel)
{
  ignition::msgs::Twist cmdVelMsg;

  cmdVelMsg.mutable_linear()->set_x(_linVel);
  cmdVelMsg.mutable_angular()->set_z(_angVel);

  if (!this->dataPtr->cmdVelPub.Publish(cmdVelMsg))
  {
    ignerr << "ignition::msgs::Twist message couldn't be published at topic: "
      << this->dataPtr->topic << std::endl;
  }
}

/////////////////////////////////////////////////
QString Teleop::Topic() const
{
  return QString::fromStdString(this->dataPtr->topic);
}

/////////////////////////////////////////////////
void Teleop::SetTopic(const QString &_topic)
{
  this->dataPtr->topic = _topic.toStdString();
  ignmsg << "A new topic has been entered: '" <<
      this->dataPtr->topic << " ' " <<std::endl;

  // Update publisher with new topic.
  this->dataPtr->cmdVelPub = ignition::transport::Node::Publisher();
  this->dataPtr->cmdVelPub =
      this->dataPtr->node.Advertise<ignition::msgs::Twist>
      (this->dataPtr->topic);
  if (!this->dataPtr->cmdVelPub)
  {
    App()->findChild<MainWindow *>()->notifyWithDuration(
      QString::fromStdString("Error when advertising topic: " +
        this->dataPtr->topic), 4000);
    ignerr << "Error when advertising topic: " <<
      this->dataPtr->topic << std::endl;
  }
  else
  {
    App()->findChild<MainWindow *>()->notifyWithDuration(
      QString::fromStdString("Subscribing to topic: '<b>" +
        this->dataPtr->topic + "</b>'"), 4000);
  }
  this->TopicChanged();
}

/////////////////////////////////////////////////
void Teleop::SetMaxLinearVel(double _velocity)
{
  this->dataPtr->maxLinearVel = _velocity;
  this->MaxLinearVelChanged();
}

/////////////////////////////////////////////////
double Teleop::MaxLinearVel() const
{
  return this->dataPtr->maxLinearVel;
}

/////////////////////////////////////////////////
void Teleop::SetMaxAngularVel(double _velocity)
{
  this->dataPtr->maxAngularVel = _velocity;
  this->MaxAngularVelChanged();
}

/////////////////////////////////////////////////
double Teleop::MaxAngularVel() const
{
  return this->dataPtr->maxAngularVel;
}

/////////////////////////////////////////////////
void Teleop::OnKeySwitch(bool _checked)
{
  this->dataPtr->keyEnable = _checked;
}

/////////////////////////////////////////////////
void Teleop::OnSlidersSwitch(bool _checked)
{
  //if (_checked)
  //{
  //  this->OnTeleopTwist();
  //}
}

/////////////////////////////////////////////////
bool Teleop::eventFilter(QObject *_obj, QEvent *_event)
{
  if (this->dataPtr->keyEnable == true)
  {
    if (_event->type() == QEvent::KeyPress)
    {
      QKeyEvent *keyEvent = static_cast<QKeyEvent*>(_event);
      switch(keyEvent->key())
      {
        case Qt::Key_W:
          this->dataPtr->linearKeyState = KeyLinear::kForward;
          break;
        case Qt::Key_A:
          this->dataPtr->angularKeyState = KeyAngular::kLeft;
          break;
        case Qt::Key_D:
          this->dataPtr->angularKeyState = KeyAngular::kRight;
          break;
        case Qt::Key_S:
          this->dataPtr->linearKeyState = KeyLinear::kBackward;
          break;
        default:
          break;
      }
      this->SetKeyDirection();
      this->OnTeleopTwist(
          this->dataPtr->linearKeyDir * this->dataPtr->maxLinearVel,
          this->dataPtr->angularKeyDir * this->dataPtr->maxAngularVel);
    }

    if (_event->type() == QEvent::KeyRelease)
    {
      QKeyEvent *keyEvent = static_cast<QKeyEvent*>(_event);
      switch(keyEvent->key())
      {
        case Qt::Key_W:
          this->dataPtr->linearKeyState = KeyLinear::kStop;
          break;
        case Qt::Key_A:
          this->dataPtr->angularKeyState = KeyAngular::kStop;
          break;
        case Qt::Key_D:
          this->dataPtr->angularKeyState = KeyAngular::kStop;
          break;
        case Qt::Key_S:
          this->dataPtr->linearKeyState = KeyLinear::kStop;
          break;
        default:
          break;
      }
      this->SetKeyDirection();
      this->OnTeleopTwist(
          this->dataPtr->linearKeyDir * this->dataPtr->maxLinearVel,
          this->dataPtr->angularKeyDir * this->dataPtr->maxAngularVel);
    }
  }
  return QObject::eventFilter(_obj, _event);
}

/////////////////////////////////////////////////
void Teleop::SetKeyDirection()
{
  this->dataPtr->linearKeyDir = this->dataPtr->linearKeyState ==
      KeyLinear::kForward ? 1 : this->dataPtr->linearKeyState ==
      KeyLinear::kBackward ? -1 : 0;

  this->dataPtr->angularKeyDir = this->dataPtr->angularKeyState ==
      KeyAngular::kLeft ? 1 : this->dataPtr->angularKeyState ==
      KeyAngular::kRight ? -1 : 0;
}

// Register this plugin
IGNITION_ADD_PLUGIN(ignition::gui::plugins::Teleop,
                    ignition::gui::Plugin)
