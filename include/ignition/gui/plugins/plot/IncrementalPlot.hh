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
#ifndef IGNITION_GUI_PLUGINS_PLOT_INCREMENTALPLOT_HH_
#define IGNITION_GUI_PLUGINS_PLOT_INCREMENTALPLOT_HH_

#include <memory>
#include <string>
#include <vector>

#include <ignition/math/Vector2.hh>

#include "ignition/gui/qt.h"
#include "ignition/gui/qwt.h"
#include "ignition/gui/plugins/plot/Types.hh"
#include "ignition/gui/System.hh"

namespace ignition
{
namespace common
{
  class Time;
}

namespace gui
{
namespace plugins
{
namespace plot
{
  // Forward declare private data class.
  class IncrementalPlotPrivate;

  /// \brief A plotting widget that handles incremental addition of data.
  class IGNITION_GUI_VISIBLE IncrementalPlot : public QwtPlot
  {
    Q_OBJECT

    /// \brief Axis enum.
    public: enum PlotAxis
    {
      /// \brief bottom x axis.
      X_BOTTOM_AXIS = 0,

      /// \brief top x axis.
      X_TOP_AXIS = 1,

      /// \brief left y axis.
      Y_LEFT_AXIS = 2,

      /// \brief right y axis.
      Y_RIGHT_AXIS = 3
    };

    /// \brief Constructor.
    /// \param[in] _parent Pointer to a parent widget.
    public: explicit IncrementalPlot(QWidget *_parent = nullptr);

    /// \brief Destructor.
    public: virtual ~IncrementalPlot();

    /// \brief Create a new curve and attach it to this plot.
    /// \param[in] _label Name of the curve.
    /// \return A pointer to the new curve.
    /// \sa AttachCurve
    /// \sa RemoveCurve
    /// \sa DetachCurve
    public: CurveWeakPtr AddCurve(const std::string &_label);

    /// \brief Add a new point to a curve.
    /// \param[in] _id Unique id of the curve.
    /// \param[in] _pt Point to add.
    public: void AddPoint(const unsigned int _id,
                          const math::Vector2d &_pt);

    /// \brief Add new points to a curve.
    /// \param[in] _id Unique id of the curve.
    /// \param[in] _pts Points to add.
    public: void AddPoints(const unsigned int _id,
                           const std::vector<math::Vector2d> &_pts);

    /// \brief Clear all points from the plot.
    public: void Clear();

    /// \brief Find a plot curve by name
    /// \param[in] _label Name of the curve to look for.
    /// \return Plot curve if found, nullptr otherwise.
    public: CurveWeakPtr Curve(const std::string &_label) const;

    /// \brief Find a plot curve by id.
    /// \param[in] _id Unique id of the plot curve.
    /// \return Plot curve if found, nullptr otherwise.
    public: CurveWeakPtr Curve(const unsigned int _id) const;

    /// \brief Update all the curves in the plot.
    public: void Update();

    /// \brief Remove a curve by id, this renders the curve unusable by other
    /// plots.
    /// \param[in] _id Unique id of the curve.
    /// \sa AddCurve
    /// \sa AttachCurve
    /// \sa DetachCurve
    public: void RemoveCurve(const unsigned int _id);

    /// \brief Get the period over which to plot. The period refers to the X
    /// axis.
    /// \return Period duration in seconds.
    /// \sa SetPeriod
    public: common::Time Period() const;

    /// \brief Set the period over which to plot.
    /// \param[in] _time Period duration in seconds.
    /// \sa Period
    public: void SetPeriod(const common::Time &_time);

    /// \brief Attach an existing curve to this plot.
    /// \param[in] _curve The curve to attach to the plot.
    /// \sa AddCurve
    /// \sa DetachCurve
    /// \sa RemoveCurve
    public: void AttachCurve(CurveWeakPtr _curve);

    /// \brief Detach a curve from this plot. Unlike RemoveCurve, this doesn't
    /// delete the curve, so it can be reused.
    /// \param[in] _id Unique id of the plot curve to detach.
    /// \return Pointer to the plot curve
    /// \sa AttachCurve
    /// \sa AddCurve
    /// \sa RemoveCurve
    public: CurvePtr DetachCurve(const unsigned int _id);

    /// \brief Set a new label for the given curve.
    /// \param[in] _id Unique id of the plot curve.
    /// \param[in] _label New label to set the plot curve to.
    public: void SetCurveLabel(const unsigned int _id,
                               const std::string &_label);

    /// \brief Set whether to show the axis label.
    /// \param[in] _axis Plot axis: X_BOTTOM_AXIS or Y_LEFT_AXIS.
    /// \param[in] _label The label to show.
    public: void ShowAxisLabel(const PlotAxis _axis,
                               const std::string &_label);

    /// \brief Set whether to show the grid lines.
    /// \param[in] _show True to show grid lines.
    public: void ShowGrid(const bool _show);

    /// \brief Get whether the grid lines are shown.
    /// \return True if the grid lines are visible.
    public: bool IsShowGrid() const;

    /// \brief Set whether to show the hover line.
    /// \param[in] _show True to show hover line.
    public: void ShowHoverLine(const bool _show);

    /// \brief Get whether the hover line is shown.
    /// \return True if the hover line is visible.
    public: bool IsShowHoverLine() const;

    /// \brief Get all curves in this plot
    /// \return A list of curves in this plot.
    public: std::vector<CurveWeakPtr> Curves() const;

    /// \brief Give QT a size hint.
    /// \return Default size of the plot.
    public: virtual QSize sizeHint() const override;

    /// \brief Used to accept drag enter events.
    /// \param[in] _evt The drag event.
    protected: void dragEnterEvent(QDragEnterEvent *_evt) override;

    /// \brief Used to accept drop events.
    /// \param[in] _evt The drop event.
    protected: void dropEvent(QDropEvent *_evt) override;

    /// \brief Qt signal emitted when a variable pill is added
    /// \param[in] _name Name of variable pill added.
    signals: void VariableAdded(const std::string &_name);

    /// \internal
    /// \brief Private data pointer
    private: std::unique_ptr<IncrementalPlotPrivate> dataPtr;
  };
}
}
}
}
#endif
