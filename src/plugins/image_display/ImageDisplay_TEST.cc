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

#include <gtest/gtest.h>
#include <ignition/common/Console.hh>
#include <ignition/common/Image.hh>
#include <ignition/transport/Node.hh>

#include "ignition/gui/Iface.hh"
#include "ignition/gui/Plugin.hh"
#include "ignition/gui/MainWindow.hh"

using namespace ignition;
using namespace gui;

/////////////////////////////////////////////////
TEST(ImageDisplayTest, Load)
{
  EXPECT_TRUE(initApp());

  EXPECT_TRUE(loadPlugin("ImageDisplay"));

  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(ImageDisplayTest, DefaultConfig)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // Load plugin
  EXPECT_TRUE(loadPlugin("ImageDisplay"));

  // Create main window
  EXPECT_TRUE(createMainWindow());
  auto win = mainWindow();
  EXPECT_TRUE(win != nullptr);

  // Get plugin
  auto plugins = win->findChildren<Plugin *>();
  EXPECT_EQ(plugins.size(), 1);
  auto plugin = plugins[0];
  EXPECT_EQ(plugin->Title(), "Image display");

  // Has a topic picker
  auto topicsCombo = plugin->findChild<QComboBox *>("topicsCombo");
  EXPECT_TRUE(topicsCombo != nullptr);
  EXPECT_EQ(topicsCombo->count(), 0);

  auto refreshButton = plugin->findChild<QPushButton *>("refreshButton");
  EXPECT_TRUE(refreshButton != nullptr);

  // No images
  auto label = plugin->findChild<QLabel *>();
  EXPECT_TRUE(label != nullptr);
  EXPECT_EQ(label->text(), "No image");

  // Cleanup
  plugins.clear();
  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(ImageDisplayTest, NoPickerNeedsTopic)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // Load plugin
  const char *pluginStr =
    "<plugin filename=\"ImageDisplay\">"
      "<topic_picker>false</topic_picker>"
    "</plugin>";

  tinyxml2::XMLDocument pluginDoc;
  pluginDoc.Parse(pluginStr);
  EXPECT_TRUE(loadPlugin("ImageDisplay",
      pluginDoc.FirstChildElement("plugin")));

  // Create main window
  EXPECT_TRUE(createMainWindow());
  auto win = mainWindow();
  EXPECT_TRUE(win != nullptr);

  // Get plugin
  auto plugins = win->findChildren<Plugin *>();
  EXPECT_EQ(plugins.size(), 1);
  auto plugin = plugins[0];
  EXPECT_EQ(plugin->Title(), "Image display");

  // Has a topic picker anyway
  auto topicsCombo = plugin->findChild<QComboBox *>("topicsCombo");
  EXPECT_TRUE(topicsCombo != nullptr);
  EXPECT_EQ(topicsCombo->count(), 0);

  auto refreshButton = plugin->findChild<QPushButton *>("refreshButton");
  EXPECT_TRUE(refreshButton != nullptr);

  // No images
  auto label = plugin->findChild<QLabel *>();
  EXPECT_TRUE(label != nullptr);
  EXPECT_EQ(label->text(), "No image");

  // Cleanup
  plugins.clear();
  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(ImageDisplayTest, ReceiveImage)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // Load plugin
  const char *pluginStr =
    "<plugin filename=\"ImageDisplay\">"
      "<topic>/image_test</topic>"
      "<topic_picker>false</topic_picker>"
    "</plugin>";

  tinyxml2::XMLDocument pluginDoc;
  pluginDoc.Parse(pluginStr);
  EXPECT_TRUE(loadPlugin("ImageDisplay",
      pluginDoc.FirstChildElement("plugin")));

  // Create main window
  EXPECT_TRUE(createMainWindow());
  auto win = mainWindow();
  EXPECT_TRUE(win != nullptr);

  // Get plugin
  auto plugins = win->findChildren<Plugin *>();
  EXPECT_EQ(plugins.size(), 1);
  auto plugin = plugins[0];
  EXPECT_EQ(plugin->Title(), "Image display");

  // Doesn't have a topic picker
  EXPECT_EQ(plugin->findChildren<QComboBox *>().size(), 0);
  EXPECT_EQ(plugin->findChildren<QPushButton *>().size(), 0);

  // Starts with no image
  auto label = plugin->findChild<QLabel *>();
  EXPECT_TRUE(label != nullptr);
  EXPECT_EQ(label->text(), "No image");
  EXPECT_TRUE(label->pixmap() == nullptr);

  // Publish images
  transport::Node node;
  auto pub = node.Advertise<msgs::Image>("/image_test");

  // Unsupported type
  {
    msgs::Image msg;
    msg.set_height(100);
    msg.set_width(200);
    msg.set_pixel_format(common::Image::RGB_FLOAT32);
    pub.Publish(msg);
  }

  // Give it time to be processed
  int sleep = 0;
  int maxSleep = 10;
  while (!label->text().isEmpty() && sleep < maxSleep)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    QCoreApplication::processEvents();
    sleep++;
  }

  // Still no image
  EXPECT_EQ(label->text(), "No image");
  EXPECT_TRUE(label->pixmap() == nullptr);

  // Good message
  {
    msgs::Image msg;
    msg.set_height(100);
    msg.set_width(200);
    msg.set_pixel_format(common::Image::RGB_INT8);
    pub.Publish(msg);
  }

  // Give it time to be processed
  sleep = 0;
  while (!label->text().isEmpty() && sleep < maxSleep)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    QCoreApplication::processEvents();
    sleep++;
  }

  // Now it has an image
  EXPECT_TRUE(label->text().isEmpty());
  ASSERT_NE(nullptr, label->pixmap());
  EXPECT_EQ(label->pixmap()->height(), 100);
  EXPECT_EQ(label->pixmap()->width(), 200);

  // Cleanup
  plugins.clear();
  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(ImageDisplayTest, TopicPicker)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // Load plugin
  EXPECT_TRUE(loadPlugin("ImageDisplay"));

  // Create main window
  EXPECT_TRUE(createMainWindow());
  auto win = mainWindow();
  EXPECT_TRUE(win != nullptr);

  // Get plugin
  auto plugins = win->findChildren<Plugin *>();
  EXPECT_EQ(plugins.size(), 1);
  auto plugin = plugins[0];
  EXPECT_EQ(plugin->Title(), "Image display");

  // Topic picker starts empty
  auto topicsCombo = plugin->findChild<QComboBox *>("topicsCombo");
  EXPECT_TRUE(topicsCombo != nullptr);
  EXPECT_EQ(topicsCombo->count(), 0);

  auto refreshButton = plugin->findChild<QPushButton *>("refreshButton");
  EXPECT_TRUE(refreshButton != nullptr);

  // Refresh and still empty
  refreshButton->click();
  EXPECT_EQ(topicsCombo->count(), 0);

  // Advertise topics
  transport::Node node;
  auto pubImage = node.Advertise<msgs::Image>("/image_test");
  auto pubImage2 = node.Advertise<msgs::Image>("/image_test_2");
  auto pubString = node.Advertise<msgs::StringMsg>("/string_test");

  // Refresh and now we have image topics
  refreshButton->click();
  EXPECT_EQ(topicsCombo->count(), 2);
  EXPECT_EQ(topicsCombo->itemText(0), "/image_test");
  EXPECT_EQ(topicsCombo->itemText(1), "/image_test_2");

  // Pick topics
  topicsCombo->setCurrentIndex(1);
  topicsCombo->setCurrentIndex(0);
  topicsCombo->setCurrentIndex(1);

  // Cleanup
  plugins.clear();
  EXPECT_TRUE(stop());
}
