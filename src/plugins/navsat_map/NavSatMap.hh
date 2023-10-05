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

#ifndef GZ_GUI_PLUGINS_IMAGEDISPLAY_HH_
#define GZ_GUI_PLUGINS_IMAGEDISPLAY_HH_

#include <memory>

#include <gz/msgs/navsat.pb.h>

#include "gz/gui/Plugin.hh"

namespace gz::gui::plugins
{
class NavSatMapPrivate;

/// \brief Display NavSat messages coming through a Gazebo Transport topic
/// on top of a map.
///
/// ## Configuration
///
/// \<topic\> : Set the topic to receive NavSat messages.
/// \<topic_picker\> : Whether to show the topic picker, true by default. If
///                    this is false, a \<topic\> must be specified.
class NavSatMap : public Plugin
{
  Q_OBJECT

  /// \brief Topic list
  Q_PROPERTY(
    QStringList topicList
    READ TopicList
    WRITE SetTopicList
    NOTIFY TopicListChanged
  )

  /// \brief Constructor
  public: NavSatMap();

  /// \brief Destructor
  public: virtual ~NavSatMap();

  // Documentation inherited
  public: virtual void LoadConfig(const tinyxml2::XMLElement *_pluginElem);

  /// \brief Callback when refresh button is pressed.
  public slots: void OnRefresh();

  /// \brief Callback when a new topic is chosen on the combo box.
  public slots: void OnTopic(const QString _topic);

  /// \brief Get the list of topics publishing NavSat messages
  /// \return List of topics
  public: Q_INVOKABLE QStringList TopicList() const;

  /// \brief Set the topic list
  /// \param[in] _topicList List of topics
  public: Q_INVOKABLE void SetTopicList(const QStringList &_topicList);

  /// \brief Notify that topic list has changed
  signals: void TopicListChanged();

  /// \brief Notify that a new message has been received.
  /// \param[in] _latitudeDeg Latitude in degrees
  /// \param[in] _longitudeDeg Longitude in degrees
  signals: void newMessage(double _latitudeDeg, double _longitudeDeg);

  /// \brief Callback in main thread when message changes
  private slots: void ProcessMessage();

  /// \brief Subscriber callback when new message is received
  /// \param[in] _msg New message
  private: void OnMessage(const gz::msgs::NavSat &_msg);

  /// \internal
  /// \brief Pointer to private data.
  private: std::unique_ptr<NavSatMapPrivate> dataPtr;
};
}  // namespace gz::gui::plugins
#endif  // GZ_GUI_PLUGINS_IMAGEDISPLAY_HH_
