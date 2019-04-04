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
#include <ignition/common/Console.hh>
#include <ignition/math/Color.hh>

#include "test_config.h"  // NOLINT(build/include)
#include "ignition/gui/Iface.hh"

#include "ignition/gui/Conversions.hh"
#include "ignition/gui/QtMetatypes.hh"
#include "ignition/gui/ColorWidget.hh"

using namespace ignition;
using namespace gui;

/////////////////////////////////////////////////
TEST(ColorWidgetTest, Signal)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // Create widget
  auto widget = new ColorWidget();
  ASSERT_NE(widget, nullptr);

  // Connect signals
  bool signalReceived = false;
  widget->connect(widget, &ColorWidget::ValueChanged,
    [&signalReceived](QVariant _var)
    {
      auto v = _var.value<math::Color>();
      EXPECT_EQ(v, math::Color(0.5, 0, 0));
      signalReceived = true;
    });

  // Check default value
  EXPECT_EQ(widget->Value().value<math::Color>(), math::Color());

  // Get signal emitting widgets
  auto spins = widget->findChildren<QDoubleSpinBox *>();
  ASSERT_EQ(spins.size(), 4);

  // Change the value and check new value at callback
  spins[0]->setValue(0.5);
  spins[0]->editingFinished();

  // Check callback was called
  EXPECT_TRUE(signalReceived);

  delete widget;
  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(ColorWidgetTest, SetValue)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  auto widget = new ColorWidget();
  ASSERT_NE(widget, nullptr);

  // Set good value
  EXPECT_TRUE(widget->SetValue(QVariant::fromValue(
      math::Color(0, 1, 2))));

  // Set bad value
  EXPECT_FALSE(widget->SetValue(true));

  delete widget;
  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(ColorWidgetTest, Dialog)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // Create widget
  auto widget = new ColorWidget();
  ASSERT_NE(widget, nullptr);

  // Get button
  auto button = widget->findChild<QPushButton *>();
  ASSERT_NE(button, nullptr);
  EXPECT_EQ(button->text(), QString("..."));

  // Close dialog after a while
  bool closed = false;
  QTimer::singleShot(300, [&]
  {
    auto dialog = widget->findChild<QColorDialog *>();
    ASSERT_NE(dialog, nullptr);

    EXPECT_EQ(convert(dialog->currentColor()), math::Color());

    dialog->close();
    closed = true;
  });

  // Open dialog
  button->click();

  while (!closed)
    QCoreApplication::processEvents();

  EXPECT_TRUE(closed);

  // Check value wasn't changed
  EXPECT_EQ(widget->Value().value<math::Color>(), math::Color());

  // Close dialog after a while
  closed = false;
  QTimer::singleShot(300, [&]
  {
    auto dialog = widget->findChild<QColorDialog *>();
    ASSERT_NE(dialog, nullptr);

    // Change color
    dialog->currentColorChanged(Qt::blue);

    dialog->close();
    closed = true;
  });

  // Open dialog again
  button->click();

  while (!closed)
    QCoreApplication::processEvents();

  EXPECT_TRUE(closed);

  // Check value was changed
  EXPECT_EQ(widget->Value().value<math::Color>(), math::Color::Blue);

  delete widget;
  EXPECT_TRUE(stop());
}

