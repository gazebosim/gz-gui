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

#include "ignition/gui/BoolWidget.hh"

using namespace ignition;
using namespace gui;

/////////////////////////////////////////////////
TEST(BoolWidgetTest, Signal)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // Create widget
  auto widget = new BoolWidget("a bool");
  EXPECT_TRUE(widget != nullptr);

  // Connect signals
  bool signalReceived = false;
  widget->connect(widget, &BoolWidget::ValueChanged,
    [&signalReceived](QVariant _var)
    {
      auto v = _var.value<bool>();
      EXPECT_TRUE(v);
      signalReceived = true;
    });

  // Check default value
  EXPECT_FALSE(widget->Value().value<bool>());

  // Check key label
  auto label = widget->findChild<QLabel *>();
  ASSERT_NE(label, nullptr);
  EXPECT_EQ(label->text().toStdString(), "A bool");

  // Get signal emitting widgets
  auto radios = widget->findChildren<QRadioButton *>();
  EXPECT_EQ(radios.size(), 2);

  // Change the value and check new value at callback
  radios[0]->setChecked(true);
  radios[1]->setChecked(false);

  // Check callback was called
  EXPECT_TRUE(signalReceived);

  delete widget;
  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(StringWidgetTest, SetValue)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // Create widget
  auto widget = new BoolWidget("a_string");
  ASSERT_NE(widget, nullptr);

  // Set good value
  EXPECT_TRUE(widget->SetValue(true));

  // Set bad value
  EXPECT_FALSE(widget->SetValue(QVariant()));

  delete widget;
  EXPECT_TRUE(stop());
}

