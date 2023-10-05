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

#ifndef GZ_GUI_PLUGINS_TOPICECHO_HH_
#define GZ_GUI_PLUGINS_TOPICECHO_HH_

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4251)
#endif
#include <google/protobuf/message.h>
#ifdef _MSC_VER
#pragma warning(pop)
#endif

#ifndef _WIN32
#  define TopicEcho_EXPORTS_API
#else
#  if (defined(TopicEcho_EXPORTS))
#    define TopicEcho_EXPORTS_API __declspec(dllexport)
#  else
#    define TopicEcho_EXPORTS_API __declspec(dllimport)
#  endif
#endif

#include <memory>

#include "gz/gui/Plugin.hh"

namespace gz::gui::plugins
{
class TopicEchoPrivate;

/// \brief Echo messages coming through a Gazebo Transport topic.
///
/// ## Configuration
/// This plugin doesn't accept any custom configuration.
class TopicEcho_EXPORTS_API TopicEcho : public Plugin
{
  Q_OBJECT

  /// \brief Topic
  Q_PROPERTY(
    QString topic
    READ Topic
    WRITE SetTopic
    NOTIFY TopicChanged
  )

  /// \brief Paused
  Q_PROPERTY(
    bool paused
    READ Paused
    WRITE SetPaused
    NOTIFY PausedChanged
  )

  /// \brief Constructor
  public: TopicEcho();

  /// \brief Destructor
  public: virtual ~TopicEcho();

  // Documentation inherited
  public: virtual void LoadConfig(const tinyxml2::XMLElement *_pluginElem);

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

  public slots: void OnBuffer(const unsigned int _steps);

  /// \brief Get whether it is paused
  /// \return True if paused
  public: Q_INVOKABLE bool Paused() const;

  /// \brief Set whether to be paused
  /// \param[in] _paused True if paused
  public: Q_INVOKABLE void SetPaused(const bool &_paused);

  /// \brief Notify that paused has changed
  signals: void PausedChanged();

  /// \brief Signal to add a message to the GUI list.
  /// \param[in] _msg Text message to add.
  signals: void AddMsg(QString _msg);

  /// \brief Receives incoming text messages.
  /// \param[in] _msg New text message.
  private: void OnMessage(const google::protobuf::Message &_msg);

  /// \brief Clear list and unsubscribe.
  private: void Stop();

  /// \brief Callback when echo button is pressed
  public slots: void OnEcho(const bool _checked);

  /// \brief Callback from the ::AddMsg signal.
  /// \param[in] _msg Message to add to the list.
  private slots: void OnAddMsg(QString _msg);

  /// \internal
  /// \brief Pointer to private data.
  private: std::unique_ptr<TopicEchoPrivate> dataPtr;
};
}  // namespace gz::gui::plugins
#endif  // GZ_GUI_PLUGINS_TOPICECHO_HH_
