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

    /// \brief holds data to set as the next image
    public: ignition::msgs::Image imageMsg;

    /// \brief tools for setting up a subscriber
    public: ignition::transport::Node node;

    /// \brief mutex for accessing image data
    public: std::mutex imageMutex;
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
  this->title = "Image display";

  // Dropdown to choose ign topic
  this->dataPtr->topicsCombo = new QComboBox();
  this->dataPtr->topicsCombo->setMinimumWidth(300);
  this->connect(this->dataPtr->topicsCombo, SIGNAL(activated(const QString)), this,
      SLOT(OnTopic(const QString)));

  // Button to refresh topics
  auto refreshButton = new QPushButton("Refresh");
  refreshButton->setMaximumWidth(80);
  this->connect(refreshButton, SIGNAL(clicked()), this, SLOT(OnRefresh()));

  // Label to hold the image
  auto label = new QLabel("No image");

  // Layout
  auto topicLayout = new QHBoxLayout;
  topicLayout->addWidget(this->dataPtr->topicsCombo);
  topicLayout->addWidget(refreshButton);

  auto layout = new QVBoxLayout;
  layout->addLayout(topicLayout);
  layout->addWidget(label);
  this->setLayout(layout);

  this->OnRefresh();
}

/////////////////////////////////////////////////
ImageDisplay::~ImageDisplay()
{
}

/////////////////////////////////////////////////
void ImageDisplay::ProcessImage()
{
  std::lock_guard<std::mutex> lock(this->dataPtr->imageMutex);
  switch(this->dataPtr->imageMsg.pixel_format())
  {
    case ignition::common::Image::RGB_INT8:
      this->UpdateFromRgbInt8();
      break;
    default:
      std::cerr << "Unsupported image type: " << this->dataPtr->imageMsg.pixel_format()
        << std::endl;
  }
}

/////////////////////////////////////////////////
void ImageDisplay::OnImageMsg(const ignition::msgs::Image &_msg)
{
  std::lock_guard<std::mutex> lock(this->dataPtr->imageMutex);
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
  for (unsigned int y_pixel = 0; y_pixel < this->dataPtr->imageMsg.height(); ++y_pixel)
  {
    for (unsigned int x_pixel = 0; x_pixel < this->dataPtr->imageMsg.width(); ++x_pixel)
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
