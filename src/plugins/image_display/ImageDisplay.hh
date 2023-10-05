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

#ifndef GZ_GUI_PLUGINS_IMAGEDISPLAY_HH_
#define GZ_GUI_PLUGINS_IMAGEDISPLAY_HH_

#include <algorithm>
#include <memory>
#include <QQuickImageProvider>

#include <gz/msgs/image.pb.h>

#ifndef _WIN32
#  define ImageDisplay_EXPORTS_API
#else
#  if (defined(ImageDisplay_EXPORTS))
#    define ImageDisplay_EXPORTS_API __declspec(dllexport)
#  else
#    define ImageDisplay_EXPORTS_API __declspec(dllimport)
#  endif
#endif

#include "gz/gui/Plugin.hh"

namespace gz::gui::plugins
{
class ImageDisplayPrivate;

class ImageProvider : public QQuickImageProvider
{
  public: ImageProvider()
     : QQuickImageProvider(QQuickImageProvider::Image)
  {
  }

  public: QImage requestImage(const QString &, QSize *,
      const QSize &) override
  {
    if (!this->img.isNull())
    {
      // Must return a copy
      QImage copy(this->img);
      return copy;
    }

    // Placeholder in case we have no image yet
    QImage i(400, 400, QImage::Format_RGB888);
    i.fill(QColor(128, 128, 128, 100));
    return i;
  }

  public: void SetImage(const QImage &_image)
  {
    this->img = _image;
  }

  private: QImage img;
};

/// \brief Display images coming through a Gazebo Transport topic.
///
/// ## Configuration
///
/// \<topic\> : Set the topic to receive image messages.
/// \<topic_picker\> : Whether to show the topic picker, true by default. If
///                    this is false, a \<topic\> must be specified.
class ImageDisplay_EXPORTS_API ImageDisplay : public Plugin
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
  /// 'gz.msgs.StringMsg'
  /// \return Message type
  public: Q_INVOKABLE QStringList TopicList() const;

  /// \brief Set the topic list from a string, for example
  /// 'gz.msgs.StringMsg'
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
  private: void OnImageMsg(const gz::msgs::Image &_msg);

  /// \internal
  /// \brief Pointer to private data.
  private: std::unique_ptr<ImageDisplayPrivate> dataPtr;
};
}  // namespace gz::gui::plugins
#endif  // GZ_GUI_PLUGINS_IMAGEDISPLAY_HH_
