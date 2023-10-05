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

#include <gz/common/KeyEvent.hh>
#include <gz/common/MouseEvent.hh>
#include <gz/math/Color.hh>

#include "gz/gui/Conversions.hh"

namespace gz::gui {

//////////////////////////////////////////////////
QColor convert(const gz::math::Color &_color)
{
  return QColor(_color.R()*255.0,
                _color.G()*255.0,
                _color.B()*255.0,
                _color.A()*255.0);
}

//////////////////////////////////////////////////
gz::math::Color convert(const QColor &_color)
{
  return gz::math::Color(_color.red() / 255.0,
                       _color.green() / 255.0,
                       _color.blue() / 255.0,
                       _color.alpha() / 255.0);
}

//////////////////////////////////////////////////
QPointF convert(const gz::math::Vector2d &_pt)
{
  return QPointF(_pt.X(), _pt.Y());
}

//////////////////////////////////////////////////
gz::math::Vector2d convert(const QPointF &_pt)
{
  return gz::math::Vector2d(_pt.x(), _pt.y());
}

//////////////////////////////////////////////////
QVector3D convert(const gz::math::Vector3d &_vec)
{
  return QVector3D(_vec.X(), _vec.Y(), _vec.Z());
}

//////////////////////////////////////////////////
gz::math::Vector3d convert(const QVector3D &_vec)
{
  return gz::math::Vector3d(_vec.x(), _vec.y(), _vec.z());
}

//////////////////////////////////////////////////
gz::common::MouseEvent convert(const QMouseEvent &_e)
{
  common::MouseEvent event;
  event.SetPos(_e.pos().x(), _e.pos().y());

  // Button
  if (_e.button() == Qt::LeftButton)
    event.SetButton(common::MouseEvent::LEFT);
  else if (_e.button() == Qt::RightButton)
    event.SetButton(common::MouseEvent::RIGHT);
  else if (_e.button() == Qt::MiddleButton)
    event.SetButton(common::MouseEvent::MIDDLE);

  // Buttons
  if (_e.buttons() & Qt::LeftButton)
    event.SetButtons(event.Buttons() | common::MouseEvent::LEFT);

  if (_e.buttons() & Qt::RightButton)
    event.SetButtons(event.Buttons() | common::MouseEvent::RIGHT);

  if (_e.buttons() & Qt::MiddleButton)
    event.SetButtons(event.Buttons() | common::MouseEvent::MIDDLE);

  // Type
  if (_e.type() == QEvent::MouseButtonPress)
    event.SetType(common::MouseEvent::PRESS);
  else if (_e.type() == QEvent::MouseButtonRelease)
    event.SetType(common::MouseEvent::RELEASE);
  else if (_e.type() == QEvent::MouseMove)
  {
    event.SetType(common::MouseEvent::MOVE);

    // Dragging
    if (_e.buttons() || _e.button())
      event.SetDragging(true);
  }

  // Modifiers
  if (_e.modifiers() & Qt::ShiftModifier)
    event.SetShift(true);

  if (_e.modifiers() & Qt::ControlModifier)
    event.SetControl(true);

  if (_e.modifiers() & Qt::AltModifier)
    event.SetAlt(true);

  return event;
}

//////////////////////////////////////////////////
gz::common::MouseEvent convert(const QWheelEvent &_e)
{
  common::MouseEvent event;

  event.SetType(common::MouseEvent::SCROLL);
#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
  event.SetPos(_e.x(), _e.y());
#else
  event.SetPos(_e.position().x(), _e.position().y());
#endif
  double scroll = (_e.angleDelta().y() > 0) ? -1.0 : 1.0;
  event.SetScroll(scroll, scroll);

  // Buttons
  if (_e.buttons() & Qt::LeftButton)
    event.SetButtons(event.Buttons() | common::MouseEvent::LEFT);

  if (_e.buttons() & Qt::RightButton)
    event.SetButtons(event.Buttons() | common::MouseEvent::RIGHT);

  if (_e.buttons() & Qt::MiddleButton)
    event.SetButtons(event.Buttons() | common::MouseEvent::MIDDLE);

  // Modifiers
  if (_e.modifiers() & Qt::ShiftModifier)
    event.SetShift(true);

  if (_e.modifiers() & Qt::ControlModifier)
    event.SetControl(true);

  if (_e.modifiers() & Qt::AltModifier)
    event.SetAlt(true);

  return event;
}

//////////////////////////////////////////////////
gz::common::KeyEvent convert(const QKeyEvent &_e)
{
  common::KeyEvent event;
  event.SetKey(_e.key());
  event.SetText(_e.text().toStdString());

  if (_e.type() == QEvent::KeyPress)
  {
    event.SetType(common::KeyEvent::PRESS);
  }
  else if (_e.type() == QEvent::KeyRelease)
  {
    event.SetType(common::KeyEvent::RELEASE);
  }
  else
  {
    event.SetType(common::KeyEvent::NO_EVENT);
  }

  event.SetControl(_e.modifiers() & Qt::ControlModifier);
  event.SetShift(_e.modifiers() & Qt::ShiftModifier);
  event.SetAlt(_e.modifiers() & Qt::AltModifier);

  return event;
}
}  // namespace gz::gui
