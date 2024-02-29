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

#include <gz/msgs/stringmsg.pb.h>

#include <gz/common/Console.hh>
#include <gz/common/Image.hh>
#include <gz/transport/Node.hh>
#include <gz/utils/ExtraTestMacros.hh>

#include "gz/gui/Application.hh"
#include "gz/gui/MainWindow.hh"
#include "gz/gui/Plugin.hh"
#include "test_config.hh"  // NOLINT(build/include)
#include "ImageDisplay.hh"

int g_argc = 1;
char* g_argv[] =
{
  reinterpret_cast<char*>(const_cast<char*>("./ImageDisplay_TEST")),
};

using namespace gz;
using namespace gui;

/////////////////////////////////////////////////
TEST(ImageDisplayTest, GZ_UTILS_TEST_DISABLED_ON_WIN32(Load))
{
  common::Console::SetVerbosity(4);

  Application app(g_argc, g_argv);
  app.AddPluginPath(
    common::joinPaths(std::string(PROJECT_BINARY_PATH), "lib"));

  // Load plugin
  EXPECT_TRUE(app.LoadPlugin("ImageDisplay"));

  // Get main window
  auto win = app.findChild<MainWindow *>();
  ASSERT_NE(win, nullptr);

  // Get plugin
  auto plugins = win->findChildren<plugins::ImageDisplay *>();
  EXPECT_EQ(plugins.size(), 1);

  auto plugin = plugins[0];
  EXPECT_EQ(plugin->Title(), "Image display");

  // Cleanup
  plugins.clear();
}

/////////////////////////////////////////////////
TEST(ImageDisplayTest, GZ_UTILS_TEST_DISABLED_ON_WIN32(DefaultConfig))
{
  common::Console::SetVerbosity(4);

  Application app(g_argc, g_argv);
  app.AddPluginPath(
    common::joinPaths(std::string(PROJECT_BINARY_PATH), "lib"));

  // Load plugin
  EXPECT_TRUE(app.LoadPlugin("ImageDisplay"));

  // Get main window
  auto win = app.findChild<MainWindow *>();
  ASSERT_NE(win, nullptr);

  // Get plugin
  auto plugins = win->findChildren<plugins::ImageDisplay *>();
  EXPECT_EQ(plugins.size(), 1);
  auto plugin = plugins[0];
  EXPECT_EQ(plugin->Title(), "Image display");

  // Has a topic picker
  auto topicsCombo = plugin->PluginItem()->findChild<QObject *>("topicsCombo");
  ASSERT_NE(topicsCombo, nullptr);
  auto topicProp = topicsCombo->property("model");
  EXPECT_TRUE(topicProp.isValid());
  auto topicList = topicProp.toStringList();
  EXPECT_EQ(topicList.size(), 0);

  auto refreshButton =
    plugin->PluginItem()->findChild<QObject *>("refreshButton");
  ASSERT_NE(refreshButton, nullptr);

  auto picker =
    topicsCombo->parent();  // RowLayout that holds `visible: showPicker`
  ASSERT_NE(picker, nullptr);
  auto pickerProp = picker->property("visible");
  EXPECT_TRUE(pickerProp.isValid());
  EXPECT_TRUE(pickerProp.toBool());

  // No images (gray image)
  auto providerBase = app.Engine()->imageProvider(
      plugin->CardItem()->objectName() + "imagedisplay");
  ASSERT_NE(providerBase, nullptr);
  auto imageProvider = static_cast<plugins::ImageProvider *>(providerBase);
  ASSERT_NE(imageProvider, nullptr);
  QSize dummySize;
  QImage img = imageProvider->requestImage(QString(), &dummySize, dummySize);
  EXPECT_TRUE(img.allGray());

  // Cleanup
  plugins.clear();
}

