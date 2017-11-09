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

#include "test_config.h"  // NOLINT(build/include)
#include "ignition/gui/Iface.hh"

#include "ignition/gui/NumberWidget.hh"
#include "ignition/gui/CollapsibleWidget.hh"

using namespace ignition;
using namespace gui;

/////////////////////////////////////////////////
TEST(CollapsibleWidgetTest, Toggle)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // Create widget
  auto widget = new CollapsibleWidget("collapse_me");
  ASSERT_NE(widget, nullptr);

  // Check key label
  auto label = widget->findChild<QLabel *>();
  ASSERT_NE(label, nullptr);
  EXPECT_EQ(label->text().toStdString(), "Collapse me");

  // Check the content is empty
  EXPECT_EQ(widget->ContentCount(), 0u);

  // Add content
  widget->AppendContent(new QDoubleSpinBox());
  widget->AppendContent(new QDoubleSpinBox());
  widget->show();
  EXPECT_EQ(widget->ContentCount(), 2u);

  // Check the new content is not visible (start collapsed)
  EXPECT_FALSE(widget->IsExpanded());
  auto spins = widget->findChildren<QDoubleSpinBox *>();
  ASSERT_EQ(spins.size(), 2);
  EXPECT_FALSE(spins[0]->isVisible());
  EXPECT_FALSE(spins[1]->isVisible());

  // Expand
  widget->Toggle(true);
  EXPECT_TRUE(widget->IsExpanded());

  // Check the content is visible
  EXPECT_TRUE(spins[0]->isVisible());
  EXPECT_TRUE(spins[1]->isVisible());

  // Collapse
  widget->Toggle(false);
  EXPECT_FALSE(widget->IsExpanded());

  // Check the content is not visible
  EXPECT_FALSE(spins[0]->isVisible());
  EXPECT_FALSE(spins[1]->isVisible());

  delete widget;
  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(CollapsibleWidgetTest, Value)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // Create widget
  auto widget = new CollapsibleWidget("collapse_me");
  ASSERT_NE(widget, nullptr);

  // Fail to set value if it has no children
  EXPECT_FALSE(widget->SetValue(QVariant()));

  // Invalid value if it has no children
  EXPECT_EQ(widget->Value(), QVariant());

  // Add a property
  auto numberProp = new NumberWidget("a_double");
  widget->layout()->addWidget(numberProp);

  // Set invalid value to first widget
  EXPECT_FALSE(widget->SetValue(QVariant()));

  // Set valid value to first widget
  EXPECT_TRUE(widget->SetValue(-0.5));
  EXPECT_DOUBLE_EQ(widget->Value().toDouble(), -0.5);

  delete widget;
  EXPECT_TRUE(stop());
}

