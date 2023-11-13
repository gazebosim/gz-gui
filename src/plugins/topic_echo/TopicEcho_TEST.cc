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
#include <QRegularExpression>

#include <gz/msgs/stringmsg.pb.h>

#include <gz/common/Console.hh>
#include <gz/common/Filesystem.hh>
#include <gz/transport/Node.hh>
#include <gz/utils/ExtraTestMacros.hh>

#include "gz/gui/Application.hh"
#include "gz/gui/MainWindow.hh"
#include "gz/gui/Plugin.hh"
#include "test_config.hh"  // NOLINT(build/include)
#include "TopicEcho.hh"

int g_argc = 1;
char* g_argv[] =
{
  reinterpret_cast<char*>(const_cast<char*>("./TopicEcho_TEST")),
};

using namespace gz;
using namespace gui;

/////////////////////////////////////////////////
TEST(TopicEchoTest, GZ_UTILS_TEST_DISABLED_ON_WIN32(Load))
{
  common::Console::SetVerbosity(4);

  Application app(g_argc, g_argv);
  app.AddPluginPath(
    common::joinPaths(std::string(PROJECT_BINARY_PATH), "lib"));

  // Load plugin
  EXPECT_TRUE(app.LoadPlugin("TopicEcho"));

  // Get main window
  auto win = app.findChild<MainWindow *>();
  ASSERT_NE(win, nullptr);

  // Get plugin
  auto plugins = win->findChildren<Plugin *>();
  EXPECT_EQ(plugins.size(), 1);

  auto plugin = plugins[0];
  EXPECT_EQ(plugin->Title(), "Topic echo");

  // Cleanup
  plugins.clear();
}

