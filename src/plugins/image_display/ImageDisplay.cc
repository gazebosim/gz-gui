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

#include "ImageDisplay.hh"

#include <QQuickImageProvider>

#include <algorithm>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

#include <ignition/common/Console.hh>
#include <ignition/common/Image.hh>
#include <ignition/plugin/Register.hh>
#include <ignition/transport/Node.hh>

#include "ignition/gui/Application.hh"

namespace ignition
{
namespace gui
{
namespace plugins
{
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

  class ImageDisplayPrivate
  {
    /// \brief List of topics publishing image messages.
    public: QStringList topicList;

    /// \brief Holds data to set as the next image
    public: msgs::Image imageMsg;

    /// \brief Node for communication.
    public: transport::Node node;

    /// \brief Mutex for accessing image data
    public: std::recursive_mutex imageMutex;

    /// \brief To provide images for QML.
    public: ImageProvider *provider{nullptr};
  };
}
}
}

using namespace ignition;
using namespace gui;
using namespace plugins;

/////////////////////////////////////////////////
ImageDisplay::ImageDisplay()
  : Plugin(), dataPtr(new ImageDisplayPrivate)
{
}

/////////////////////////////////////////////////
ImageDisplay::~ImageDisplay()
{
  App()->Engine()->removeImageProvider(
      this->CardItem()->objectName() + "imagedisplay");
}

/////////////////////////////////////////////////
void ImageDisplay::LoadConfig(const tinyxml2::XMLElement *_pluginElem)
{
  // Default name in case user didn't define one
  if (this->title.empty())
    this->title = "Image display";

  std::string topic;
  bool topicPicker = true;

  // Read configuration
  if (_pluginElem)
  {
    if (auto topicElem = _pluginElem->FirstChildElement("topic"))
      topic = topicElem->GetText();

    if (auto pickerElem = _pluginElem->FirstChildElement("topic_picker"))
      pickerElem->QueryBoolText(&topicPicker);
  }

  if (topic.empty() && !topicPicker)
  {
    ignwarn << "Can't hide topic picker without a default topic." << std::endl;
    topicPicker = true;
  }

  this->PluginItem()->setProperty("showPicker", topicPicker);

  if (!topic.empty())
    this->OnTopic(QString::fromStdString(topic));
  else
    this->OnRefresh();

  this->dataPtr->provider = new ImageProvider();
  App()->Engine()->addImageProvider(
      this->CardItem()->objectName() + "imagedisplay", this->dataPtr->provider);
}

/////////////////////////////////////////////////
void ImageDisplay::ProcessImage()
{
  std::lock_guard<std::recursive_mutex> lock(this->dataPtr->imageMutex);

  unsigned int height = this->dataPtr->imageMsg.height();
  unsigned int width = this->dataPtr->imageMsg.width();
  QImage::Format qFormat = QImage::Format_RGB888;

  QImage image = QImage(width, height, qFormat);

  common::Image output;
  switch (this->dataPtr->imageMsg.pixel_format_type())
  {
    case msgs::PixelFormatType::RGB_INT8:
      // copy image data buffer directly to QImage
      image = QImage(reinterpret_cast<const uchar *>(
          this->dataPtr->imageMsg.data().c_str()), width, height, qFormat);
      break;
    // for other cases, convert to RGB common::Image
    case msgs::PixelFormatType::R_FLOAT32:
      // specify custom min max and also flip the pixel values
      // i.e. darker pixels = higher values and brighter pixels = lower values
      common::Image::ConvertToRGBImage<float>(
          this->dataPtr->imageMsg.data().c_str(), width, height, output,
          0.0f, std::numeric_limits<float>::lowest(), true);
      break;
    case msgs::PixelFormatType::L_INT16:
      common::Image::ConvertToRGBImage<uint16_t>(
          this->dataPtr->imageMsg.data().c_str(), width, height, output);
       break;
    case msgs::PixelFormatType::L_INT8:
      common::Image::ConvertToRGBImage<uint8_t>(
          this->dataPtr->imageMsg.data().c_str(), width, height, output);
      break;
    default:
    {
      ignwarn << "Unsupported image type: "
              << this->dataPtr->imageMsg.pixel_format_type() << std::endl;
      return;
    }
  }
  common::Image::PixelFormatType pixelFormat =
      common::Image::ConvertPixelFormat(
      msgs::ConvertPixelFormatType(
      this->dataPtr->imageMsg.pixel_format_type()));


  // if not rgb, copy values from common::Image to QImage
  if (pixelFormat != common::Image::PixelFormatType::RGB_INT8)
  {
    unsigned int outputSize = 0;
    unsigned char *data = nullptr;
    output.Data(&data, outputSize);

    for (unsigned int j = 0; j < height; ++j)
    {
      for (unsigned int i = 0; i < width; ++i)
      {
        unsigned int idx = j*width*3 + i * 3;
        int r = data[idx];
        int g = data[idx + 1];
        int b = data[idx + 2];
        QRgb value = qRgb(r, g, b);
        image.setPixel(i, j, value);
      }
    }
  }

  this->dataPtr->provider->SetImage(image);
  this->newImage();
}

/////////////////////////////////////////////////
void ImageDisplay::OnImageMsg(const msgs::Image &_msg)
{
  std::lock_guard<std::recursive_mutex> lock(this->dataPtr->imageMutex);
  this->dataPtr->imageMsg = _msg;

  // Signal to main thread that the image changed
  QMetaObject::invokeMethod(this, "ProcessImage");
}

/////////////////////////////////////////////////
void ImageDisplay::OnTopic(const QString _topic)
{
  auto topic = _topic.toStdString();
  if (topic.empty())
    return;

  // Unsubscribe
  auto subs = this->dataPtr->node.SubscribedTopics();
  for (auto sub : subs)
    this->dataPtr->node.Unsubscribe(sub);

  // Subscribe to new topic
  if (!this->dataPtr->node.Subscribe(topic, &ImageDisplay::OnImageMsg,
      this))
  {
    ignerr << "Unable to subscribe to topic [" << topic << "]" << std::endl;
  }
}

/////////////////////////////////////////////////
void ImageDisplay::OnRefresh()
{
  // Clear
  this->dataPtr->topicList.clear();

  // Get updated list
  std::vector<std::string> allTopics;
  this->dataPtr->node.TopicList(allTopics);
  for (auto topic : allTopics)
  {
    std::vector<transport::MessagePublisher> publishers;
    this->dataPtr->node.TopicInfo(topic, publishers);
    for (auto pub : publishers)
    {
      if (pub.MsgTypeName() == "ignition.msgs.Image")
      {
        this->dataPtr->topicList.push_back(QString::fromStdString(topic));
        break;
      }
    }
  }

  // Select first one
  if (this->dataPtr->topicList.count() > 0)
    this->OnTopic(this->dataPtr->topicList.at(0));
  this->TopicListChanged();
}

/////////////////////////////////////////////////
QStringList ImageDisplay::TopicList() const
{
  return this->dataPtr->topicList;
}

/////////////////////////////////////////////////
void ImageDisplay::SetTopicList(const QStringList &_topicList)
{
  this->dataPtr->topicList = _topicList;
  this->TopicListChanged();
}

// Register this plugin
IGNITION_ADD_PLUGIN(ignition::gui::plugins::ImageDisplay,
                    ignition::gui::Plugin)
