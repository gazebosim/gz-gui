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

#ifndef IGNITION_GUI_PLUGINS_TOPICECHO_HH_
#define IGNITION_GUI_PLUGINS_TOPICECHO_HH_

#ifdef _MSC_VER
#pragma warning(push, 0)
#endif
#include <google/protobuf/message.h>
#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include <memory>

#include "ignition/gui/Plugin.hh"

namespace ignition
{
namespace gui
{
namespace plugins
{
  class TopicEchoPrivate;

  /// \brief Echo messages coming through an Ignition transport topic.
  ///
  /// ## Configuration
  /// This plugin doesn't accept any custom configuration.
  class TopicEcho : public Plugin
  {
    Q_OBJECT

    // Topic
    Q_PROPERTY(
      QString topic
      READ Topic
      WRITE SetTopic
      NOTIFY TopicChanged
    )

    // Buffer size
    Q_PROPERTY(
      int buffer
      READ Buffer
      WRITE SetBuffer
      NOTIFY BufferChanged
    )

    // Paused
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

    public: Q_INVOKABLE QString Topic() const;
    public: Q_INVOKABLE void SetTopic(const QString &_topic);
    signals: void TopicChanged();

    public: Q_INVOKABLE int Buffer() const;
    public: Q_INVOKABLE void SetBuffer(const int &_buffer);
    signals: void BufferChanged();

    public: Q_INVOKABLE bool Paused() const;
    public: Q_INVOKABLE void SetPaused(const bool &_paused);
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
}
}
}

#endif
