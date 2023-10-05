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
#include <string>

#include <gz/msgs/twist.pb.h>

#include "Teleop.hh"

#include <gz/plugin/Register.hh>

#include <gz/gui/Application.hh>
#include <gz/gui/MainWindow.hh>

namespace
{
enum class KeyForward{
  kForward,
  kBackward,
  kStop,
};

enum class KeyVertical{
  kUp,
  kDown,
  kStop,
};

enum class KeyYaw{
  kLeft,
  kRight,
  kStop,
};
}  // namespace

namespace gz::gui::plugins
{
class TeleopPrivate
{
  /// \brief Node for communication.
  public: gz::transport::Node node;

  /// \brief Topic. Set '/cmd_vel' as default.
  public: std::string topic = "/cmd_vel";

  /// \brief Publisher.
  public: gz::transport::Node::Publisher cmdVelPub;

  /// \brief Maximum forward velocity in m/s. GUI buttons and key presses
  /// will use this velocity. Sliders will scale up to this value.
  public: double maxForwardVel = 1.0;

  /// \brief Maximum vertical velocity in m/s. GUI buttons and key presses
  /// will use this velocity. Sliders will scale up to this value.
  public: double maxVerticalVel = 1.0;

  /// \brief Maximum yaw velocity in rad/s. GUI buttons and key presses
  /// will use this velocity. Sliders will scale up to this value.
  public: double maxYawVel = 0.5;

  /// \brief Forward scale to multiply by maxForwardVel, in the [-1, 1] range.
  /// Negative values go backwards, zero stops movement in the forward axis.
  public: int forwardKeyScale = 0;

  /// \brief Vertical scale to multiply by maxVerticalVel, in the [-1, 1]
  /// range. Negative values go down, zero stops movement in the vertical
  /// axis.
  public: int verticalKeyScale = 0;

  /// \brief Yaw scale to multiply by maxYawVel, in the [-1, 1] range.
  /// Negative values rotate clockwise when looking from above, zero stops
  /// movement in the yaw axis.
  public: int yawKeyScale = 0;

  /// \brief Forward state set by keyboard input.
  public: KeyForward forwardKeyState = KeyForward::kStop;

  /// \brief Vertical state set by keyboard input.
  public: KeyVertical verticalKeyState = KeyVertical::kStop;

  /// \brief Yaw state set by keyboard input.
  public: KeyYaw yawKeyState = KeyYaw::kStop;

