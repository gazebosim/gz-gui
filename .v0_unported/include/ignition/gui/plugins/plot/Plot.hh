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
#ifndef IGNITION_GUI_PLUGINS_PLOT_PLOT_HH_
#define IGNITION_GUI_PLUGINS_PLOT_PLOT_HH_

#include <memory>
#include <string>
#include <vector>

#include "ignition/gui/Enums.hh"
#include "ignition/gui/Plugin.hh"
#include "ignition/gui/Export.hh"

namespace ignition
{
namespace gui
{
namespace plugins
{
namespace plot
{
  // Forward declarations.
  class IncrementalPlot;
  class PlotPrivate;

  /// \brief This plugin allows plotting Ignition Transport topics.
  /// Topics can be dragged from other widgets, such as `TopicViewer`
  /// and dropped onto a variable pill container or a plot. There is support
  /// for multiple plots in parallel with different Y axes, while sharing the
  /// same X axis. The X axis is currently always based on wall time.
  ///
  /// Through the settings menu, plots can be cleared, configured or exported
  /// to CSV or PDF files,
  ///
  /// ## Configuration
  /// This plugin doesn't accept any custom configuration.
  ///
  class IGNITION_GUI_VISIBLE Plot : public Plugin
  {
    Q_OBJECT

    /// \brief Constructor.
    public: Plot();

    /// \brief Destructor.
    public: virtual ~Plot();

    // Documentation inherited.
    public: virtual void LoadConfig(const tinyxml2::XMLElement *_pluginElem)
        override;

    // Documentation inherited
    protected slots: void ShowContextMenu(const QPoint &_pos) override;

    /// \brief Qt Callback when a new variable has been dropped into a plot.
    /// \param[in] _variable Name of the variable
    private slots: void OnAddVariableFromPlot(const std::string &_variable);

    /// \brief Qt Callback when a new variable has been dropped into a variable
    /// container.
    /// \param[in] _id Unique id of the variable
    /// \param[in] _variable Name of the variable
    /// \param[in] _colocatedId Unique id of the target variable that the
    /// variable is now co-located with.
    private slots: void OnAddVariableFromPill(const unsigned int _id,
                                              const std::string &_variable,
                                              const unsigned int _colocatedId);

    /// \brief Qt Callback when a variable has been removed.
    /// \param[in] _id Unique id of the variable
    /// \param[in] _colocatedId Unique id of the target variable that the
    /// variable was co-located with.
    private slots: void OnRemoveVariableFromPill(const unsigned int _id,
        const unsigned int _colocatedId);

    /// \brief Qt Callback when a variable has moved from one pill to another.
    /// \param[in] _id Unique id of the variable that has moved.
    /// \param[in] _targetId Unique id of the target variable that the
    /// moved variable is now co-located with.
    private slots: void OnMoveVariableFromPill(const unsigned int _id,
                                               const unsigned int _targetId);

    /// \brief Qt Callback to clear all variable and plots on canvas.
    private slots: void OnClear();

    /// \brief Qt Callback to show/hide grids on plot.
    /// \param[in] _show True to show.
    private slots: void OnShowGrid(const bool _show);

    /// \brief Qt Callback to show/hide hover line on plot.
    /// \param[in] _show True to show.
    private slots: void OnShowHoverLine(const bool _show);

    /// \brief Export to a PDF file.
    private slots: void OnExportPDF();

    /// \brief Export to a CSV file.
    private slots: void OnExportCSV();

    /// \brief Update all plots; this is called periodically by a timer.
    private slots: void Update();

    /// \brief Add a new variable to a plot.
    /// \param[in] _variable Name of the variable.
    /// \param[in] _plotId Unique id of the plot to add the variable to,
    /// defaults to the empty plot.
    /// \return Unique id of the variable
    private: unsigned int AddVariableToPill(const std::string &_variable,
        const unsigned int _plotId = EmptyPlot);

    /// \brief Add a variable to a plot. Note this function
    /// only updates the plot but not the variable pill container.
    /// \param[in] _id Unique id of the variable
    /// \param[in] _variable Name of the variable
    /// \param[in] _plotId Unique id of the plot to add the variable to.
    /// EmptyPlot means add to a new plot.
    private: void AddVariableToPlot(const unsigned int _id,
                                    const std::string &_variable,
                                    const unsigned int _plotId = EmptyPlot);

    /// \brief Remove a variable from a plot.
    /// \param[in] _id Unique id of the variable
    /// \param[in] _plotId Unique id of plot to remove the variable from.
    /// If EmptyPlot is specified, the function will search through all
    /// plots for the variable and remove it from a plot if found.
    private: void RemoveVariable(const unsigned int _id,
                                 const unsigned int _plotId = EmptyPlot);

    /// \brief Add a new empty plot to the canvas.
    /// \return Unique id of the plot
    private: unsigned int AddPlot();

    /// \brief Remove a plot from the canvas.
    /// \param[in] _id Unique id of the plot
    /// \return True if successfully removed.
    private: bool RemovePlot(const unsigned int _plotId);

    /// \brief
    /// \param[in]
    private: void ShowEmptyPlot(const bool _show);

    /// \brief Get all the plots in this canvas.
    /// \return A list of plots in this canvas.
    private: std::vector<IncrementalPlot *> Plots() const;

    /// \brief Clear the canvas and remove all variables and plots.
    private: void Clear();

    /// \brief Update the axis label for plots in the canvas.
    /// Currently used to determine which plot will display the x-axis label
    /// when plots are added/removed
    private: void UpdateAxisLabel();

    /// \brief Used to filter scroll wheel events.
    /// \param[in] _o Object that receives the event.
    /// \param[in] _event Pointer to the event.
    /// \return True if event was handled.
    private: virtual bool eventFilter(QObject *_o, QEvent *_event) override;

    /// \brief Prompts the user to choose a path to export plots to. This is
    /// independent of file type.
    /// \return File path up to the name, without an extension.
    private: std::string ExportFilename();

    /// \brief Empty plot used to indicate non-existent plot.
    private: static const unsigned int EmptyPlot;

    // Private data
    private: std::unique_ptr<PlotPrivate> dataPtr;
  };
}
}
}
}
#endif
