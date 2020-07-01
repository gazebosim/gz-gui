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
#include <ignition/msgs/stringmsg.pb.h>
#include <ignition/transport/Node.hh>

#include "test_config.h"  // NOLINT(build/include)
#include "ignition/gui/Application.hh"
#include "ignition/gui/Plugin.hh"
#include "ignition/gui/MainWindow.hh"

#include "Publisher.hh"

int g_argc = 1;
char **g_argv = new char *[g_argc];

using namespace ignition;
using namespace gui;

// See https://github.com/ignitionrobotics/ign-gui/issues/75
#if not defined(__APPLE__) && not defined(_WIN32)
/////////////////////////////////////////////////
TEST(PublisherTest, Load)
{
  common::Console::SetVerbosity(4);

  Application app(g_argc, g_argv);
  app.AddPluginPath(std::string(PROJECT_BINARY_PATH) + "/lib");

  EXPECT_TRUE(app.LoadPlugin("Publisher"));

  // Get main window
  auto win = app.findChild<MainWindow *>();
  ASSERT_NE(nullptr, win);

  // Get plugin
  auto plugins = win->findChildren<Plugin *>();
  EXPECT_EQ(plugins.size(), 1);

  auto plugin = plugins[0];
  EXPECT_EQ(plugin->Title(), "Publisher");

  // Cleanup
  plugins.clear();
}

/////////////////////////////////////////////////
TEST(PublisherTest, Publish)
{
  common::Console::SetVerbosity(4);

  Application app(g_argc, g_argv);
  app.AddPluginPath(std::string(PROJECT_BINARY_PATH) + "/lib");

  // Load plugin
  const char *pluginStr =
    "<plugin filename=\"Publisher\">"
      "<ignition-gui>"
        "<title>Publisher!</title>"
      "</ignition-gui>"
    "</plugin>";

  tinyxml2::XMLDocument pluginDoc;
  EXPECT_EQ(tinyxml2::XML_SUCCESS, pluginDoc.Parse(pluginStr));
  EXPECT_TRUE(app.LoadPlugin("Publisher",
      pluginDoc.FirstChildElement("plugin")));

  // Get main window
  auto win = app.findChild<MainWindow *>();
  ASSERT_NE(nullptr, win);

  // Show, but don't exec, so we don't block
  win->QuickWindow()->show();

  // Get plugin
  auto plugins = win->findChildren<plugins::Publisher *>();
  EXPECT_EQ(plugins.size(), 1);

  auto plugin = plugins[0];
  EXPECT_EQ(plugin->Title(), "Publisher!");

  // Message type
  EXPECT_EQ(plugin->MsgType(), "ignition.msgs.StringMsg");

  // Message
  EXPECT_EQ(plugin->MsgData(), "data: \"Hello\"");

  // Topic
  EXPECT_EQ(plugin->Topic(), "/echo");

  // Frequency
  EXPECT_DOUBLE_EQ(plugin->Frequency(), 1.0);

  // Subscribe
  bool received = false;
  std::function<void(const msgs::StringMsg &)> cb =
      [&](const msgs::StringMsg &_msg)
  {
    EXPECT_EQ(_msg.data(), "Hello");
    received = true;
  };
  transport::Node node;
  node.Subscribe("/echo", cb);

  EXPECT_FALSE(received);

  // Publish
  plugin->OnPublish(true);

  int sleep = 0;
  int maxSleep = 30;
  while (!received && sleep < maxSleep)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    QCoreApplication::processEvents();
    sleep++;
  }

  EXPECT_TRUE(received);
  received = false;

  // Stop publishing
  plugin->OnPublish(false);

  sleep = 0;
  // cppcheck-suppress knownConditionTrueFalse
  while (!received && sleep < maxSleep)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    QCoreApplication::processEvents();
    sleep++;
  }

  EXPECT_FALSE(received);

  // Publish once
  plugin->SetFrequency(0.0);
  plugin->OnPublish(true);

  sleep = 0;
  while (!received && sleep < maxSleep)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    QCoreApplication::processEvents();
    sleep++;
  }

  EXPECT_TRUE(received);
  plugin->OnPublish(false);
  received = false;

  // Bad message type
  plugin->SetFrequency(1.0);
  plugin->SetMsgType("banana.message");
  plugin->OnPublish(true);

  sleep = 0;
  // cppcheck-suppress knownConditionTrueFalse
  while (!received && sleep < maxSleep)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    QCoreApplication::processEvents();
    sleep++;
  }
  EXPECT_FALSE(received);
  plugin->OnPublish(false);

  // Bad message type - msg combination
  plugin->SetMsgType("ignition.msgs.StringMsg");
  plugin->SetMsgData("banana: apple");
  plugin->OnPublish(true);

  sleep = 0;
  while (!received && sleep < maxSleep)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    QCoreApplication::processEvents();
    sleep++;
  }
  EXPECT_FALSE(received);
  plugin->OnPublish(false);

  // Cleanup
  plugins.clear();
}

//////////////////////////////////////////////////
TEST(PublisherTest, ParamsFromSDF)
{
  common::Console::SetVerbosity(4);

  Application app(g_argc, g_argv);
  app.AddPluginPath(std::string(PROJECT_BINARY_PATH) + "/lib");

  // Load plugin
  const char *pluginStr =
    "<plugin filename=\"Publisher\">"
      "<topic>/fruit</topic>"
      "<message>number: 1 fruit {name:\"banana\"}</message>"
      "<message_type>ignition.msgs.Fruits</message_type>"
      "<frequency>0.1</frequency>"
    "</plugin>";

  tinyxml2::XMLDocument pluginDoc;
  EXPECT_EQ(tinyxml2::XML_SUCCESS, pluginDoc.Parse(pluginStr));
  EXPECT_TRUE(app.LoadPlugin("Publisher",
      pluginDoc.FirstChildElement("plugin")));

  // Get main window
  auto win = app.findChild<MainWindow *>();
  ASSERT_NE(nullptr, win);

  // Show, but don't exec, so we don't block
  win->QuickWindow()->show();

  // Get plugin
  auto plugins = win->findChildren<plugins::Publisher *>();
  EXPECT_EQ(plugins.size(), 1);

  auto plugin = plugins[0];
  EXPECT_EQ(plugin->Title(), "Publisher");

  // Message type
  EXPECT_EQ(plugin->MsgType(), "ignition.msgs.Fruits");

  // Message
  EXPECT_EQ(plugin->MsgData(), "number: 1 fruit {name:\"banana\"}");

  // Topic
  EXPECT_EQ(plugin->Topic(), "/fruit");

  // Frequency
  EXPECT_DOUBLE_EQ(plugin->Frequency(), 0.1);
}
#endif
