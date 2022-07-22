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

#ifndef IGNITION_GUI_PLUGINS_TELEOP_HH_
#define IGNITION_GUI_PLUGINS_TELEOP_HH_

#include <memory>

#include <ignition/transport/Node.hh>

#include <ignition/gui/Plugin.hh>
#include <ignition/gui/qt.h>

#ifndef _WIN32
#  define Teleop_EXPORTS_API
#else
#  if (defined(Teleop_EXPORTS))
#    define Teleop_EXPORTS_API __declspec(dllexport)
#  else
#    define Teleop_EXPORTS_API __declspec(dllimport)
#  endif
#endif

namespace ignition
{
namespace gui
{
namespace plugins
{
  class TeleopPrivate;

  /// \brief Publish teleop stokes to a user selected topic,
  /// or to '/cmd_vel' if no topic is selected.
  /// Buttons, the keyboard or sliders can be used to move a
  /// vehicle load to the world.
  /// ## Configuration
  /// This plugin doesn't accept any custom configuration.
  class Teleop_EXPORTS_API Teleop : public Plugin
  {
    Q_OBJECT

    /// \brief Topic
    Q_PROPERTY(
      QString topic
      READ Topic
      WRITE SetTopic
      NOTIFY TopicChanged
    )

    /// \brief Linear velocity
    Q_PROPERTY(
      double maxLinearVel
      READ MaxLinearVel
      WRITE SetMaxLinearVel
      NOTIFY MaxLinearVelChanged
    )

    /// \brief Angular velocity
    Q_PROPERTY(
      double maxAngularVel
      READ MaxAngularVel
      WRITE SetMaxAngularVel
      NOTIFY MaxAngularVelChanged
    )

    /// \brief Constructor
    public: Teleop();

    /// \brief Destructor
    public: virtual ~Teleop();

    // Documentation inherited.
    public: virtual void LoadConfig(const tinyxml2::XMLElement *) override;

    /// \brief Filters events of type 'keypress' and 'keyrelease'.
    protected: bool eventFilter(QObject *_obj, QEvent *_event) override;

    /// \brief Publish the twist message to the selected command velocity topic.
    public slots: void OnTeleopTwist(double _linVel, double _angVel);

    /// \brief Get the topic as a string, for example
    /// '/echo'
    /// \return Topic
    public: Q_INVOKABLE QString Topic() const;

    /// \brief Callback in Qt thread when the topic changes.
    /// \param[in] _topic variable to indicate the topic to
    /// publish the Twist commands.
    public slots: void SetTopic(const QString &_topic);

    /// \brief Notify that topic has changed
    signals: void TopicChanged();

    /// \brief Get the linear velocity.
    /// \return Linear velocity.
    public: Q_INVOKABLE double MaxLinearVel() const;

    /// \brief Callback in Qt thread when the linear velocity changes.
    /// \param[in] _velocity variable to indicate the linear velocity.
    public slots: void SetMaxLinearVel(double _velocity);

    /// \brief Notify that linear velocity has changed
    signals: void MaxLinearVelChanged();

    /// \brief Get the angular velocity.
    /// \return Angular velocity.
    public: Q_INVOKABLE double MaxAngularVel() const;

    /// \brief Callback in Qt thread when the angular velocity changes.
    /// \param[in] _velocity variable to indicate the angular velocity.
    public slots: void SetMaxAngularVel(double _velocity);

    /// \brief Notify that angular velocity has changed
    signals: void MaxAngularVelChanged();

    /// \brief Callback in Qt thread when the keyboard is enabled or disabled.
    /// \param[in] _checked variable to indicate the state of the switch.
    public slots: void OnKeySwitch(bool _checked);

    /// \brief Sets the movement direction when the keyboard is used.
    public: void SetKeyDirection();

    /// \internal
    /// \brief Pointer to private data.
    private: std::unique_ptr<TeleopPrivate> dataPtr;

  };
}
}
}

#endif
