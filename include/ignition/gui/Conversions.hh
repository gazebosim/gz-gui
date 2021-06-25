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

#ifndef IGNITION_GUI_CONVERSIONS_HH_
#define IGNITION_GUI_CONVERSIONS_HH_

#ifdef _MSC_VER
#pragma warning(push, 0)
#endif
#include <ignition/msgs/time.pb.h>
#ifdef _MSC_VER
#pragma warning(pop)
#endif
#include <ignition/common/Time.hh>
#include <ignition/math/Color.hh>
#include <ignition/math/Vector2.hh>
#include <ignition/math/Vector3.hh>

#include "ignition/gui/qt.h"
#include "ignition/gui/Export.hh"

namespace ignition
{
  namespace common
  {
    class MouseEvent;
  }

  namespace gui
  {
    /// \brief Return the equivalent qt color
    /// \param[in] _color Ignition color to convert
    /// \return Qt color value
    IGNITION_GUI_VISIBLE
    QColor convert(const math::Color &_color);

    /// \brief Return the equivalent ignition color
    /// \param[in] _color Qt color to convert
    /// \return Ignition color value
    IGNITION_GUI_VISIBLE
    math::Color convert(const QColor &_color);

    /// \brief Return the equivalent QPointF.
    /// \param[in] _pt Ignition vector to convert.
    /// \return QPointF.
    IGNITION_GUI_VISIBLE
    QPointF convert(const math::Vector2d &_pt);

    /// \brief Return the equivalent ignition vector.
    /// \param[in] _pt QPointF to convert
    /// \return Ignition Vector2d.
    IGNITION_GUI_VISIBLE
    math::Vector2d convert(const QPointF &_pt);

    /// \brief Return the equivalent qt vector 3d.
    /// \param[in] _vec Ignition vector 3d to convert.
    /// \return Qt vector 3d value.
    IGNITION_GUI_VISIBLE
    QVector3D convert(const math::Vector3d &_vec);

    /// \brief Return the equivalent ignition vector 3d.
    /// \param[in] _vec Qt vector 3d to convert.
    /// \return Ignition vector 3d value
    IGNITION_GUI_VISIBLE
    math::Vector3d convert(const QVector3D &_vec);

    /// \brief Return the equivalent ignition mouse event.
    ///
    /// Note that there isn't a 1-1 mapping between these types, so fields such
    /// as common::MouseEvent::PressPos need to be set afterwards.
    /// \param[in] _e Qt mouse event
    /// \return Ignition mouse event
    IGNITION_GUI_VISIBLE
    common::MouseEvent convert(const QMouseEvent &_e);
  }
}
#endif