/////////////////////////////////////////////////
TEST(ImageDisplayTest, GZ_UTILS_TEST_DISABLED_ON_WIN32(NoPickerNeedsTopic))
{
  common::Console::SetVerbosity(4);

  Application app(g_argc, g_argv);
  app.AddPluginPath(
    common::joinPaths(std::string(PROJECT_BINARY_PATH), "lib"));

  // Load plugin
  const char *pluginStr =
    "<plugin filename=\"ImageDisplay\">"
      "<topic_picker>false</topic_picker>"
    "</plugin>";

  tinyxml2::XMLDocument pluginDoc;
  pluginDoc.Parse(pluginStr);
  EXPECT_TRUE(app.LoadPlugin("ImageDisplay",
      pluginDoc.FirstChildElement("plugin")));

  // Get main window
  auto win = app.findChild<MainWindow *>();
  ASSERT_NE(win, nullptr);

  // Get plugin
  auto plugins = win->findChildren<plugins::ImageDisplay *>();
  EXPECT_EQ(plugins.size(), 1);
  auto plugin = plugins[0];
  EXPECT_EQ(plugin->Title(), "Image display");

  // Has a topic picker anyway
  auto topicsCombo = plugin->PluginItem()->findChild<QObject *>("topicsCombo");
  ASSERT_NE(topicsCombo, nullptr);
  auto topicProp = topicsCombo->property("model");
  EXPECT_TRUE(topicProp.isValid());
  auto topicList = topicProp.toStringList();
  EXPECT_EQ(topicList.size(), 0);

  auto refreshButton =
    plugin->PluginItem()->findChild<QObject *>("refreshButton");
  ASSERT_NE(refreshButton, nullptr);

  auto picker =
    topicsCombo->parent();  // RowLayout that holds `visible: showPicker`
  ASSERT_NE(picker, nullptr);
  auto pickerProp = picker->property("visible");
  EXPECT_TRUE(pickerProp.isValid());
  EXPECT_TRUE(pickerProp.toBool());

  // No images (gray image)
  auto providerBase = app.Engine()->imageProvider(
      plugin->CardItem()->objectName() + "imagedisplay");
  ASSERT_NE(providerBase, nullptr);
  auto imageProvider = static_cast<plugins::ImageProvider *>(providerBase);
  ASSERT_NE(imageProvider, nullptr);
  QSize dummySize;
  QImage img = imageProvider->requestImage(QString(), &dummySize, dummySize);
  EXPECT_TRUE(img.allGray());

  // Cleanup
  plugins.clear();
}

/////////////////////////////////////////////////
TEST(ImageDisplayTest, GZ_UTILS_TEST_DISABLED_ON_WIN32(ReceiveImage))
{
  common::Console::SetVerbosity(4);

  Application app(g_argc, g_argv);
  app.AddPluginPath(
    common::joinPaths(std::string(PROJECT_BINARY_PATH), "lib"));

  // Load plugin
  const char *pluginStr =
    "<plugin filename=\"ImageDisplay\">"
      "<topic>/image_test</topic>"
      "<topic_picker>false</topic_picker>"
    "</plugin>";

  tinyxml2::XMLDocument pluginDoc;
  pluginDoc.Parse(pluginStr);
  EXPECT_TRUE(app.LoadPlugin("ImageDisplay",
      pluginDoc.FirstChildElement("plugin")));

  // Get main window
  auto win = app.findChild<MainWindow *>();
  ASSERT_NE(win, nullptr);

  // Get plugin
  auto plugins = win->findChildren<plugins::ImageDisplay *>();
  EXPECT_EQ(plugins.size(), 1);
  auto plugin = plugins[0];
  EXPECT_EQ(plugin->Title(), "Image display");

  // Doesn't have a topic picker by checking `showPicker == false`
  auto topicsCombo = plugin->PluginItem()->findChild<QObject *>("topicsCombo");
  ASSERT_NE(topicsCombo, nullptr);
  auto picker =
    topicsCombo->parent();  // RowLayout that holds `visible: showPicker`
  ASSERT_NE(picker, nullptr);
  auto pickerProp = picker->property("visible");
  EXPECT_TRUE(pickerProp.isValid());
  EXPECT_FALSE(pickerProp.toBool());

  // Starts with no image (gray image)
  auto providerBase = app.Engine()->imageProvider(
      plugin->CardItem()->objectName() + "imagedisplay");
  ASSERT_NE(providerBase, nullptr);
  auto imageProvider = static_cast<plugins::ImageProvider *>(providerBase);
  ASSERT_NE(imageProvider, nullptr);
  QSize dummySize;
  QImage img = imageProvider->requestImage(QString(), &dummySize, dummySize);
  EXPECT_TRUE(img.allGray());

  // Publish images
  transport::Node node;
  auto pub = node.Advertise<msgs::Image>("/image_test");

  // Unsupported type
  {
    msgs::Image msg;
    msg.set_height(100);
    msg.set_width(200);
    msg.set_pixel_format_type(msgs::PixelFormatType::RGB_FLOAT32);
    pub.Publish(msg);
  }

  // Give it time to be processed
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  QCoreApplication::processEvents();
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  // Still no image
  img = imageProvider->requestImage(QString(), &dummySize, dummySize);
  EXPECT_TRUE(img.allGray());

  // Good message
  {
    msgs::Image msg;
    msg.set_height(100);
    msg.set_width(200);
    msg.set_pixel_format_type(msgs::PixelFormatType::RGB_INT8);
    // bytes per pixel = channels * bytes = 3 * 1
    int bpp = 3;
    msg.set_step(msg.width() * bpp);

    // red image
    int bufferSize = msg.width() * msg.height() * bpp;
    std::shared_ptr<unsigned char> buffer(new unsigned char[bufferSize]);
    for (int i = 0; i < bufferSize; i += bpp)
    {
      buffer.get()[i] = 255u;
      buffer.get()[i + 1] = 0u;
      buffer.get()[i + 2] = 0u;
    }
    msg.set_data(buffer.get(), bufferSize);
    pub.Publish(msg);
  }

  // Give it time to be processed
  int sleep = 0;
  int maxSleep = 30;
  while (img.allGray() && sleep < maxSleep)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    QCoreApplication::processEvents();
    img = imageProvider->requestImage(QString(), &dummySize, dummySize);
    ++sleep;
  }

  // Now it has an image
  EXPECT_FALSE(img.allGray());
  EXPECT_EQ(img.height(), 100);
  EXPECT_EQ(img.width(), 200);

  // check image is red
  for (int y = 0; y < img.height(); ++y)
  {
    for (int x = 0; x < img.width(); ++x)
    {
      EXPECT_EQ(img.pixelColor(x, y).red(), 255);
      EXPECT_EQ(img.pixelColor(x, y).green(), 0);
      EXPECT_EQ(img.pixelColor(x, y).blue(), 0);
    }
  }

  // Cleanup
  plugins.clear();
}

