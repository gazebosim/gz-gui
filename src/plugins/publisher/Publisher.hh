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

#ifndef GZ_GUI_PLUGINS_PUBLISHER_HH_
#define GZ_GUI_PLUGINS_PUBLISHER_HH_

#include <memory>

#include "gz/gui/Plugin.hh"

#ifndef _WIN32
#  define Publisher_EXPORTS_API
#else
#  if (defined(Publisher_EXPORTS))
#    define Publisher_EXPORTS_API __declspec(dllexport)
#  else
#    define Publisher_EXPORTS_API __declspec(dllimport)
#  endif
#endif

namespace gz::gui::plugins
{
class PublisherPrivate;
/// \brief Widget which publishes a custom Gazebo Transport message.
///
/// ## Configuration
/// This plugin doesn't accept any custom configuration.
class Publisher_EXPORTS_API Publisher : public Plugin
{
  Q_OBJECT

  /// \brief Message type
  Q_PROPERTY(
    QString msgType
    READ MsgType
    WRITE SetMsgType
    NOTIFY MsgTypeChanged
  )

  /// \brief Message data
  Q_PROPERTY(
    QString msgData
    READ MsgData
    WRITE SetMsgData
    NOTIFY MsgDataChanged
  )

  /// \brief Topic
  Q_PROPERTY(
    QString topic
    READ Topic
    WRITE SetTopic
    NOTIFY TopicChanged
  )

  /// \brief Frequency
  Q_PROPERTY(
    double frequency
    READ Frequency
    WRITE SetFrequency
    NOTIFY FrequencyChanged
  )

  /// \brief Constructor
  public: Publisher();

  /// \brief Destructor
  public: virtual ~Publisher();

  // Documentation inherited
  public: virtual void LoadConfig(const tinyxml2::XMLElement *_pluginElem);

  /// \brief Callback when publish button is checked or unchecked.
  /// \param[in] _checked True if button is checked.
  public slots: void OnPublish(const bool _checked);

  /// \brief Get the message type as a string, for example
  /// 'gz.msgs.StringMsg'
  /// \return Message type
  public: Q_INVOKABLE QString MsgType() const;

  /// \brief Set the message type from a string, for example
  /// 'gz.msgs.StringMsg'
  /// \param[in] _msgType Message type
  public: Q_INVOKABLE void SetMsgType(const QString &_msgType);

  /// \brief Notify that message type has changed
  signals: void MsgTypeChanged();

  /// \brief Get the message data as a string, for example
  /// 'data: "Hello"'
  /// \return Message data
  public: Q_INVOKABLE QString MsgData() const;

  /// \brief Set the message data from a string, for example
  /// 'data: "Hello"'
  /// \param[in] _msgData Message data
  public: Q_INVOKABLE void SetMsgData(const QString &_msgData);

  /// \brief Notify that message data has changed
  signals: void MsgDataChanged();

  /// \brief Get the topic as a string, for example
  /// '/echo'
  /// \return Topic
  public: Q_INVOKABLE QString Topic() const;

  /// \brief Set the topic from a string, for example
  /// '/echo'
  /// \param[in] _topic Topic
  public: Q_INVOKABLE void SetTopic(const QString &_topic);

  /// \brief Notify that topic has changed
  signals: void TopicChanged();

  /// \brief Get the frequency, in Hz
  /// \return Frequency
  public: Q_INVOKABLE double Frequency() const;

  /// \brief Set the frequency, in Hz
  /// \param[in] _frequency Frequency
  public: Q_INVOKABLE void SetFrequency(const double _frequency);

  /// \brief Notify that frequency has changed
  signals: void FrequencyChanged();

  /// \internal
  /// \brief Pointer to private data.
  private: std::unique_ptr<PublisherPrivate> dataPtr;
};
}  // namespace gz::gui::plugins
#endif  // GZ_GUI_PLUGINS_PUBLISHER_HH_