/////////////////////////////////////////////////
TEST(TopicEchoTest, GZ_UTILS_TEST_DISABLED_ON_WIN32(Echo))
{
  common::Console::SetVerbosity(4);

  Application app(g_argc, g_argv);
  app.AddPluginPath(
    common::joinPaths(std::string(PROJECT_BINARY_PATH), "lib"));

  // Load plugin
  EXPECT_TRUE(app.LoadPlugin("TopicEcho"));

  // Get main window
  auto win = app.findChild<MainWindow *>();
  ASSERT_NE(win, nullptr);

  // Get plugin
  auto plugins = win->findChildren<plugins::TopicEcho *>();
  EXPECT_EQ(plugins.size(), 1);

  auto plugin = plugins[0];
  EXPECT_EQ(plugin->Title(), "Topic echo");

  // Widgets
  auto echoSwitch = plugin->PluginItem()->findChild<QObject *>("echoSwitch");
  ASSERT_NE(echoSwitch, nullptr);
  QVariant objProp = echoSwitch->property("text");
  EXPECT_TRUE(objProp.isValid());
  EXPECT_EQ(objProp.toString().toStdString(), "Echo");

  auto msgList = plugin->PluginItem()->findChild<QQuickItem *>("listView");
  ASSERT_NE(msgList, nullptr);
  objProp = msgList->property("model");
  EXPECT_TRUE(objProp.isValid());
  auto msgStringList = objProp.value<QStringListModel *>();
  ASSERT_NE(msgStringList, nullptr);
  EXPECT_EQ(msgStringList->rowCount(), 0);

  auto bufferField = plugin->PluginItem()->findChild<QObject *>("bufferField");
  ASSERT_NE(bufferField, nullptr);
  auto bufferProp = bufferField->property("value");
  EXPECT_TRUE(bufferProp.isValid());
  EXPECT_EQ(bufferProp.toInt(), 10);

  auto pauseCheck = plugin->PluginItem()->findChild<QObject *>("pauseCheck");
  ASSERT_NE(pauseCheck, nullptr);
  auto pauseProp = pauseCheck->property("checked");
  EXPECT_TRUE(pauseProp.isValid());
  EXPECT_FALSE(pauseProp.toBool());
  EXPECT_FALSE(plugin->Paused());

  // Start echoing
  plugin->OnEcho(true);

  // Publish string
  transport::Node node;
  auto pub = node.Advertise<msgs::StringMsg>("/echo");
  msgs::StringMsg msg;
  msg.set_data("example string");
  pub.Publish(msg);

  int sleep = 0;
  int maxSleep = 30;
  while(msgStringList->rowCount() == 0 && sleep < maxSleep)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    QCoreApplication::processEvents();
    ++sleep;
  }

  // Check message was echoed
  ASSERT_EQ(msgStringList->rowCount(), 1);
  EXPECT_EQ(msgStringList->stringList().at(0).toStdString(),
            "data: \"example string\"\n");

  // Publish more than buffer size (messages numbered 0 to 14)
  for (auto i = 0; i < bufferProp.toInt() + 5; ++i)
  {
    msg.Clear();
    msg.set_data("many messages: " + std::to_string(i));
    pub.Publish(msg);
  }

  auto regExp13 = QRegularExpression::wildcardToRegularExpression("*13");
  auto regExp14 = QRegularExpression::wildcardToRegularExpression("*14");

  // Wait until all 15 messages are received
  // To avoid flakiness due to messages coming out of order, we check for both
  // 13 and 14. There's a chance a lower number comes afterwards, but that's
  // just bad luck.
  sleep = 0;
  while (msgStringList->stringList().filter(regExp13).count() == 0
      && msgStringList->stringList().filter(regExp14).count() == 0
      && sleep < maxSleep)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    QCoreApplication::processEvents();
    ++sleep;
  }
  EXPECT_LT(sleep, maxSleep);

  // Check we have only 10 messages listed
  ASSERT_EQ(msgStringList->rowCount(), 10);

  // We can't guarantee the order of messages
  // We expect that out of the 10 messages last, at least 6 belong to the [5-14]
  // range
  unsigned int count = 0;
  for (auto i = 5; i < 15; ++i)
  {
    auto regExp =
      QRegularExpression::wildcardToRegularExpression("*" + QString::number(i));
    if (msgStringList->stringList().filter(regExp).count() > 0)
      ++count;
  }
  EXPECT_GE(count, 6u);

  // Increase buffer
  bufferField->setProperty("value", 20);
  bufferProp = bufferField->property("value");
  EXPECT_TRUE(bufferProp.isValid());
  EXPECT_EQ(bufferProp.toInt(), 20);

  // Publish another message and now it fits
  msg.Clear();
  msg.set_data("new message");
  pub.Publish(msg);

  sleep = 0;
  while (msgStringList->rowCount() < 11 && sleep < maxSleep)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    QCoreApplication::processEvents();
    ++sleep;
  }

  // We have 11 messages
  ASSERT_EQ(msgStringList->rowCount(), 11);

  // The last one is guaranteed to be the new message
  EXPECT_EQ(msgStringList->stringList().constLast().toStdString(),
            "data: \"new message\"\n")
            << msgStringList->stringList().constLast().toStdString();

  // Pause
  plugin->SetPaused(true);
  pauseProp = pauseCheck->property("checked");
  EXPECT_TRUE(pauseProp.toBool());

  // Publish another message and check it is not received
  msg.Clear();
  msg.set_data("dropped message");
  pub.Publish(msg);

  sleep = 0;
  while (sleep < maxSleep)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    QCoreApplication::processEvents();
    ++sleep;
  }
  ASSERT_EQ(msgStringList->rowCount(), 11);
  EXPECT_EQ(msgStringList->stringList().constLast().toStdString(),
            "data: \"new message\"\n")
            << msgStringList->stringList().constLast().toStdString();

  // Decrease buffer
  bufferField->setProperty("value", 5);
  bufferProp = bufferField->property("value");
  EXPECT_TRUE(bufferProp.isValid());
  EXPECT_EQ(bufferProp.toInt(), 5);

  // Publish another message to decrease message list
  plugin->SetPaused(false);
  msg.Clear();
  msg.set_data("new message 2");
  pub.Publish(msg);

  sleep = 0;
  while (msgStringList->rowCount() != 5 && sleep < maxSleep)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    QCoreApplication::processEvents();
    ++sleep;
  }

  // Check we have less messages
  ASSERT_EQ(msgStringList->rowCount(), 5);

  // The last message is still the new one
  EXPECT_EQ(msgStringList->stringList().constLast().toStdString(),
            "data: \"new message 2\"\n")
            << msgStringList->stringList().constLast().toStdString();

  // Stop echoing
  plugin->OnEcho(false);
  EXPECT_EQ(msgStringList->rowCount(), 0);

  // Cleanup
  plugins.clear();
}
