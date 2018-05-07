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
#include "ignition/gui/Pose3dWidget.hh"

using namespace ignition;
using namespace gui;

/////////////////////////////////////////////////
TEST(Pose3dWidgetTest, Signal)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // Create widget
  auto widget = new Pose3dWidget();
  ASSERT_NE(widget, nullptr);

  // Connect signals
  bool signalReceived = false;
  widget->connect(widget, &Pose3dWidget::ValueChanged,
    [&signalReceived](QVariant _var)
    {
      auto v = _var.value<math::Pose3d>();
      EXPECT_EQ(v, math::Pose3d(0, 0, -0.1, 0, 0, 0));
      signalReceived = true;
    });

  // Check default value
  EXPECT_EQ(widget->Value().value<math::Pose3d>(), math::Pose3d::Zero);

  // Get signal emitting widgets
  auto spins = widget->findChildren<QDoubleSpinBox *>();
  ASSERT_EQ(spins.size(), 6);

  // Change the value and check new value at callback
  spins[2]->setValue(-0.1);
  spins[2]->editingFinished();

  // Check callback was called
  EXPECT_TRUE(signalReceived);

  delete widget;
  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(Pose3dWidgetTest, SetValue)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  auto widget = new Pose3dWidget();
  ASSERT_NE(widget, nullptr);

  // Set good value
  EXPECT_TRUE(widget->SetValue(QVariant::fromValue(
      math::Pose3d(0, 1, 2, 0, 1, 0))));

  // Set bad value
  EXPECT_FALSE(widget->SetValue(true));

  delete widget;
  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(Pose3dWidgetTest, SetDragAndDrop)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  auto widget = new Pose3dWidget();
  ASSERT_NE(widget, nullptr);

  std::string uri = "aURI";
  EXPECT_TRUE(widget->DragAndDropURI().empty());
  widget->SetDragAndDropURI(uri);
  EXPECT_EQ(uri, widget->DragAndDropURI());

  delete widget;
  EXPECT_TRUE(stop());
}
