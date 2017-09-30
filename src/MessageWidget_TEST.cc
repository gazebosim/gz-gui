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

#include <ignition/msgs.hh>

#include "test_config.h"  // NOLINT(build/include)
#include "ignition/gui/Iface.hh"
#include "ignition/gui/PropertyWidget.hh"
#include "ignition/gui/QtMetatypes.hh"
#include "ignition/gui/StringWidget.hh"

#include "ignition/gui/MessageWidget.hh"

using namespace ignition;
using namespace gui;

/////////////////////////////////////////////////
TEST(MessageWidgetTest, PluginMsgWidget)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // Message
  msgs::Plugin pluginMsg;
  pluginMsg.set_name("test_plugin");
  pluginMsg.set_filename("test_plugin_filename");
  pluginMsg.set_innerxml("<param>1</param>\n");

  // Create widget
  auto msgWidget = new MessageWidget(&pluginMsg);
  EXPECT_NE(msgWidget, nullptr);

  // Retrieve message
  auto retPluginMsg = dynamic_cast<msgs::Plugin *>(msgWidget->Msg());
  EXPECT_NE(retPluginMsg, nullptr);

  EXPECT_EQ(retPluginMsg->name(), "test_plugin");
  EXPECT_EQ(retPluginMsg->filename(), "test_plugin_filename");
  EXPECT_EQ(retPluginMsg->innerxml(), "<param>1</param>\n");

  // Update from message
  pluginMsg.set_name("test_plugin_new");
  pluginMsg.set_filename("test_plugin_filename_new");
  pluginMsg.set_innerxml("<param>2</param>\n");

  msgWidget->UpdateFromMsg(&pluginMsg);

  // Check new message
  retPluginMsg = dynamic_cast<msgs::Plugin *>(msgWidget->Msg());
  EXPECT_NE(retPluginMsg, nullptr);

  EXPECT_EQ(retPluginMsg->name(), "test_plugin_new");
  EXPECT_EQ(retPluginMsg->filename(), "test_plugin_filename_new");
  EXPECT_EQ(retPluginMsg->innerxml(), "<param>2</param>\n");

  delete msgWidget;
  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(MessageWidgetTest, ChildStringSignal)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // Message
  auto msg = new msgs::StringMsg();
  msg->set_data("banana");

  // Create widget from message
  auto messageWidget = new MessageWidget(msg);
  EXPECT_TRUE(messageWidget != nullptr);

  // Check we got a string widget
  auto propWidget = messageWidget->PropertyWidgetByName("data");
  EXPECT_NE(propWidget, nullptr);

  auto stringWidget = qobject_cast<StringWidget *>(propWidget);
  EXPECT_NE(stringWidget, nullptr);

  // Connect signals
  bool signalReceived = false;
  messageWidget->connect(messageWidget, &MessageWidget::ValueChanged,
    [&signalReceived](const std::string &_name, QVariant _var)
    {
      auto v = _var.value<std::string>();
      EXPECT_EQ(_name, "data");
      EXPECT_EQ(v, "orange");
      signalReceived = true;
    });

  // Get signal emitting widgets
  auto lineEdits = stringWidget->findChildren<QLineEdit *>();
  EXPECT_EQ(lineEdits.size(), 1);

  // Change the value and check new value at callback
  lineEdits[0]->setText("orange");
  lineEdits[0]->editingFinished();

  // Check callback was called
  EXPECT_TRUE(signalReceived);

  delete messageWidget;
  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(MessageWidgetTest, PropertyByName)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // Message
  auto msg = new msgs::StringMsg();

  // Create widget from message
  auto messageWidget = new MessageWidget(msg);
  EXPECT_TRUE(messageWidget != nullptr);

  // Get generated widgets by name
  // TODO: add double fields once NumberWidget is integrated
  for (auto name : {"header", "header::stamp", /*"header::stamp::sec", */
      /*"header::stamp::nsec", */ "data"})
  {
    EXPECT_NE(messageWidget->PropertyWidgetByName(name), nullptr) << name;
  }

  // Fail with invalid names
  for (auto name : {"", "banana"})
  {
    EXPECT_EQ(messageWidget->PropertyWidgetByName(name), nullptr) << name;
  }

  delete messageWidget;
  EXPECT_TRUE(stop());
}

