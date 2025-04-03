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

#include <gz/msgs/image.pb.h>

#include "ImageDisplay.hh"

#include <iostream>
#include <limits>
#include <string>
#include <vector>
#include <mutex>

#include <gz/common/Console.hh>
#include <gz/common/Image.hh>
#include <gz/msgs/Utility.hh>
#include <gz/plugin/Register.hh>
#include <gz/transport/Node.hh>

#include "gz/gui/Application.hh"
#include "gz/gui/MainWindow.hh"

namespace gz::gui::plugins
{
class ImageDisplay::Implementation
{
  /// \brief List of topics publishing image messages.
  public: QStringList topicList;

  /// \brief Holds data to set as the next image
  public: msgs::Image imageMsg;

  /// \brief Node for communication.
  public: transport::Node node;

  /// \brief Mutex for accessing image data
  public: std::recursive_mutex imageMutex;

  /// \brief Mutex for variable mutated by the checkbox.
  /// The variables are: flipDepthVisualization
  public: std::mutex serviceMutex;

  /// \brief To provide images for QML.
  public: ImageProvider *provider{nullptr};

  /// \brief Enable Flip visualization flag
  public: bool enableDepthFlipCheck{true};

  /// \brief Flip visualization flag
  public: bool flipDepthVisualization{true};