/////////////////////////////////////////////////
TEST(ImageDisplayTest, GZ_UTILS_TEST_DISABLED_ON_WIN32(ReceiveImageFloat32))
{
  common::Console::SetVerbosity(4);

  Application app(g_argc, g_argv);
  app.AddPluginPath(
    common::joinPaths(std::string(PROJECT_BINARY_PATH), "lib"));

  // Load plugin
  const char *pluginStr =
    "<plugin filename=\"ImageDisplay\">"
      "<topic>/image_test</topic>"
    "</plugin>";

  tinyxml2::XMLDocument pluginDoc;
  pluginDoc.Parse(pluginStr);
  EXPECT_TRUE(app.LoadPlugin("ImageDisplay",
      pluginDoc.FirstChildElement("plugin")));

  // Get main window
  auto win = app.findChild<MainWindow *>();
  ASSERT_NE(win, nullptr);

  // Get plugin
  auto plugins = win->findChildren<plugins::ImageDisplay *>();
  EXPECT_EQ(plugins.size(), 1);
  auto plugin = plugins[0];
  EXPECT_EQ(plugin->Title(), "Image display");

  // Starts with no image
  auto providerBase = app.Engine()->imageProvider(
      plugin->CardItem()->objectName() + "imagedisplay");
  ASSERT_NE(providerBase, nullptr);
  auto imageProvider = static_cast<plugins::ImageProvider *>(providerBase);
  ASSERT_NE(imageProvider, nullptr);
  QSize dummySize;
  QImage img = imageProvider->requestImage(QString(), &dummySize, dummySize);
  // When there is no image yet, a placeholder image with size 400 is given
  // See ImageDisplay.hh, ImageProvider for more details
  int placeholderSize = 400;
  EXPECT_EQ(img.width(), placeholderSize);
  EXPECT_EQ(img.height(), placeholderSize);

  // Publish images
  transport::Node node;
  auto pub = node.Advertise<msgs::Image>("/image_test");

  // Good message
  {
    msgs::Image msg;
    msg.set_height(32);
    msg.set_width(32);
    msg.set_pixel_format_type(msgs::PixelFormatType::R_FLOAT32);
    // bytes per pixel = channels * bytes = 1 * 4
    int bpp = 4;
    msg.set_step(msg.width() * bpp);

    // first half is gray, second half is black
    int bufferSize = msg.width() * msg.height() * bpp;
    std::shared_ptr<float> buffer(new float[bufferSize]);
    for (unsigned int y = 0; y < msg.width(); ++y)
    {
      float v = 0.5f * static_cast<int>(y / (msg.height() / 2.0) + 1);
      for (unsigned int x = 0; x < msg.height(); ++x)
      {
        buffer.get()[y * msg.width() + x] = v;
      }
    }

    msg.set_data(buffer.get(), bufferSize);
    pub.Publish(msg);
  }

  // Give it time to be processed
  int sleep = 0;
  int maxSleep = 30;
  while (img.width() == placeholderSize && sleep < maxSleep)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    QCoreApplication::processEvents();
    img = imageProvider->requestImage(QString(), &dummySize, dummySize);
    ++sleep;
  }

  EXPECT_EQ(img.width(), 32);
  EXPECT_EQ(img.height(), 32);

  // check image half gray & half black
  for (int y = 0; y < img.height(); ++y)
  {
    for (int x = 0; x < img.width(); ++x)
    {
      if (y < img.height() / 2)
      {
        // expect gray
        EXPECT_EQ(img.pixelColor(x, y).red(), 127);
        EXPECT_EQ(img.pixelColor(x, y).green(), 127);
        EXPECT_EQ(img.pixelColor(x, y).blue(), 127);
      }
      else
      {
        // expect black
        EXPECT_EQ(img.pixelColor(x, y).red(), 0);
        EXPECT_EQ(img.pixelColor(x, y).green(), 0);
        EXPECT_EQ(img.pixelColor(x, y).blue(), 0);
      }
    }
  }

  // Cleanup
  plugins.clear();
}

