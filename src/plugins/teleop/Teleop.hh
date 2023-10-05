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

#ifndef GZ_GUI_PLUGINS_TELEOP_HH_
#define GZ_GUI_PLUGINS_TELEOP_HH_

#include <memory>

#include <gz/transport/Node.hh>

#include <gz/gui/Plugin.hh>
#include <gz/gui/qt.h>

#ifndef _WIN32
#  define Teleop_EXPORTS_API
#else
#  if (defined(Teleop_EXPORTS))
#    define Teleop_EXPORTS_API __declspec(dllexport)
#  else
#    define Teleop_EXPORTS_API __declspec(dllimport)
#  endif
#endif

namespace gz::gui::plugins
{
class TeleopPrivate;

/// \brief Publish teleop stokes to a user selected topic,
/// or to '/cmd_vel' if no topic is selected.
/// Buttons, the keyboard or sliders can be used to move a
/// vehicle in the world.
/// ## Configuration
/// * `<topic>`: Topic to publish twist messages to.
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

  /// \brief Forward velocity
  Q_PROPERTY(
    double maxForwardVel
    READ MaxForwardVel
    WRITE SetMaxForwardVel
    NOTIFY MaxForwardVelChanged
  )

  /// \brief Vertical velocity
  Q_PROPERTY(
    double maxVerticalVel
    READ MaxVerticalVel
    WRITE SetMaxVerticalVel
    NOTIFY MaxVerticalVelChanged
  )

  /// \brief Yaw velocity
  Q_PROPERTY(
    double maxYawVel
    READ MaxYawVel
    WRITE SetMaxYawVel
    NOTIFY MaxYawVelChanged
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
  /// \param[in] _forwardVel Forward velocity
  /// \param[in] _verticalVel Vertical velocity
  /// \param[in] _angVel Yaw velocity
  public slots: void OnTeleopTwist(double _forwardVel, double _verticalVel,
      double _angVel);

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

  /// \brief Get the forward velocity.
  /// \return Forward velocity.
  public: Q_INVOKABLE double MaxForwardVel() const;

  /// \brief Callback in Qt thread when the forward velocity changes.
  /// \param[in] _velocity variable to indicate the forward velocity.
  public slots: void SetMaxForwardVel(double _velocity);

  /// \brief Notify that forward velocity has changed
  signals: void MaxForwardVelChanged();

  /// \brief Get the vertical velocity.
  /// \return Vertical velocity.
  public: Q_INVOKABLE double MaxVerticalVel() const;

  /// \brief Callback in Qt thread when the vertical velocity changes.
  /// \param[in] _velocity variable to indicate the vertical velocity.
  public slots: void SetMaxVerticalVel(double _velocity);

  /// \brief Notify that vertical velocity has changed
  signals: void MaxVerticalVelChanged();

  /// \brief Get the yaw velocity.
  /// \return Yaw velocity.
  public: Q_INVOKABLE double MaxYawVel() const;

  /// \brief Callback in Qt thread when the yaw velocity changes.
  /// \param[in] _velocity variable to indicate the yaw velocity.
  public slots: void SetMaxYawVel(double _velocity);

  /// \brief Notify that yaw velocity has changed
  signals: void MaxYawVelChanged();

  /// \brief Callback in Qt thread when the keyboard is enabled or disabled.
  /// \param[in] _checked variable to indicate the state of the switch.
  public slots: void OnKeySwitch(bool _checked);

  /// \brief Sets the movement scale when the keyboard is used.
  public: void SetKeyScale();

  /// \internal
  /// \brief Pointer to private data.
  private: std::unique_ptr<TeleopPrivate> dataPtr;

};
}  // namespace gz::gui::plugins
#endif  // GZ_GUI_PLUGINS_TELEOP_HH_
