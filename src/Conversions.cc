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

#include <ignition/math/Color.hh>

#include "ignition/gui/Conversions.hh"

//////////////////////////////////////////////////
QColor ignition::gui::convert(const ignition::math::Color &_color)
{
  return QColor(_color.R()*255.0,
                _color.G()*255.0,
                _color.B()*255.0,
                _color.A()*255.0);
}

//////////////////////////////////////////////////
ignition::math::Color ignition::gui::convert(const QColor &_color)
{
  return ignition::math::Color(_color.red() / 255.0,
                       _color.green() / 255.0,
                       _color.blue() / 255.0,
                       _color.alpha() / 255.0);
}

//////////////////////////////////////////////////
QPointF ignition::gui::convert(const ignition::math::Vector2d &_pt)
{
  return QPointF(_pt.X(), _pt.Y());
}

//////////////////////////////////////////////////
ignition::math::Vector2d ignition::gui::convert(const QPointF &_pt)
{
  return ignition::math::Vector2d(_pt.x(), _pt.y());
}

//////////////////////////////////////////////////
QVector3D ignition::gui::convert(const ignition::math::Vector3d &_vec)
{
  return QVector3D(_vec.X(), _vec.Y(), _vec.Z());
}

//////////////////////////////////////////////////
ignition::math::Vector3d ignition::gui::convert(const QVector3D &_vec)
{
  return ignition::math::Vector3d(_vec.x(), _vec.y(), _vec.z());
}

