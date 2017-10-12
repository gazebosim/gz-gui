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

#include "ignition/gui/NumberWidget.hh"
#include "ignition/gui/QtMetatypes.hh"
#include "ignition/gui/StringWidget.hh"
#include "ignition/gui/Vector3dWidget.hh"
#include "ignition/gui/GeometryWidget.hh"

using namespace ignition;
using namespace gui;

/////////////////////////////////////////////////
TEST(GeometryWidgetTest, Signal)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  auto widget = new GeometryWidget();
  ASSERT_NE(widget, nullptr);

  // Connect signals
  bool signalReceived = false;
  widget->connect(widget, &GeometryWidget::ValueChanged,
    [&signalReceived](QVariant _var)
    {
      auto v = _var.value<msgs::Geometry>();
      EXPECT_EQ(v.type(), msgs::Geometry::BOX);
      EXPECT_DOUBLE_EQ(v.box().size().y(), 2.0);
      signalReceived = true;
    });

  // Get signal emitting widgets
  auto boxWidget = widget->findChild<Vector3dWidget *>("boxWidget");
  ASSERT_NE(nullptr, boxWidget);

  auto spins = boxWidget->findChildren<QDoubleSpinBox *>();
  EXPECT_EQ(spins.size(), 3);

  // Change the value and check new value at callback
  spins[1]->setValue(2.0);
  spins[1]->editingFinished();

  // Check callback was called
  EXPECT_TRUE(signalReceived);

  delete widget;
  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(GeometryWidgetTest, SetValue)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  auto widget = new GeometryWidget();
  ASSERT_NE(widget, nullptr);

  // Set bad value
  EXPECT_FALSE(widget->SetValue(true));

  // Set message without type
  auto msg = msgs::Geometry();
  msg.set_type(msgs::Geometry::EMPTY);
  EXPECT_FALSE(widget->SetValue(QVariant::fromValue(msg)));

  // Set message with supported type
  for (const auto &type : {msgs::Geometry::BOX,
                           msgs::Geometry::CYLINDER,
                           msgs::Geometry::SPHERE,
                           msgs::Geometry::MESH})
  {
    msg.set_type(type);
    EXPECT_TRUE(widget->SetValue(QVariant::fromValue(msg)));
    EXPECT_EQ(widget->Value().value<msgs::Geometry>().type(), type);
  }

  delete widget;
  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(GeometryWidgetTest, Dialog)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // Create widget
  auto widget = new GeometryWidget();
  ASSERT_NE(widget, nullptr);

  // Set it to mesh
  msgs::Geometry msg;
  msg.set_type(msgs::Geometry::MESH);
  widget->SetValue(QVariant::fromValue(msg));

  // Get URI line edit
  auto string = widget->findChild<StringWidget *>("meshUriWidget");
  ASSERT_NE(string, nullptr);
  EXPECT_EQ(string->Value().value<std::string>(), std::string(""));

  // Example file path
  auto exampleFile = QString::fromStdString(
        std::string(PROJECT_SOURCE_PATH) + "/test/media/fakemesh.dae");

  // Close dialog after a while
  bool closed = false;
  QTimer::singleShot(300, [&]
  {
    auto dialog = widget->findChild<QFileDialog *>();
    ASSERT_NE(dialog, nullptr);

    // Select file
    auto edits = dialog->findChildren<QLineEdit *>();
    ASSERT_GT(edits.size(), 0);
    edits[0]->setText(exampleFile);

    // Accept
    auto buttons = dialog->findChildren<QPushButton *>();
    EXPECT_GT(buttons.size(), 0);
    buttons[0]->click();

    closed = true;
  });

  // Open dialog
  auto button = widget->findChild<QPushButton *>();
  ASSERT_NE(button, nullptr);
  EXPECT_EQ(button->text(), QString("..."));
  button->click();

  while (!closed)
    QCoreApplication::processEvents();

  EXPECT_TRUE(closed);

  // Check new URI value
  EXPECT_EQ(string->Value().value<std::string>(), exampleFile.toStdString());

  delete widget;
  EXPECT_TRUE(stop());
}

