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

#ifndef GZ_GUI_CONVERSIONS_HH_
#define GZ_GUI_CONVERSIONS_HH_

#include <gz/common/KeyEvent.hh>
#include <gz/common/MouseEvent.hh>
#include <gz/math/Color.hh>
#include <gz/math/Vector2.hh>
#include <gz/math/Vector3.hh>

#include "gz/gui/qt.h"
#include "gz/gui/Export.hh"

namespace gz::common
{
class MouseEvent;
}  // namespace gz::common

namespace gz::gui
{
/// \brief Return the equivalent Qt color
/// \param[in] _color Gazebo color to convert
/// \return Qt color value
GZ_GUI_VISIBLE
QColor convert(const math::Color &_color);

/// \brief Return the equivalent Gazebo color
/// \param[in] _color Qt color to convert
/// \return Gazebo color value
GZ_GUI_VISIBLE
math::Color convert(const QColor &_color);

/// \brief Return the equivalent QPointF.
/// \param[in] _pt Gazebo vector to convert.
/// \return QPointF.
GZ_GUI_VISIBLE
QPointF convert(const math::Vector2d &_pt);

/// \brief Return the equivalent Gazebo vector.
/// \param[in] _pt QPointF to convert
/// \return Gazebo Vector2d.
GZ_GUI_VISIBLE
math::Vector2d convert(const QPointF &_pt);

/// \brief Return the equivalent Qt vector 3d.
/// \param[in] _vec Gazebo vector 3d to convert.
/// \return Qt vector 3d value.
GZ_GUI_VISIBLE
QVector3D convert(const math::Vector3d &_vec);

/// \brief Return the equivalent Gazebo vector 3d.
/// \param[in] _vec Qt vector 3d to convert.
/// \return Gazebo vector 3d value
GZ_GUI_VISIBLE
math::Vector3d convert(const QVector3D &_vec);

/// \brief Return the equivalent Gazebo mouse event.
///
/// Note that there isn't a 1-1 mapping between these types, so fields such
/// as common::MouseEvent::PressPos need to be set afterwards.
/// \param[in] _e Qt mouse event
/// \return Gazebo mouse event
GZ_GUI_VISIBLE
common::MouseEvent convert(const QMouseEvent &_e);

/// \brief Return the equivalent Gazebo mouse event.
///
/// Note that there isn't a 1-1 mapping between these types.
/// \param[in] _e Qt wheel event
/// \return Gazebo mouse event
GZ_GUI_VISIBLE
common::MouseEvent convert(const QWheelEvent &_e);

/// \brief Return the equivalent Gazebo key event.
///
/// \param[in] _e Qt key event
/// \return Gazebo key event
GZ_GUI_VISIBLE
common::KeyEvent convert(const QKeyEvent &_e);
}  // namespace gz::gui
#endif  // GZ_GUI_CONVERSIONS_HH_
