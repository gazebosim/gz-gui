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

  // Check it has a button and no content
  auto layout = widget->layout();
  EXPECT_EQ(layout->count(), 1);

  // Add content
  layout->addWidget(new QLabel("banana"));
  layout->addWidget(new QLabel("grape"));
  widget->show();

  // Check the new content is visible
  for (auto i = 0; i < layout->count(); ++i)
  {
    EXPECT_TRUE(layout->itemAt(i)->widget()->isVisible());
  }

  // Collapse
  widget->Toggle(false);

  // Check the content is not visible
  for (auto i = 1; i < layout->count(); ++i)
  {
    EXPECT_FALSE(layout->itemAt(i)->widget()->isVisible());
  }

  // Expand
  widget->Toggle(true);

  // Check the content is visible
  for (auto i = 1; i < layout->count(); ++i)
  {
    EXPECT_TRUE(layout->itemAt(i)->widget()->isVisible());
  }

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