  /// \brief Indicates if the keyboard is enabled or
  /// disabled.
  public: bool keyEnable = false;
};

/////////////////////////////////////////////////
Teleop::Teleop(): Plugin(), dataPtr(std::make_unique<TeleopPrivate>())
{
  // Initialize publisher using default topic.
  this->dataPtr->cmdVelPub = transport::Node::Publisher();
  this->dataPtr->cmdVelPub =
      this->dataPtr->node.Advertise<msgs::Twist>
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

  App()->findChild<MainWindow *>()->QuickWindow()->installEventFilter(this);
}

/////////////////////////////////////////////////
void Teleop::OnTeleopTwist(double _forwardVel, double _verticalVel,
        double _angVel)
{
  msgs::Twist cmdVelMsg;

  cmdVelMsg.mutable_linear()->set_x(_forwardVel);
  cmdVelMsg.mutable_linear()->set_z(_verticalVel);
  cmdVelMsg.mutable_angular()->set_z(_angVel);

  if (!this->dataPtr->cmdVelPub.Publish(cmdVelMsg))
  {
    gzerr << "gz::msgs::Twist message couldn't be published at topic: "
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
  gzmsg << "A new topic has been entered: '" <<
      this->dataPtr->topic << " ' " <<std::endl;

  // Update publisher with new topic.
  this->dataPtr->cmdVelPub = transport::Node::Publisher();
  this->dataPtr->cmdVelPub =
      this->dataPtr->node.Advertise<msgs::Twist>
      (this->dataPtr->topic);
  if (!this->dataPtr->cmdVelPub)
  {
    App()->findChild<MainWindow *>()->notifyWithDuration(
      QString::fromStdString("Error when advertising topic: " +
        this->dataPtr->topic), 4000);
    gzerr << "Error when advertising topic: " <<
      this->dataPtr->topic << std::endl;
  }
  else
  {
    App()->findChild<MainWindow *>()->notifyWithDuration(
      QString::fromStdString("Advertising topic: '<b>" +
        this->dataPtr->topic + "</b>'"), 4000);
  }
  this->TopicChanged();
}

/////////////////////////////////////////////////
void Teleop::SetMaxForwardVel(double _velocity)
{
  this->dataPtr->maxForwardVel = _velocity;
  this->MaxForwardVelChanged();
}

/////////////////////////////////////////////////
double Teleop::MaxForwardVel() const
{
  return this->dataPtr->maxForwardVel;
}

/////////////////////////////////////////////////
void Teleop::SetMaxVerticalVel(double _velocity)
{
  this->dataPtr->maxVerticalVel = _velocity;
  this->MaxVerticalVelChanged();
}

/////////////////////////////////////////////////
double Teleop::MaxVerticalVel() const
{
  return this->dataPtr->maxVerticalVel;
}

/////////////////////////////////////////////////
void Teleop::SetMaxYawVel(double _velocity)
{
  this->dataPtr->maxYawVel = _velocity;
  this->MaxYawVelChanged();
}

/////////////////////////////////////////////////
double Teleop::MaxYawVel() const
{
  return this->dataPtr->maxYawVel;
}

/////////////////////////////////////////////////
void Teleop::OnKeySwitch(bool _checked)
{
  this->dataPtr->keyEnable = _checked;
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
          this->dataPtr->forwardKeyState = KeyForward::kForward;
          break;
        case Qt::Key_A:
          this->dataPtr->yawKeyState = KeyYaw::kLeft;
          break;
        case Qt::Key_D:
          this->dataPtr->yawKeyState = KeyYaw::kRight;
          break;
        case Qt::Key_S:
          this->dataPtr->forwardKeyState = KeyForward::kBackward;
          break;
        case Qt::Key_Q:
          this->dataPtr->verticalKeyState = KeyVertical::kUp;
          break;
        case Qt::Key_E:
          this->dataPtr->verticalKeyState = KeyVertical::kDown;
          break;
        default:
          break;
      }
      this->SetKeyScale();
      this->OnTeleopTwist(
          this->dataPtr->forwardKeyScale * this->dataPtr->maxForwardVel,
          this->dataPtr->verticalKeyScale * this->dataPtr->maxVerticalVel,
          this->dataPtr->yawKeyScale * this->dataPtr->maxYawVel);
    }

    if (_event->type() == QEvent::KeyRelease)
    {
      QKeyEvent *keyEvent = static_cast<QKeyEvent*>(_event);
      switch(keyEvent->key())
      {
        case Qt::Key_W:
          this->dataPtr->forwardKeyState = KeyForward::kStop;
          break;
        case Qt::Key_A:
          this->dataPtr->yawKeyState = KeyYaw::kStop;
          break;
        case Qt::Key_D:
          this->dataPtr->yawKeyState = KeyYaw::kStop;
          break;
        case Qt::Key_S:
          this->dataPtr->forwardKeyState = KeyForward::kStop;
          break;
        case Qt::Key_Q:
          this->dataPtr->verticalKeyState = KeyVertical::kStop;
          break;
        case Qt::Key_E:
          this->dataPtr->verticalKeyState = KeyVertical::kStop;
          break;
        default:
          break;
      }
      this->SetKeyScale();
      this->OnTeleopTwist(
          this->dataPtr->forwardKeyScale * this->dataPtr->maxForwardVel,
          this->dataPtr->verticalKeyScale * this->dataPtr->maxVerticalVel,
          this->dataPtr->yawKeyScale * this->dataPtr->maxYawVel);
    }
  }
  return QObject::eventFilter(_obj, _event);
}

/////////////////////////////////////////////////
void Teleop::SetKeyScale()
{
  this->dataPtr->forwardKeyScale = this->dataPtr->forwardKeyState ==
      KeyForward::kForward ? 1 : this->dataPtr->forwardKeyState ==
      KeyForward::kBackward ? -1 : 0;

  this->dataPtr->verticalKeyScale = this->dataPtr->verticalKeyState ==
      KeyVertical::kUp ? 1 : this->dataPtr->verticalKeyState ==
      KeyVertical::kDown ? -1 : 0;

  this->dataPtr->yawKeyScale = this->dataPtr->yawKeyState ==
      KeyYaw::kLeft ? 1 : this->dataPtr->yawKeyState ==
      KeyYaw::kRight ? -1 : 0;
}
}  // namespace gz::gui::plugins

// Register this plugin
GZ_ADD_PLUGIN(gz::gui::plugins::Teleop,
              gui::Plugin)
