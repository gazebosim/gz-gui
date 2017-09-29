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

#include "ignition/gui/NumberWidget.hh"

using namespace ignition;
using namespace gui;

/////////////////////////////////////////////////
TEST(NumberWidgetTest, DoubleSignal)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // Create widget
  auto widget = new NumberWidget("a double number");
  EXPECT_TRUE(widget != nullptr);

  // Connect signals
  bool signalReceived = false;
  widget->connect(widget, &NumberWidget::ValueChanged,
    [&signalReceived](QVariant _var)
    {
      auto v = _var.value<double>();
      EXPECT_DOUBLE_EQ(v, -1.5);
      signalReceived = true;
    });

  // Check default value
  EXPECT_DOUBLE_EQ(widget->Value().value<double>(), 0.0);

  // Check key label
  auto label = widget->findChild<QLabel *>();
  ASSERT_NE(label, nullptr);
  EXPECT_EQ(label->text().toStdString(), "A double number");

  // Get signal emitting widgets
  auto spins = widget->findChildren<QDoubleSpinBox *>();
  ASSERT_EQ(spins.size(), 1);

  // Change the value and check new value at callback
  spins[0]->setValue(-1.5);
  spins[0]->editingFinished();

  // Check callback was called
  EXPECT_TRUE(signalReceived);

  delete widget;
  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(NumberWidgetTest, IntSignal)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // Create widget
  auto widget = new NumberWidget("a int number", NumberWidget::INT);
  EXPECT_TRUE(widget != nullptr);

  // Connect signals
  bool signalReceived = false;
  widget->connect(widget, &NumberWidget::ValueChanged,
    [&signalReceived](QVariant _var)
    {
      auto v = _var.value<int>();
      EXPECT_EQ(v, -9999);
      signalReceived = true;
    });

  // Check default value
  EXPECT_EQ(widget->Value().value<int>(), 0);

  // Check key label
  auto label = widget->findChild<QLabel *>();
  ASSERT_NE(label, nullptr);
  EXPECT_EQ(label->text().toStdString(), "A int number");

  // Get signal emitting widgets
  auto spins = widget->findChildren<QSpinBox *>();
  ASSERT_EQ(spins.size(), 1);

  // Change the value and check new value at callback
  spins[0]->setValue(-9999);
  spins[0]->editingFinished();

  // Check callback was called
  EXPECT_TRUE(signalReceived);

  delete widget;
  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(NumberWidgetTest, UIntSignal)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // Create widget
  auto widget = new NumberWidget("an unsigned int number", NumberWidget::UINT);
  EXPECT_TRUE(widget != nullptr);

  // Connect signals
  bool signalReceived = false;
  widget->connect(widget, &NumberWidget::ValueChanged,
    [&signalReceived](QVariant _var)
    {
      auto v = _var.value<unsigned int>();
      EXPECT_EQ(v, 66);
      signalReceived = true;
    });

  // Check default value
  EXPECT_EQ(widget->Value().value<unsigned int>(), 0);

  // Check key label
  auto label = widget->findChild<QLabel *>();
  ASSERT_NE(label, nullptr);
  EXPECT_EQ(label->text().toStdString(), "An unsigned int number");

  // Get signal emitting widgets
  auto spins = widget->findChildren<QSpinBox *>();
  ASSERT_EQ(spins.size(), 1);

  // Change the value and check new value at callback
  spins[0]->setValue(66);
  spins[0]->editingFinished();

  // Check callback was called
  EXPECT_TRUE(signalReceived);

  delete widget;
  EXPECT_TRUE(stop());
}


