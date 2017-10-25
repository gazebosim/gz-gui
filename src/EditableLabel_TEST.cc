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
#include "ignition/gui/EditableLabel.hh"
#include "ignition/gui/Iface.hh"
#include "ignition/gui/qt.h"

using namespace ignition;
using namespace gui;

/////////////////////////////////////////////////
TEST(EditableLabelTest, Text)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // Create a new EditableLabel widget
  EditableLabel editableLabel("test_label", nullptr);
  EXPECT_EQ("test_label", editableLabel.Text());
  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(EditableLabelTest, Edit)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // Create a new EditableLabel widget
  auto editableLabel = new EditableLabel("test_label");
  ASSERT_NE(nullptr, editableLabel);
  editableLabel->show();

  // Get child widgets
  auto lineEdit = editableLabel->findChild<QLineEdit *>();
  ASSERT_NE(nullptr, lineEdit);

  auto label = editableLabel->findChild<QLabel *>();
  ASSERT_NE(nullptr, label);

  // Check the initial state is not editing
  EXPECT_EQ("test_label", editableLabel->Text());
  EXPECT_TRUE(label->isVisible());
  EXPECT_FALSE(lineEdit->isVisible());

  // Double-click to trigger edit mode
  QPoint center(editableLabel->width() * 0.5, editableLabel->height() * 0.5);
  auto mouseEvent = new QMouseEvent(QEvent::MouseButtonDblClick,
    center, Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
  QCoreApplication::postEvent(editableLabel, mouseEvent);
  QCoreApplication::processEvents();

  EXPECT_EQ("test_label", label->text().toStdString());
  EXPECT_FALSE(label->isVisible());
  EXPECT_TRUE(lineEdit->isVisible());

  // Edit and check new value
  lineEdit->setText("aaa");
  lineEdit->editingFinished();

  EXPECT_EQ("aaa", editableLabel->Text());
  EXPECT_TRUE(label->isVisible());
  EXPECT_FALSE(lineEdit->isVisible());

  // Double-click again to enter edit mode
  auto mouseEvent2 = new QMouseEvent(QEvent::MouseButtonDblClick,
    center, Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
  QCoreApplication::postEvent(editableLabel, mouseEvent2);
  QCoreApplication::processEvents();

  EXPECT_EQ("aaa", lineEdit->text().toStdString());
  EXPECT_FALSE(label->isVisible());
  EXPECT_TRUE(lineEdit->isVisible());

  // Test editing value and hitting escape to forget changes
  lineEdit->setText("bbb");

  auto keyboardEvent = new QKeyEvent(QEvent::KeyPress, Qt::Key_Escape,
    Qt::NoModifier);
  QCoreApplication::postEvent(lineEdit, keyboardEvent);
  QCoreApplication::processEvents();

  EXPECT_EQ("aaa", editableLabel->Text());
  EXPECT_TRUE(label->isVisible());
  EXPECT_FALSE(lineEdit->isVisible());

  delete editableLabel;
  EXPECT_TRUE(stop());
}
