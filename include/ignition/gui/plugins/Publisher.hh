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

#ifndef IGNITION_GUI_PLUGINS_PUBLISHER_HH_
#define IGNITION_GUI_PLUGINS_PUBLISHER_HH_

#include <memory>

#include "ignition/gui/qt.h"
#include "ignition/gui/Plugin.hh"

namespace ignition
{
namespace gui
{
namespace plugins
{
  class PublisherPrivate;

  /// \brief Widget which publishes a custom Ignition transport message.
  ///
  /// ## Configuration
  /// This plugin doesn't accept any custom configuration.
  class Publisher : public Plugin
  {
    Q_OBJECT

    // Message type
    Q_PROPERTY(
      QString msgType
      READ MsgType
      WRITE SetMsgType
      NOTIFY MsgTypeChanged
    )

    // Message data
    Q_PROPERTY(
      QString msgData
      READ MsgData
      WRITE SetMsgData
      NOTIFY MsgDataChanged
    )

    // Topic
    Q_PROPERTY(
      QString topic
      READ Topic
      WRITE SetTopic
      NOTIFY TopicChanged
    )

    // Frequency
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

    /// \brief
    public: QQuickItem *Item() const override;

    /// \brief Callback when publish button is checked or unchecked.
    /// \param[in] _checked True if button is checked.
    public slots: void OnPublish(const bool _checked);

    public: QString MsgType() const;
    public: void SetMsgType(const QString &_msgType);
    signals: void MsgTypeChanged();

    public: QString MsgData() const;
    public: void SetMsgData(const QString &_msgData);
    signals: void MsgDataChanged();

    public: QString Topic() const;
    public: void SetTopic(const QString &_topic);
    signals: void TopicChanged();

    public: double Frequency() const;
    public: void SetFrequency(const double _frequency);
    signals: void FrequencyChanged();

    private: QQuickItem *item;

    /// \internal
    /// \brief Pointer to private data.
    private: std::unique_ptr<PublisherPrivate> dataPtr;
  };
}
}
}

#endif
