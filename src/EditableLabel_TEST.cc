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
  EditableLabel *editableLabel =
      new EditableLabel("test_label", nullptr);
  ASSERT_NE(nullptr, editableLabel);
  QLineEdit *lineEdit = editableLabel->findChild<QLineEdit *>();
  ASSERT_NE(nullptr, lineEdit);
  EXPECT_EQ("test_label", editableLabel->Text());

  editableLabel->show();

  // test editing value and hitting enter to save
  QPoint center(editableLabel->width() * 0.5, editableLabel->height() * 0.5);
  QMouseEvent *mouseEvent = new QMouseEvent(QEvent::MouseButtonDblClick,
    center, Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
  QCoreApplication::postEvent(editableLabel, mouseEvent);
  QCoreApplication::processEvents();

  lineEdit->setText("aaa");
  lineEdit->editingFinished();
  EXPECT_EQ("aaa", editableLabel->Text());

  // Test editing value and hitting escape to forget changes
  mouseEvent = new QMouseEvent(QEvent::MouseButtonDblClick,
    center, Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
  QCoreApplication::postEvent(editableLabel, mouseEvent);
  QCoreApplication::processEvents();

  lineEdit->setText("bbb");
  QKeyEvent *keyboardEvent = new QKeyEvent(QEvent::KeyPress, Qt::Key_Escape,
    Qt::NoModifier);
  QCoreApplication::postEvent(lineEdit, keyboardEvent);
  QCoreApplication::processEvents();
  EXPECT_EQ("aaa", editableLabel->Text());

  delete editableLabel;
  EXPECT_TRUE(stop());
}