/////////////////////////////////////////////////
TEST(ImageDisplayTest, GZ_UTILS_TEST_DISABLED_ON_WIN32(ReceiveImageInt16))
{
  common::Console::SetVerbosity(4);

  Application app(g_argc, g_argv);
  app.AddPluginPath(
    common::joinPaths(std::string(PROJECT_BINARY_PATH), "lib"));

  // Load plugin
  const char *pluginStr =
    "<plugin filename=\"ImageDisplay\">"
      "<topic>/image_test</topic>"
    "</plugin>";

  tinyxml2::XMLDocument pluginDoc;
  pluginDoc.Parse(pluginStr);
  EXPECT_TRUE(app.LoadPlugin("ImageDisplay",
      pluginDoc.FirstChildElement("plugin")));

  // Get main window
  auto win = app.findChild<MainWindow *>();
  ASSERT_NE(win, nullptr);

  // Get plugin
  auto plugins = win->findChildren<plugins::ImageDisplay *>();
  EXPECT_EQ(plugins.size(), 1);
  auto plugin = plugins[0];
  EXPECT_EQ(plugin->Title(), "Image display");

  // Starts with no image
  auto providerBase = app.Engine()->imageProvider(
      plugin->CardItem()->objectName() + "imagedisplay");
  ASSERT_NE(providerBase, nullptr);
  auto imageProvider = static_cast<plugins::ImageProvider *>(providerBase);
  ASSERT_NE(imageProvider, nullptr);
  QSize dummySize;
  QImage img = imageProvider->requestImage(QString(), &dummySize, dummySize);
  // When there is no image yet, a placeholder image with size 400 is given
  // See ImageDisplay.hh, ImageProvider for more details
  int placeholderSize = 400;
  EXPECT_EQ(img.width(), placeholderSize);
  EXPECT_EQ(img.height(), placeholderSize);

  // Publish images
  transport::Node node;
  auto pub = node.Advertise<msgs::Image>("/image_test");

  // Good message
  {
    msgs::Image msg;
    msg.set_height(32);
    msg.set_width(32);
    msg.set_pixel_format_type(msgs::PixelFormatType::L_INT16);
    // bytes per pixel = channels * bytes = 1 * 2
    int bpp = 2;
    msg.set_step(msg.width() * bpp);

    // first half is black, second half is white
    int bufferSize = msg.width() * msg.height() * bpp;
    std::shared_ptr<uint16_t> buffer(new uint16_t[bufferSize]);
    for (unsigned int y = 0; y < msg.width(); ++y)
    {
      uint16_t v = 100 * static_cast<int>(y / (msg.height() / 2.0) + 1);
      for (unsigned int x = 0; x < msg.height(); ++x)
      {
        buffer.get()[y * msg.width() + x] = v;
      }
    }

    msg.set_data(buffer.get(), bufferSize);
    pub.Publish(msg);
  }

  // Give it time to be processed
  int sleep = 0;
  int maxSleep = 30;
  while (img.width() == placeholderSize && sleep < maxSleep)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    QCoreApplication::processEvents();
    img = imageProvider->requestImage(QString(), &dummySize, dummySize);
    ++sleep;
  }

  EXPECT_EQ(img.width(), 32);
  EXPECT_EQ(img.height(), 32);

  // check image half gray & half black
  for (int y = 0; y < img.height(); ++y)
  {
    for (int x = 0; x < img.width(); ++x)
    {
      if (y < img.height() / 2)
      {
        // expect black
        EXPECT_EQ(img.pixelColor(x, y).red(), 0);
        EXPECT_EQ(img.pixelColor(x, y).green(), 0);
        EXPECT_EQ(img.pixelColor(x, y).blue(), 0);
      }
      else
      {
        // expect white
        EXPECT_EQ(img.pixelColor(x, y).red(), 255);
        EXPECT_EQ(img.pixelColor(x, y).green(), 255);
        EXPECT_EQ(img.pixelColor(x, y).blue(), 255);
      }
    }
  }

  // Cleanup
  plugins.clear();
}

