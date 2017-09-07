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

#include <iostream>
#include <ignition/common/Console.hh>
#include <ignition/common/Image.hh>
#include <ignition/common/PluginMacros.hh>
#include <ignition/transport/Node.hh>

#include "ignition/gui/plugins/ImageDisplay.hh"

namespace ignition
{
namespace gui
{
namespace plugins
{
  class ImageDisplayPrivate
  {
    /// \brief Topic dropdown
    public: QComboBox *topicsCombo;

    /// \brief Holds data to set as the next image
    public: msgs::Image imageMsg;

    /// \brief Node for communication.
    public: transport::Node node;

    /// \brief Mutex for accessing image data
    public: std::recursive_mutex imageMutex;
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

  // Main layout
  auto layout = new QVBoxLayout;

  // If should show topic picker
  if (topicPicker)
  {
    // Dropdown to choose ign topic
    this->dataPtr->topicsCombo = new QComboBox();
    this->dataPtr->topicsCombo->setObjectName("topicsCombo");
    this->dataPtr->topicsCombo->setMinimumWidth(300);
    this->dataPtr->topicsCombo->setToolTip(
        "Ignition transport topics publishing Image messages.");
    this->connect(this->dataPtr->topicsCombo,
        SIGNAL(currentIndexChanged(const QString)), this,
        SLOT(OnTopic(const QString)));

    // Button to refresh topics
    auto refreshButton = new QPushButton("Refresh");
    refreshButton->setObjectName("refreshButton");
    refreshButton->setToolTip("Refresh list of topics publishing images");
    refreshButton->setMaximumWidth(80);
    this->connect(refreshButton, SIGNAL(clicked()), this, SLOT(OnRefresh()));

    // Layout
    auto topicLayout = new QHBoxLayout;
    topicLayout->addWidget(this->dataPtr->topicsCombo);
    topicLayout->addWidget(refreshButton);

    layout->addLayout(topicLayout);
  }

  // Label to hold the image
  auto label = new QLabel("No image");

  layout->addWidget(label);
  this->setLayout(layout);

  if (!topic.empty())
    this->OnTopic(QString::fromStdString(topic));
  else
    this->OnRefresh();
}

/////////////////////////////////////////////////
void ImageDisplay::ProcessImage()
{
  std::lock_guard<std::recursive_mutex> lock(this->dataPtr->imageMutex);
  switch (this->dataPtr->imageMsg.pixel_format())
  {
    case common::Image::RGB_INT8:
      this->UpdateFromRgbInt8();
      break;
    default:
      ignerr << "Unsupported image type: " <<
          this->dataPtr->imageMsg.pixel_format() << std::endl;
  }
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
  // Unsubscribe
  auto subs = this->dataPtr->node.SubscribedTopics();
  for (auto sub : subs)
    this->dataPtr->node.Unsubscribe(sub);

  // Subscribe to new topic
  auto topic = _topic.toStdString();
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
  this->dataPtr->topicsCombo->clear();

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
        this->dataPtr->topicsCombo->addItem(QString::fromStdString(topic));
        break;
      }
    }
  }

  // Select first one
  if (this->dataPtr->topicsCombo->count() > 0)
    this->OnTopic(this->dataPtr->topicsCombo->itemText(0));
}

/////////////////////////////////////////////////
void ImageDisplay::UpdateFromRgbInt8()
{
  QImage i(this->dataPtr->imageMsg.width(), this->dataPtr->imageMsg.height(),
      QImage::Format_RGB888);

  auto const &d = this->dataPtr->imageMsg.data();
  for (unsigned int y_pixel = 0; y_pixel < this->dataPtr->imageMsg.height();
      ++y_pixel)
  {
    for (unsigned int x_pixel = 0; x_pixel < this->dataPtr->imageMsg.width();
        ++x_pixel)
    {
      int idx = x_pixel + y_pixel * this->dataPtr->imageMsg.width();
      unsigned char red = d[3 * idx];
      unsigned char green = d[3 * idx + 1];
      unsigned char blue = d[3 * idx + 2];
      i.setPixel(x_pixel, y_pixel, qRgb(red, green, blue));
    }
  }

  QPixmap pixmap;
  pixmap.convertFromImage(i);

  auto label = this->findChild<QLabel *>();

  if (label)
    label->setPixmap(pixmap);
}

// Register this plugin
IGN_COMMON_REGISTER_SINGLE_PLUGIN(ignition::gui::plugins::ImageDisplay,
                                  ignition::gui::Plugin)
