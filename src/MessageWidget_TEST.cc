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
#include "ignition/gui/BoolWidget.hh"
#include "ignition/gui/Iface.hh"
#include "ignition/gui/NumberWidget.hh"
#include "ignition/gui/PropertyWidget.hh"
#include "ignition/gui/QtMetatypes.hh"
#include "ignition/gui/StringWidget.hh"

#include "ignition/gui/MessageWidget.hh"

using namespace ignition;
using namespace gui;

/////////////////////////////////////////////////
TEST(MessageWidgetTest, ConstructAndUpdate)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // Invalid constructor
  {
    auto widget = new MessageWidget(nullptr);
    ASSERT_NE(widget, nullptr);

    EXPECT_FALSE(widget->UpdateFromMsg(new msgs::StringMsg()));
  }

  // Valid constructor, invalid update
  {
    auto widget = new MessageWidget(new msgs::StringMsg());
    ASSERT_NE(widget, nullptr);

    EXPECT_FALSE(widget->UpdateFromMsg(nullptr));
  }

  // Update type different from constructor
  {
    auto widget = new MessageWidget(new msgs::StringMsg());
    ASSERT_NE(widget, nullptr);

    EXPECT_FALSE(widget->UpdateFromMsg(new msgs::Int32()));
  }

  // Same type as constructor
  {
    auto widget = new MessageWidget(new msgs::StringMsg());
    ASSERT_NE(widget, nullptr);

    EXPECT_TRUE(widget->UpdateFromMsg(new msgs::StringMsg()));
  }

  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
// Test LINE and PLAIN_TEXT string fields
TEST(MessageWidgetTest, PluginMsgWidget)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // Message
  msgs::Plugin msg;
  msg.set_name("test_plugin");
  msg.set_filename("test_plugin_filename");
  msg.set_innerxml("<param>1</param>\n");

  // Create widget
  auto widget = new MessageWidget(&msg);
  ASSERT_NE(widget, nullptr);

  // Retrieve message
  auto retMsg = dynamic_cast<msgs::Plugin *>(widget->Msg());
  ASSERT_NE(retMsg, nullptr);

  EXPECT_EQ(retMsg->name(), "test_plugin");
  EXPECT_EQ(retMsg->filename(), "test_plugin_filename");
  EXPECT_EQ(retMsg->innerxml(), "<param>1</param>\n");

  // Update from message
  msg.set_name("test_plugin_new");
  msg.set_filename("test_plugin_filename_new");
  msg.set_innerxml("<param>2</param>\n");

  widget->UpdateFromMsg(&msg);

  // Check new message
  retMsg = dynamic_cast<msgs::Plugin *>(widget->Msg());
  ASSERT_NE(retMsg, nullptr);

  EXPECT_EQ(retMsg->name(), "test_plugin_new");
  EXPECT_EQ(retMsg->filename(), "test_plugin_filename_new");
  EXPECT_EQ(retMsg->innerxml(), "<param>2</param>\n");

  delete widget;
  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
// Test double, uint32 and bool fields
TEST(MessageWidgetTest, SurfaceMsgWidget)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // Message
  msgs::Surface msg;
  msg.set_kp(100.5);
  msg.set_collide_bitmask(1);
  msg.set_collide_without_contact(true);

  // Create widget
  auto widget = new MessageWidget(&msg);
  ASSERT_NE(widget, nullptr);

  // Retrieve message
  auto retMsg = dynamic_cast<msgs::Surface *>(widget->Msg());
  ASSERT_NE(retMsg, nullptr);

  EXPECT_DOUBLE_EQ(retMsg->kp(), 100.5);
  EXPECT_EQ(retMsg->collide_bitmask(), 1u);
  EXPECT_TRUE(retMsg->collide_without_contact());

  // Update from message
  msg.set_kp(888.44);
  msg.set_collide_bitmask(444);
  msg.set_collide_without_contact(false);

  widget->UpdateFromMsg(&msg);

  // Check new message
  retMsg = dynamic_cast<msgs::Surface *>(widget->Msg());
  ASSERT_NE(retMsg, nullptr);

  EXPECT_DOUBLE_EQ(retMsg->kp(), 888.44);
  EXPECT_EQ(retMsg->collide_bitmask(), 444u);
  EXPECT_FALSE(retMsg->collide_without_contact());

  delete widget;
  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
// Test float fields
TEST(MessageWidgetTest, LightMsgWidget)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // Message
  msgs::Light msg;
  msg.set_spot_falloff(0.5);

  // Create widget
  auto widget = new MessageWidget(&msg);
  ASSERT_NE(widget, nullptr);

  // Retrieve message
  auto retMsg = dynamic_cast<msgs::Light *>(widget->Msg());
  ASSERT_NE(retMsg, nullptr);

  EXPECT_LT(fabs(retMsg->spot_falloff()- 0.5), 0.000001);

  // Update from message
  msg.set_spot_falloff(0.001);

  widget->UpdateFromMsg(&msg);

  // Check new message
  retMsg = dynamic_cast<msgs::Light *>(widget->Msg());
  ASSERT_NE(retMsg, nullptr);

  EXPECT_LT(fabs(retMsg->spot_falloff()- 0.001), 0.000001);

  delete widget;
  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
