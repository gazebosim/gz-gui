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

#include "test_config.h"  // NOLINT(build/include)
#include "ignition/gui/Iface.hh"
#include "ignition/gui/QtMetatypes.hh"

#include "ignition/gui/EnumWidget.hh"

using namespace ignition;
using namespace gui;

/////////////////////////////////////////////////
TEST(EnumWidgetTest, Signal)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // Create widget
  std::vector<std::string> enumValues;
  enumValues.push_back("value1");
  enumValues.push_back("value2");
  enumValues.push_back("value3");
  auto widget = new EnumWidget("an_enum", enumValues);
  ASSERT_NE(widget, nullptr);

  // Connect signals
  bool signalReceived = false;
  widget->connect(widget, &EnumWidget::ValueChanged,
    [&signalReceived](QVariant _var)
    {
      auto v = _var.value<std::string>();
      EXPECT_EQ(v, "value3");
      signalReceived = true;
    });

  // Check default value
  EXPECT_EQ(widget->Value().value<std::string>(), std::string("value1"));

  // Check key label
  auto label = widget->findChild<QLabel *>();
  ASSERT_NE(label, nullptr);
  EXPECT_EQ(label->text().toStdString(), "An enum");

  // Get signal emitting widgets
  auto comboBoxes = widget->findChildren<QComboBox *>();
  EXPECT_EQ(comboBoxes.size(), 1);

  // Change the value and check new value at callback
  comboBoxes[0]->setCurrentIndex(2);

  // Check callback was called
  EXPECT_TRUE(signalReceived);

  delete widget;
  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(EnumWidgetTest, Composing)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // Enum values
  std::vector<std::string> enumValues;
  enumValues.push_back("value1");
  enumValues.push_back("value2");
  enumValues.push_back("value3");

  // Create
  auto widget = new EnumWidget("Enum Label", enumValues);
  ASSERT_NE(widget, nullptr);

  // Check that items can be selected
  EXPECT_TRUE(widget->SetValue(QVariant::fromValue(std::string("value1"))));
  EXPECT_EQ(widget->Value().value<std::string>(), "value1");

  EXPECT_TRUE(widget->SetValue(QVariant::fromValue(std::string("value2"))));
  EXPECT_EQ(widget->Value().value<std::string>(), "value2");

  EXPECT_TRUE(widget->SetValue(QVariant::fromValue(std::string("value3"))));
  EXPECT_EQ(widget->Value().value<std::string>(), "value3");

  // Check that an inexistent item cannot be selected
  EXPECT_FALSE(widget->SetValue(QVariant::fromValue(std::string("banana"))));
  EXPECT_EQ(widget->Value().value<std::string>(), "value3");

  // Check that a bad variant cannot be selected
  EXPECT_FALSE(widget->SetValue(true));

  // Check the number of items
  auto comboBox = widget->findChild<QComboBox *>();
  ASSERT_NE(comboBox, nullptr);
  EXPECT_EQ(comboBox->count(), 3);

  // Add an item and check count
  EXPECT_TRUE(widget->AddItem("banana"));
  EXPECT_EQ(comboBox->count(), 4);

  // Check that the new item can be selected
  EXPECT_TRUE(widget->SetValue(QVariant::fromValue(std::string("banana"))));

  // Remove an item and check count
  EXPECT_TRUE(widget->RemoveItem("value2"));
  EXPECT_EQ(comboBox->count(), 3);

  // Check that the removed item cannot be selected
  EXPECT_FALSE(widget->SetValue(QVariant::fromValue(std::string("value2"))));

  // Try to remove inexistent item
  EXPECT_FALSE(widget->RemoveItem("acerola"));
  EXPECT_EQ(comboBox->count(), 3);

  // Clear all items and check count
  EXPECT_TRUE(widget->Clear());
  EXPECT_EQ(comboBox->count(), 0);

  // Check that none of the previous items can be selected
  EXPECT_FALSE(widget->SetValue(QVariant::fromValue(std::string("value1"))));
  EXPECT_FALSE(widget->SetValue(QVariant::fromValue(std::string("value2"))));
  EXPECT_FALSE(widget->SetValue(QVariant::fromValue(std::string("value3"))));
  EXPECT_FALSE(widget->SetValue(QVariant::fromValue(std::string("banana"))));

  EXPECT_TRUE(stop());
}

