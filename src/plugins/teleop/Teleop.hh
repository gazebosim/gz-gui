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

#include <ignition/gui/qt.h>

#include <memory>

#include <ignition/gui/Plugin.hh>
#include <ignition/transport/Node.hh>

namespace ignition
{
namespace gui
{
  class TeleopPrivate;

  /// \brief Publish teleop stokes to a user selected topic,
  /// or to '/cmd_vel' if no topic is selected.
  /// ## Configuration
  /// This plugin doesn't accept any custom configuration.
  class Teleop : public ignition::gui::Plugin
  {
    Q_OBJECT

    /// \brief Linear direction
    Q_PROPERTY(
      int linearDir
      READ LinearDirection
      WRITE setLinearDirection
      NOTIFY LinearDirectionChanged
    )

    /// \brief Angular direction
    Q_PROPERTY(
      int angularDir
      READ AngularDirection
      WRITE setAngularDirection
      NOTIFY AngularDirectionChanged
    )

    /// \brief Constructor
    public: Teleop();

    /// \brief Destructor
    public: virtual ~Teleop();

    // Documentation inherited
    public: virtual void LoadConfig(const tinyxml2::XMLElement *) override;

    protected: bool eventFilter(QObject *_obj, QEvent *_event) override;

    /// \brief Callback in Qt thread when the direction of the movement changes.
    public slots: void OnTeleopTwist();

    /// \brief Returns the linear direction variable value.
    ///  When the movement is forward it takes the value 1 and when
    /// is backward it takes the value -1.
    public: Q_INVOKABLE int LinearDirection() const;

    /// \brief Set the linear direction of the movement.
    public: Q_INVOKABLE void setLinearDirection(int _linearDir);

    /// \brief Notify that the linear direction changed.
    signals: void LinearDirectionChanged();

    /// \brief Returns the angular direction variable value. When the turn is
    /// left it takes the value 1 and when is right it takes the value -1.
    public: Q_INVOKABLE int AngularDirection() const;

    /// \brief Set the angular direction of the movement.
    public: Q_INVOKABLE void setAngularDirection(int _angularDir);

    /// \brief Notify that the angular direction changed.
    signals: void AngularDirectionChanged();

    /// \brief Callback in Qt thread when the topic changes.
    /// \param[in] _topic variable to indicate the topic to
    /// publish the Twist commands.
    public slots: void OnTopicSelection(const QString& _topic);

    /// \brief Callback in Qt thread when the linear velocity changes.
    /// \param[in] _velocity variable to indicate the linear velocity.
    public slots: void OnLinearVelSelection(const QString& _velocity);

    /// \brief Callback in Qt thread when the angular velocity changes.
    /// \param[in] _velocity variable to indicate the angular velocity.
    public slots: void OnAngularVelSelection(const QString& _velocity);

    /// \brief Callback in Qt thread when the keyboard is enabled or disabled.
    /// \param[in] _checked variable to indicate the state of the switch.
    public slots: void OnKeySwitch(bool _checked);

    /// \brief Callback in Qt thread when the sliders is enabled or disabled.
    /// \param[in] _checked variable to indicate the state of the switch.
    public slots: void OnSlidersSwitch(bool _checked);

    /// \brief Sets the movement direction when the keyboard is used.
    public: void setKeyDirection();

    /// \internal
    /// \brief Pointer to private data.
    private: std::unique_ptr<TeleopPrivate> dataPtr;

  };
}
}

#endif