// Test uint64 fields
TEST(MessageWidgetTest, WorldStatsMsgWidget)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // Message
  msgs::WorldStatistics msg;
  msg.set_iterations(555);

  // Create widget
  auto widget = new MessageWidget(&msg);
  ASSERT_NE(widget, nullptr);

  // Retrieve message
  auto retMsg = dynamic_cast<msgs::WorldStatistics *>(widget->Msg());
  ASSERT_NE(retMsg, nullptr);

  EXPECT_EQ(retMsg->iterations(), 555u);

  // Update from message
  msg.set_iterations(99999999);

  widget->UpdateFromMsg(&msg);

  // Check new message
  retMsg = dynamic_cast<msgs::WorldStatistics *>(widget->Msg());
  ASSERT_NE(retMsg, nullptr);

  EXPECT_EQ(retMsg->iterations(), 99999999u);

  delete widget;
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
  auto widget = new MessageWidget(msg);
  ASSERT_NE(widget, nullptr);

  // Check we got a string widget
  auto propWidget = widget->PropertyWidgetByName("data");
  ASSERT_NE(propWidget, nullptr);

  auto stringWidget = qobject_cast<StringWidget *>(propWidget);
  ASSERT_NE(stringWidget, nullptr);

  // Connect signals
  bool signalReceived = false;
  widget->connect(widget, &MessageWidget::ValueChanged,
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

  delete widget;
  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(MessageWidgetTest, ChildNumberSignal)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // Message
  auto msg = new msgs::Double();
  msg->set_data(-1.5);

  // Create widget from message
  auto widget = new MessageWidget(msg);
  ASSERT_NE(widget, nullptr);

  // Check we got a number widget
  auto propWidget = widget->PropertyWidgetByName("data");
  ASSERT_NE(propWidget, nullptr);

  auto numberWidget = qobject_cast<NumberWidget *>(propWidget);
  ASSERT_NE(numberWidget, nullptr);

  // Connect signals
  bool signalReceived = false;
  widget->connect(widget, &MessageWidget::ValueChanged,
    [&signalReceived](const std::string &_name, QVariant _var)
    {
      auto v = _var.value<double>();
      EXPECT_EQ(_name, "data");
      EXPECT_DOUBLE_EQ(v, 0.999);
      signalReceived = true;
    });

  // Get signal emitting widgets
  auto spins = widget->findChildren<QDoubleSpinBox *>();
  ASSERT_EQ(spins.size(), 1);

  // Change the value and check new value at callback
  spins[0]->setValue(0.999);
  spins[0]->editingFinished();

  // Check callback was called
  EXPECT_TRUE(signalReceived);

  delete widget;
  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(MessageWidgetTest, ChildBoolSignal)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // Message
  auto msg = new msgs::Boolean();
  msg->set_data(true);

  // Create widget from message
  auto widget = new MessageWidget(msg);
  ASSERT_NE(widget, nullptr);

  // Check we got a bool widget
  auto propWidget = widget->PropertyWidgetByName("data");
  ASSERT_NE(propWidget, nullptr);

  auto boolWidget = qobject_cast<BoolWidget *>(propWidget);
  ASSERT_NE(boolWidget, nullptr);

  // Connect signals
  bool signalReceived = false;
  widget->connect(widget, &MessageWidget::ValueChanged,
    [&signalReceived](const std::string &_name, QVariant _var)
    {
      auto v = _var.value<bool>();
      EXPECT_EQ(_name, "data");
      EXPECT_FALSE(v);
      signalReceived = true;
    });

  // Get signal emitting widgets
  auto radios = widget->findChildren<QRadioButton *>();
  EXPECT_EQ(radios.size(), 2);

  // Change the value and check new value at callback
  radios[0]->setChecked(false);
  radios[1]->setChecked(true);

  // Check callback was called
  EXPECT_TRUE(signalReceived);

  delete widget;
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
  auto widget = new MessageWidget(msg);
  ASSERT_NE(widget, nullptr);

  // Get generated widgets by name
  for (auto name : {"header", "header::stamp", "header::stamp::sec",
      "header::stamp::nsec",  "data"})
  {
    EXPECT_NE(widget->PropertyWidgetByName(name), nullptr) << name;
  }

  // Fail with invalid names
  for (auto name : {"", "banana"})
  {
    EXPECT_EQ(widget->PropertyWidgetByName(name), nullptr) << name;
  }

  delete widget;
  EXPECT_TRUE(stop());
}