  /// \brief Holds the provider name unique to this plugin instance
  public: QString providerName;
};

/////////////////////////////////////////////////
ImageDisplay::ImageDisplay()
  : dataPtr(gz::utils::MakeUniqueImpl<Implementation>())
{
  this->dataPtr->provider = new ImageProvider();
}

/////////////////////////////////////////////////
ImageDisplay::~ImageDisplay()
{
  App()->Engine()->removeImageProvider(this->ImageProviderName());
}

void ImageDisplay::RegisterImageProvider(const QString &_uniqueName)
{
  this->dataPtr->providerName = _uniqueName;
  App()->Engine()->addImageProvider(_uniqueName,
                                    this->dataPtr->provider);
}

QString ImageDisplay::ImageProviderName() {
  return this->dataPtr->providerName;
}

/////////////////////////////////////////////////
void ImageDisplay::LoadConfig(const tinyxml2::XMLElement *_pluginElem)
{
  // Default name in case user didn't define one
  if (this->title.empty())
    this->title = "Image display";

  std::string topic;
  bool topicPicker = true;
  bool showDepthFlip = true;

  // Read configuration
  if (_pluginElem)
  {
    if (auto topicElem = _pluginElem->FirstChildElement("topic"))
      topic = topicElem->GetText();

    if (auto pickerElem = _pluginElem->FirstChildElement("topic_picker"))
      pickerElem->QueryBoolText(&topicPicker);

    if (auto flipElem =
              _pluginElem->FirstChildElement("show_depth_flip"))
      flipElem->QueryBoolText(&showDepthFlip);
  }

  if (topic.empty() && !topicPicker)
  {
    gzwarn << "Can't hide topic picker without a default topic." << std::endl;
    topicPicker = true;
  }

  this->PluginItem()->setProperty("showPicker", topicPicker);
  this->PluginItem()->setProperty(
    "showDepthFlip",
    showDepthFlip
  );
  this->PluginItem()->setProperty(
    "enableDepthFlip",
    this->dataPtr->enableDepthFlipCheck
  );

  this->dataPtr->flipDepthVisualization = true;

  if (!topic.empty())
  {
    this->SetTopicList({QString::fromStdString(topic)});
  }
  else
    this->OnRefresh();
}

/////////////////////////////////////////////////
void ImageDisplay::ProcessImage()
{
  std::lock_guard<std::recursive_mutex> lock(this->dataPtr->imageMutex);
  std::lock_guard<std::mutex> service_lock(this->dataPtr->serviceMutex);

  unsigned int height = this->dataPtr->imageMsg.height();
  unsigned int width = this->dataPtr->imageMsg.width();
  QImage::Format qFormat = QImage::Format_RGB888;

  QImage image = QImage(width, height, qFormat);

  common::Image output;
  switch (this->dataPtr->imageMsg.pixel_format_type())
  {
    case msgs::PixelFormatType::RGB_INT8:
      // Disable depth flip checkbox
      this->SetEnableDepthFlip(false);

      // copy image data buffer directly to QImage
      image = QImage(reinterpret_cast<const uchar *>(
          this->dataPtr->imageMsg.data().c_str()), width, height,
          3 * width, qFormat);
      break;
    // for other cases, convert to RGB common::Image
    case msgs::PixelFormatType::R_FLOAT32:
      // Enable depth flip checkbox
      this->SetEnableDepthFlip(true);

      // specify custom min max and also flip the pixel values
      // i.e. darker pixels = higher values and brighter pixels = lower values
      common::Image::ConvertToRGBImage<float>(
          this->dataPtr->imageMsg.data().c_str(), width, height, output,
          0.0f, std::numeric_limits<float>::lowest(),
          this->dataPtr->flipDepthVisualization);
      break;
    case msgs::PixelFormatType::L_INT16:
      // Disable depth flip checkbox
      this->SetEnableDepthFlip(false);

      common::Image::ConvertToRGBImage<uint16_t>(
          this->dataPtr->imageMsg.data().c_str(), width, height, output);
       break;
    case msgs::PixelFormatType::L_INT8:
      // Disable depth flip checkbox
      this->SetEnableDepthFlip(false);

      common::Image::ConvertToRGBImage<uint8_t>(
          this->dataPtr->imageMsg.data().c_str(), width, height, output);
      break;
    case msgs::PixelFormatType::BAYER_RGGB8:
    case msgs::PixelFormatType::BAYER_BGGR8:
    case msgs::PixelFormatType::BAYER_GBRG8:
    case msgs::PixelFormatType::BAYER_GRBG8:
      // Disable depth flip checkbox
      this->SetEnableDepthFlip(false);

      common::Image::ConvertToRGBImage<uint8_t>(
          this->dataPtr->imageMsg.data().c_str(), width, height, output);
      break;
    default:
    {
      // Disable depth flip checkbox
      this->SetEnableDepthFlip(false);

      gzwarn << "Unsupported image type: "
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
    auto data = output.Data();

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
  emit this->newImage();
}

inline void ImageDisplay::SetEnableDepthFlip(bool _enable)
{
  if (_enable != this->dataPtr->enableDepthFlipCheck)
  {
    this->dataPtr->enableDepthFlipCheck = _enable;
    this->PluginItem()->setProperty("enableDepthFlip", _enable);
    gzdbg << "Enable Depth Flip: " << ((_enable) ? "Enabled": "Disabled")
          << std::endl;
  }
}

//////////////////////////////////////////////////
void ImageDisplay::SetFlipDepthVisualization(bool _value)
{
  std::lock_guard<std::mutex> service_lock(this->dataPtr->serviceMutex);
  this->dataPtr->flipDepthVisualization = _value;
  gzdbg << "Depth Visualization " << ((_value) ? "Flipped." : "Standard.")
        << std::endl;
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
  {
    // LCOV_EXCL_START
    return;
    // LCOV_EXCL_STOP
  }

  // Unsubscribe
  auto subs = this->dataPtr->node.SubscribedTopics();
  for (const auto &sub : subs)
    this->dataPtr->node.Unsubscribe(sub);

  // Subscribe to new topic
  if (!this->dataPtr->node.Subscribe(topic, &ImageDisplay::OnImageMsg,
      this))
  {
    // LCOV_EXCL_START
    gzerr << "Unable to subscribe to topic [" << topic << "]" << std::endl;
    return;
    // LCOV_EXCL_STOP
  }
  emit App()->findChild<MainWindow *>()->notifyWithDuration(
    QString::fromStdString("Subscribed to: <b>" + topic + "</b>"), 4000);
}

/////////////////////////////////////////////////
void ImageDisplay::OnRefresh()
{
  // Clear
  this->dataPtr->topicList.clear();

  // Get updated list
  std::vector<std::string> allTopics;
  this->dataPtr->node.TopicList(allTopics);
  for (const auto &topic : allTopics)
  {
    std::vector<transport::MessagePublisher> publishers;
    std::vector<transport::MessagePublisher> subscribers;
    this->dataPtr->node.TopicInfo(topic, publishers, subscribers);
    for (const auto &pub : publishers)
    {
      if (pub.MsgTypeName() == "gz.msgs.Image")
      {
        this->dataPtr->topicList.push_back(QString::fromStdString(topic));
        break;
      }
    }
  }

  // Select first one
  if (this->dataPtr->topicList.count() > 0)
    this->OnTopic(this->dataPtr->topicList.at(0));
  emit this->TopicListChanged();
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
  emit this->TopicListChanged();
}
}  // namespace gz::gui::plugins

// Register this plugin
GZ_ADD_PLUGIN(gz::gui::plugins::ImageDisplay,
              gz::gui::Plugin)
