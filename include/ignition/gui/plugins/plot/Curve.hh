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

#ifndef IGNITION_GUI_PLUGINS_PLOT_CURVE_HH_
#define IGNITION_GUI_PLUGINS_PLOT_CURVE_HH_

#include <memory>
#include <string>
#include <vector>

#include <ignition/math/Vector2.hh>

#include "ignition/gui/qt.h"
#include "ignition/gui/Export.hh"

class QwtPlotCurve;

namespace ignition
{
namespace gui
{
namespace plugins
{
namespace plot
{
  // Forward declare private data class.
  class CurvePrivate;
  class IncrementalPlot;

  /// \brief Plot Curve data.
  class IGNITION_GUI_VISIBLE Curve
  {
    /// \brief Constructor.
    /// \param[in] _label Plot label.
    public: explicit Curve(const std::string &_label);

    /// \brief Destructor.
    public: ~Curve();

    /// \brief Add a point to the curve.
    /// \param[in] _pt Point to add.
    /// \sa AddPoints
    /// \sa Point
    /// \sa Points
    public: void AddPoint(const math::Vector2d &_pt);

    /// \brief Add points to the curve.
    /// \param[in] _pts Points to add.
    /// \sa AddPoint
    /// \sa Point
    /// \sa Points
    public: void AddPoints(const std::vector<math::Vector2d> &_pts);

    /// \brief Clear all data from the curve.
    public: void Clear();

    /// \brief Attach the curve to a plot.
    /// \param[in] _plot Plot to attach to.
    /// \sa Detach
    public: void Attach(IncrementalPlot *_plot);

    /// \brief Detach the curve from the plot.
    /// \sa Attach
    public: void Detach();

    /// \brief Set the curve label.
    /// \param[in] _label Label to set the curve to.
    /// \sa Label
    public: void SetLabel(const std::string &_label);

    /// \brief Get the curve label.
    /// \return Curve label.
    /// \sa SetLabel
    public: std::string Label() const;

    /// \brief Set the curve id.
    /// \param[in] _id Id to set the curve to.
    /// \sa Id
    public: void SetId(const unsigned int _id);

    /// \brief Get the curve id.
    /// \return Curve id.
    /// \sa SetId
    public: unsigned int Id() const;

    /// \brief Set whether the curve should be active.
    /// \param[in] _active True to make the curve active and accept new points.
    /// \sa Active
    public: void SetActive(const bool _active);

    /// \brief Get whether the curve is active.
    /// \return Active state of the plot curve.
    /// \sa SetActive
    public: bool Active() const;

    /// \brief Set the age of the curve.
    /// \param[in] _age Age of the curve. Currently used to keep track of
    /// how many restarts it has been through.
    /// \sa Age
    public: void SetAge(const unsigned int _age);

    /// \brief Get the age of the curve.
    /// \return Curve age.
    /// \sa SetAge
    public: unsigned int Age() const;

    /// \brief Get the number of data points in the curve.
    /// \return Number of data points.
    public: unsigned int Size() const;

    /// \brief Get the min x and y values of this curve.
    /// \return Point with min values
    /// \sa Max
    public: math::Vector2d Min() const;

    /// \brief Get the max x and y values of this curve.
    /// \return Point with max values
    /// \sa Min
    public: math::Vector2d Max() const;

    /// \brief Get a point in the curve.
    /// \param[in] _index Index of the point in the curve.
    /// \return 2d point at the specified index. A Vector2d of nans is
    /// returned if the index is out of bounds.
    /// \sa AddPoint
    /// \sa AddPoints
    /// \sa Points
    public: math::Vector2d Point(const unsigned int _index) const;

    /// \brief Return all the sample points in the curve.
    /// \return Curve sample points.
    /// \sa AddPoint
    /// \sa AddPoints
    /// \sa Point
    public: std::vector<math::Vector2d> Points() const;

    /// \internal
    /// \brief Get the internal QwtPlotCurve object.
    /// \return QwtPlotCurve object.
    public: QwtPlotCurve *QwtCurve();

    /// \internal
    /// \brief Private data pointer.
    private: std::unique_ptr<CurvePrivate> dataPtr;
  };
}
}
}
}
#endif