/////////////////////////////////////////////////
TEST(ImageDisplayTest, GZ_UTILS_TEST_DISABLED_ON_WIN32(TopicPicker))
{
  common::Console::SetVerbosity(4);

  Application app(g_argc, g_argv);
  app.AddPluginPath(
    common::joinPaths(std::string(PROJECT_BINARY_PATH), "lib"));

  // Load plugin
  EXPECT_TRUE(app.LoadPlugin("ImageDisplay"));

  // Get main window
  auto win = app.findChild<MainWindow *>();
  ASSERT_NE(win, nullptr);

  // Get plugin
  auto plugins = win->findChildren<plugins::ImageDisplay *>();
  EXPECT_EQ(plugins.size(), 1);
  auto plugin = plugins[0];
  EXPECT_EQ(plugin->Title(), "Image display");

  // Topic picker starts empty
  auto topicsCombo = plugin->PluginItem()->findChild<QObject *>("topicsCombo");
  ASSERT_NE(topicsCombo, nullptr);
  auto topicProp = topicsCombo->property("model");
  EXPECT_TRUE(topicProp.isValid());
  auto topicList = topicProp.toStringList();
  EXPECT_EQ(topicList.size(), 0);
  EXPECT_EQ(topicList.size(), plugin->TopicList().size());

  // Refresh and still empty
  plugin->OnRefresh();
  topicProp = topicsCombo->property("model");
  EXPECT_TRUE(topicProp.isValid());
  topicList = topicProp.toStringList();
  EXPECT_EQ(topicList.size(), 0);
  EXPECT_EQ(topicList.size(), plugin->TopicList().size());

  // Advertise topics
  transport::Node node;
  auto pubImage = node.Advertise<msgs::Image>("/image_test");
  auto pubImage2 = node.Advertise<msgs::Image>("/image_test_2");
  auto pubString = node.Advertise<msgs::StringMsg>("/string_test");

  // Refresh and now we have image topics
  plugin->OnRefresh();
  topicProp = topicsCombo->property("model");
  EXPECT_TRUE(topicProp.isValid());
  topicList = topicProp.toStringList();
  EXPECT_EQ(topicList.size(), 2);
  EXPECT_EQ(topicList.size(), plugin->TopicList().size());

  EXPECT_EQ(topicList.at(0).toStdString(), "/image_test");
  EXPECT_EQ(topicList.at(1).toStdString(), "/image_test_2");
  EXPECT_EQ(topicList.at(0), plugin->TopicList().at(0));
  EXPECT_EQ(topicList.at(1), plugin->TopicList().at(1));

  // Set image topics
  QStringList newTopicList = {"/new_image_test"};
  plugin->SetTopicList(newTopicList);

  topicProp = topicsCombo->property("model");
  EXPECT_TRUE(topicProp.isValid());
  topicList = topicProp.toStringList();
  EXPECT_EQ(topicList.size(), 1);
  EXPECT_EQ(topicList.size(), plugin->TopicList().size());
  EXPECT_EQ(topicList.at(0).toStdString(), "/new_image_test");
  EXPECT_EQ(topicList.at(0), plugin->TopicList().at(0));

  // Cleanup
  plugins.clear();
}
