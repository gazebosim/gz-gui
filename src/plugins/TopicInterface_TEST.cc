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
#include <ignition/msgs.hh>
#include <ignition/transport/Node.hh>

#include "ignition/gui/Iface.hh"
#include "ignition/gui/Plugin.hh"
#include "ignition/gui/PropertyWidget.hh"
#include "ignition/gui/MainWindow.hh"
#include "ignition/gui/MessageWidget.hh"

using namespace ignition;
using namespace gui;

/////////////////////////////////////////////////
TEST(TopicInterfaceTest, Load)
{
  EXPECT_TRUE(initApp());

  EXPECT_TRUE(loadPlugin("TopicInterface"));

  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(TopicInterfaceTest, BadMessageType)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // Load plugin
  const char *pluginStr =
    "<plugin filename=\"TopicInterface\">"
      "<topic>/fruit</topic>"
      "<message_type>ignition.msgs.Fruit</message_type>"
    "</plugin>";

  tinyxml2::XMLDocument pluginDoc;
  pluginDoc.Parse(pluginStr);
  EXPECT_TRUE(ignition::gui::loadPlugin("TopicInterface",
      pluginDoc.FirstChildElement("plugin")));

  // Create main window
  EXPECT_TRUE(createMainWindow());
  auto win = mainWindow();
  EXPECT_TRUE(win != nullptr);

  // Get plugin
  auto plugins = win->findChildren<Plugin *>();
  EXPECT_EQ(plugins.size(), 1);
  auto plugin = plugins[0];
  EXPECT_EQ(plugin->Title(), "Topic interface");

  // Check no message widget was created
  auto msgWidgets = plugin->findChildren<MessageWidget *>();
  EXPECT_TRUE(msgWidgets.empty());

  // Cleanup
  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(TopicInterfaceTest, OnMessage)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // Load plugin
  const char *pluginStr =
    "<plugin filename=\"TopicInterface\">"
      "<topic>/plugins</topic>"
      "<message_type>ignition.msgs.Plugin_V</message_type>"
    "</plugin>";

  tinyxml2::XMLDocument pluginDoc;
  pluginDoc.Parse(pluginStr);
  EXPECT_TRUE(ignition::gui::loadPlugin("TopicInterface",
      pluginDoc.FirstChildElement("plugin")));

  // Create main window
  EXPECT_TRUE(createMainWindow());
  auto win = mainWindow();
  EXPECT_TRUE(win != nullptr);

  // Get plugin
  auto plugins = win->findChildren<Plugin *>();
  EXPECT_EQ(plugins.size(), 1);
  auto plugin = plugins[0];
  EXPECT_EQ(plugin->Title(), "Topic interface");

  // Check message widget was created
  auto msgWidgets = plugin->findChildren<MessageWidget *>();
  ASSERT_EQ(msgWidgets.size(), 1);

  // Check it was populated
  auto propertyWidgets = msgWidgets[0]->findChildren<PropertyWidget *>();
  EXPECT_EQ(propertyWidgets.size(), 6);

  // Publish a message
  transport::Node node;
  auto pub = node.Advertise<msgs::Plugin_V>("/plugins");

  {
    msgs::Plugin_V msg;

    auto pluginsMsg0 = msg.add_plugins();
    pluginsMsg0->set_name("test_plugin");
    pluginsMsg0->set_filename("test_plugin_filename");
    pluginsMsg0->set_innerxml("<param>1</param>\n");

    pub.Publish(msg);
  }

  int sleep = 0;
  int maxSleep = 30;
  while (propertyWidgets.size() <= 6 && sleep < maxSleep)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    QCoreApplication::processEvents();
    propertyWidgets = msgWidgets[0]->findChildren<PropertyWidget *>();
    sleep++;
  }
  EXPECT_EQ(propertyWidgets.size(), 15);

  // Check contents
  auto retMsg = dynamic_cast<msgs::Plugin_V *>(msgWidgets[0]->Msg());
  ASSERT_NE(retMsg, nullptr);
  ASSERT_EQ(retMsg->plugins_size(), 1);

  EXPECT_EQ(retMsg->plugins(0).name(), "test_plugin");
  EXPECT_EQ(retMsg->plugins(0).filename(), "test_plugin_filename");
  EXPECT_EQ(retMsg->plugins(0).innerxml(), "<param>1</param>\n");

  // Cleanup
  EXPECT_TRUE(stop());
}

