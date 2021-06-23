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

#ifndef IGNITION_GUI_PLUGINS_IMAGEDISPLAY_HH_
#define IGNITION_GUI_PLUGINS_IMAGEDISPLAY_HH_

#include <memory>
#ifdef _MSC_VER
#pragma warning(push, 0)
#endif
#include <ignition/msgs/image.pb.h>
#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include "ignition/gui/Plugin.hh"

namespace ignition
{
namespace gui
{
namespace plugins
{
  class ImageDisplayPrivate;

  /// \brief Display images coming through an Ignition transport topic.
  ///
  /// ## Configuration
  ///
  /// \<topic\> : Set the topic to receive image messages.
  /// \<topic_picker\> : Whether to show the topic picker, true by default. If
  ///                    this is false, a \<topic\> must be specified.
  class ImageDisplay : public Plugin
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
    public: ImageDisplay();

    /// \brief Destructor
    public: virtual ~ImageDisplay();

    // Documentation inherited
    public: virtual void LoadConfig(const tinyxml2::XMLElement *_pluginElem);

    /// \brief Callback when refresh button is pressed.
    public slots: void OnRefresh();

    /// \brief Callback when a new topic is chosen on the combo box.
    public slots: void OnTopic(const QString _topic);

    /// \brief Get the topic list as a string, for example
    /// 'ignition.msgs.StringMsg'
    /// \return Message type
    public: Q_INVOKABLE QStringList TopicList() const;

    /// \brief Set the topic list from a string, for example
    /// 'ignition.msgs.StringMsg'
    /// \param[in] _topicList Message type
    public: Q_INVOKABLE void SetTopicList(const QStringList &_topicList);

    /// \brief Notify that topic list has changed
    signals: void TopicListChanged();

    /// \brief Notify that a new image has been received.
    signals: void newImage();

    /// \brief Callback in main thread when image changes
    private slots: void ProcessImage();

    /// \brief Subscriber callback when new image is received
    /// \param[in] _msg New image
    private: void OnImageMsg(const ignition::msgs::Image &_msg);

    /// \internal
    /// \brief Pointer to private data.
    private: std::unique_ptr<ImageDisplayPrivate> dataPtr;
  };
}
}
}

#endif
