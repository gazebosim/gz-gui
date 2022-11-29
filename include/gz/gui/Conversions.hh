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

#ifdef _MSC_VER
#pragma warning(push, 0)
#endif
#include <gz/msgs/time.pb.h>
#ifdef _MSC_VER
#pragma warning(pop)
#endif
#include <gz/common/Time.hh>
#include <gz/math/Color.hh>
#include <gz/math/Vector2.hh>
#include <gz/math/Vector3.hh>

#include "gz/gui/qt.h"
#include "gz/gui/Export.hh"

namespace ignition
{
  namespace common
  {
    class MouseEvent;
  }

  namespace gui
  {
    /// \brief Return the equivalent qt color
    /// \param[in] _color Gazebo color to convert
    /// \return Qt color value
    IGNITION_GUI_VISIBLE
    QColor convert(const math::Color &_color);

    /// \brief Return the equivalent gazebo color
    /// \param[in] _color Qt color to convert
    /// \return Gazebo color value
    IGNITION_GUI_VISIBLE
    math::Color convert(const QColor &_color);

    /// \brief Return the equivalent QPointF.
    /// \param[in] _pt Gazebo vector to convert.
    /// \return QPointF.
    IGNITION_GUI_VISIBLE
    QPointF convert(const math::Vector2d &_pt);

    /// \brief Return the equivalent gazebo vector.
    /// \param[in] _pt QPointF to convert
    /// \return Gazebo Vector2d.
    IGNITION_GUI_VISIBLE
    math::Vector2d convert(const QPointF &_pt);

    /// \brief Return the equivalent qt vector 3d.
    /// \param[in] _vec Gazebo vector 3d to convert.
    /// \return Qt vector 3d value.
    IGNITION_GUI_VISIBLE
    QVector3D convert(const math::Vector3d &_vec);

    /// \brief Return the equivalent gazebo vector 3d.
    /// \param[in] _vec Qt vector 3d to convert.
    /// \return Gazebo vector 3d value
    IGNITION_GUI_VISIBLE
    math::Vector3d convert(const QVector3D &_vec);

    /// \brief Return the equivalent gazebo mouse event.
    ///
    /// Note that there isn't a 1-1 mapping between these types, so fields such
    /// as common::MouseEvent::PressPos need to be set afterwards.
    /// \param[in] _e Qt mouse event
    /// \return Gazebo mouse event
    IGNITION_GUI_VISIBLE
    common::MouseEvent convert(const QMouseEvent &_e);

    /// \brief Convert an gz::msgs::Time to an gz::common::Time
    /// \param[in] _t The time to convert
    /// \return An gz::common::Time object
    IGNITION_GUI_VISIBLE
    common::Time convert(const msgs::Time &_t);

    /// \brief Convert an gz::common::Time to an gz::msgs::Time
    /// \param[in] _t The time to convert
    /// \return An gz::msgs::Time object
    IGNITION_GUI_VISIBLE
    msgs::Time convert(const common::Time &_t);
  }
}
#endif
