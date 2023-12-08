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

#include <gz/common/Console.hh>
#include <gz/common/MouseEvent.hh>
#include <gz/math/Color.hh>

#include "test_config.hh"  // NOLINT(build/include)
#include "gz/gui/Conversions.hh"

using namespace gz;
using namespace gui;

/////////////////////////////////////////////////
TEST(ConversionsTest, Color)
{
  // Gazebo to Qt to Gazebo
  {
    double red = 0.1;
    double green = 0.3;
    double blue = 0.5;
    double alpha = 0.7;

    double tol = 0.01;

    math::Color color(red, green, blue, alpha);
    auto newColor = convert(convert(color));

    EXPECT_NEAR(newColor.R(), newColor.R(), tol);
    EXPECT_NEAR(newColor.G(), newColor.G(), tol);
    EXPECT_NEAR(newColor.B(), newColor.B(), tol);
    EXPECT_NEAR(newColor.A(), newColor.A(), tol);
  }

  // Qt to Gazebo to Qt
  {
    int red = 100;
    int green = 150;
    int blue = 200;
    int alpha = 186;

    QColor color(red, green, blue, alpha);
    EXPECT_EQ(convert(convert(color)), color);
  }
}

/////////////////////////////////////////////////
TEST(ConversionsTest, Point2d)
{
  double x = -0.5;
  double y = 123;

  // Gazebo to Qt to Gazebo
  {
    math::Vector2d point(x, y);
    EXPECT_EQ(convert(convert(point)), point);
  }

  // Qt to Gazebo to Qt
  {
    QPointF point(x, y);
    EXPECT_EQ(convert(convert(point)), point);
  }
}

/////////////////////////////////////////////////
TEST(ConversionsTest, Vector3d)
{
  double x = -0.1;
  double y = 0;
  double z = 1234;

  // Gazebo to Qt to Gazebo
  {
    math::Vector3d vec(x, y, z);
    EXPECT_EQ(convert(convert(vec)), vec);
  }

  // Qt to Gazebo to Qt
  {
    QVector3D vec(x, y, z);
    EXPECT_EQ(convert(convert(vec)), vec);
  }
}

/////////////////////////////////////////////////
TEST(ConversionsTest, MouseEvent)
{
  // Press + Shift
  {
    QMouseEvent qtEvent(QEvent::MouseButtonPress,
                        QPointF(10, 20), QPointF(10, 20),
                        Qt::RightButton,
                        Qt::MiddleButton,
                        Qt::ShiftModifier);

    auto gzEvent = convert(qtEvent);

    EXPECT_EQ(gzEvent.Type(), common::MouseEvent::PRESS);
    EXPECT_EQ(gzEvent.Pos(), math::Vector2i(10, 20));
    EXPECT_EQ(gzEvent.Button(), common::MouseEvent::RIGHT);
    EXPECT_EQ(gzEvent.Buttons(), common::MouseEvent::MIDDLE);
    EXPECT_FALSE(gzEvent.Dragging());
    EXPECT_TRUE(gzEvent.Shift());
  }

  // Release + Control
  {
    QMouseEvent qtEvent(QEvent::MouseButtonRelease,
                        QPointF(0, 200), QPointF(0, 200),
                        Qt::MiddleButton,
                        Qt::RightButton,
                        Qt::ControlModifier);

    auto gzEvent = convert(qtEvent);

    EXPECT_EQ(gzEvent.Type(), common::MouseEvent::RELEASE);
    EXPECT_EQ(gzEvent.Pos(), math::Vector2i(0, 200));
    EXPECT_EQ(gzEvent.Button(), common::MouseEvent::MIDDLE);
    EXPECT_EQ(gzEvent.Buttons(), common::MouseEvent::RIGHT);
    EXPECT_FALSE(gzEvent.Dragging());
    EXPECT_TRUE(gzEvent.Control());
  }

  // Move + Alt
  {
    QMouseEvent qtEvent(QEvent::MouseMove,
                        QPointF(123, 456), QPointF(123, 456),
                        Qt::LeftButton,
                        Qt::LeftButton,
                        Qt::AltModifier);

    auto gzEvent = convert(qtEvent);

    EXPECT_EQ(gzEvent.Type(), common::MouseEvent::MOVE);
    EXPECT_EQ(gzEvent.Pos(), math::Vector2i(123, 456));
    EXPECT_EQ(gzEvent.Button(), common::MouseEvent::LEFT);
    EXPECT_EQ(gzEvent.Buttons(), common::MouseEvent::LEFT);
    EXPECT_TRUE(gzEvent.Dragging());
    EXPECT_TRUE(gzEvent.Alt());
  }

  // Scroll
  {
    QWheelEvent qtEvent(QPointF(123, 456), QPointF(1000, 2000), QPoint(2, 3),
        QPoint(1, 4), Qt::MiddleButton, Qt::ShiftModifier, Qt::ScrollUpdate,
        false);

    auto gzEvent = convert(qtEvent);

    EXPECT_EQ(gzEvent.Type(), common::MouseEvent::SCROLL);
    EXPECT_EQ(gzEvent.Pos(), math::Vector2i(123, 456));
    EXPECT_EQ(gzEvent.Scroll(), math::Vector2i(-1, -1));
    EXPECT_EQ(gzEvent.Buttons(), common::MouseEvent::MIDDLE);
    EXPECT_FALSE(gzEvent.Dragging());
    EXPECT_TRUE(gzEvent.Shift());
  }
}

/////////////////////////////////////////////////
TEST(ConversionsTest, KeyEvent)
{
  {
    QKeyEvent qtEvent(QEvent::KeyPress, Qt::Key_Escape,
        Qt::ShiftModifier | Qt::ControlModifier);
    auto gzEvent = convert(qtEvent);

    EXPECT_EQ(gzEvent.Type(), common::KeyEvent::PRESS);
    EXPECT_EQ(gzEvent.Key(), Qt::Key_Escape);
    EXPECT_TRUE(gzEvent.Control());
    EXPECT_TRUE(gzEvent.Shift());
    EXPECT_FALSE(gzEvent.Alt());
  }
  {
    QKeyEvent qtEvent(QEvent::KeyRelease, Qt::Key_Control, Qt::NoModifier);
    auto gzEvent = convert(qtEvent);

    EXPECT_EQ(gzEvent.Type(), common::KeyEvent::RELEASE);
    EXPECT_EQ(gzEvent.Key(), Qt::Key_Control);
    EXPECT_TRUE(gzEvent.Control());
    EXPECT_FALSE(gzEvent.Shift());
    EXPECT_FALSE(gzEvent.Alt());
  }
  {
    QKeyEvent qtEvent(QEvent::None, Qt::Key_R, Qt::AltModifier);
    auto gzEvent = convert(qtEvent);

    EXPECT_EQ(gzEvent.Type(), common::KeyEvent::NO_EVENT);
    EXPECT_EQ(gzEvent.Key(), Qt::Key_R);
    EXPECT_FALSE(gzEvent.Control());
    EXPECT_FALSE(gzEvent.Shift());
    EXPECT_TRUE(gzEvent.Alt());
  }
}
