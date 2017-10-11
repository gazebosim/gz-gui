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
#include "ignition/gui/Vector3dWidget.hh"

using namespace ignition;
using namespace gui;

/////////////////////////////////////////////////
TEST(Vector3dWidgetTest, Signal)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // Create widget
  auto widget = new Vector3dWidget("a vector");
  ASSERT_NE(widget, nullptr);

  // Connect signals
  bool signalReceived = false;
  widget->connect(widget, &Vector3dWidget::ValueChanged,
    [&signalReceived](QVariant _var)
    {
      auto v = _var.value<math::Vector3d>();
      EXPECT_EQ(v, math::Vector3d(0, -4.5, 0));
      signalReceived = true;
    });

  // Check default value
  EXPECT_EQ(widget->Value().value<math::Vector3d>(), math::Vector3d::Zero);

  // Get signal emitting widgets
  auto spins = widget->findChildren<QDoubleSpinBox *>();
  ASSERT_EQ(spins.size(), 3);

  // Get preset combo
  auto combos = widget->findChildren<QComboBox *>();
  EXPECT_EQ(combos.size(), 1);
  EXPECT_EQ(combos[0]->currentIndex(), 0);

  // Change the value, finish edit, and check new value at callback
  spins[1]->setValue(-4.5);
  spins[1]->editingFinished();

  // Check callback was called
  EXPECT_TRUE(signalReceived);

  // Change values without finishing editi and check preset
  spins[0]->setValue(0.0);
  spins[1]->setValue(0.0);
  spins[2]->setValue(-1.0);
  spins[2]->valueChanged(-1.0);
  EXPECT_EQ(combos[0]->currentIndex(), 6);

  delete widget;
  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(Vector3dWidgetTest, SetValue)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  auto widget = new Vector3dWidget("a vector");
  ASSERT_NE(widget, nullptr);

  // Set good value
  EXPECT_TRUE(widget->SetValue(QVariant::fromValue(
      math::Vector3d(0, 1, 2))));

  // Set bad value
  EXPECT_FALSE(widget->SetValue(true));

  delete widget;
  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(Vector3dWidgetTest, Presets)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // Create widget
  auto widget = new Vector3dWidget("a vector");
  ASSERT_NE(widget, nullptr);

  // Get preset combo
  auto combos = widget->findChildren<QComboBox *>();
  EXPECT_EQ(combos.size(), 1);

  // +X
  combos[0]->setCurrentIndex(1);
  EXPECT_EQ(widget->Value().value<math::Vector3d>(), math::Vector3d::UnitX);

  // -X
  combos[0]->setCurrentIndex(2);
  EXPECT_EQ(widget->Value().value<math::Vector3d>(), -math::Vector3d::UnitX);

  // +Y
  combos[0]->setCurrentIndex(3);
  EXPECT_EQ(widget->Value().value<math::Vector3d>(), math::Vector3d::UnitY);

  // -Y
  combos[0]->setCurrentIndex(4);
  EXPECT_EQ(widget->Value().value<math::Vector3d>(), -math::Vector3d::UnitY);

  // +Z
  combos[0]->setCurrentIndex(5);
  EXPECT_EQ(widget->Value().value<math::Vector3d>(), math::Vector3d::UnitZ);

  // -Z
  combos[0]->setCurrentIndex(6);
  EXPECT_EQ(widget->Value().value<math::Vector3d>(), -math::Vector3d::UnitZ);

  // Invalid index doesn't change anything
  combos[0]->setCurrentIndex(7);
  EXPECT_EQ(widget->Value().value<math::Vector3d>(), -math::Vector3d::UnitZ);

  delete widget;
  EXPECT_TRUE(stop());
}

