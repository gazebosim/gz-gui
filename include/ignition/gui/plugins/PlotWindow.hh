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
#ifndef IGNITION_GUI_PLUGINS_PLOTWINDOW_HH_
#define IGNITION_GUI_PLUGINS_PLOTWINDOW_HH_

#ifndef Q_MOC_RUN
  #include <ignition/gui/qt.h>
#endif

#include <list>
#include <memory>

#include "ignition/gui/Plugin.hh"

namespace ignition
{
namespace gui
{
namespace plugins
{
  // Forward declare private data class.
  class PlotWindowPrivate;

  /// \brief ToDo.
  class PlotWindow : public Plugin
  {
    Q_OBJECT

    /// \brief Constructor.
    public: PlotWindow();

    /// \brief Destructor.
    public: virtual ~PlotWindow();

    // Documentation inherited
    public: virtual void LoadConfig(const tinyxml2::XMLElement *_pluginElem);

    /*

    /// \brief Add a new canvas.
    public: PlotCanvas *AddCanvas();

    /// \brief Get a list of all the plots
    /// \return A list of all the plots.
    public: std::list<PlotCanvas *> Plots();

    /// \brief Remove a plot canvas
    /// \param[in] _canvas Canvas to remove
    public: void RemoveCanvas(PlotCanvas *_canvas);

    /// \brief Get the number of canvases in this plot window.
    /// \return Number of canvases
    public: unsigned int CanvasCount() const;

    /// \brief Clear and remove all canvases
    public: void Clear();

    /// \brief Restart plotting. A new plot curve will be created for each
    /// variable in the plot. Existing plot curves will no longer be updated.
    public: void Restart();

    /// \brief Update a canvas. This currently just enables/disables the
    /// delete canvas setting option based on the number of canvases in the
    /// window.
    private: void UpdateCanvas();

    /// \brief Update all canvases
    private slots: void Update();

    /// \brief QT callback for when a plot is to be exported.
    private slots: void OnExport();

    /// \brief Qt Callback when a new plot canvas should be added.
    private slots: void OnAddCanvas();

    /// \brief Qt Callback when a plot canvas should be removed.
    private slots: void OnRemoveCanvas();

    /// \brief Toggle simulation play/pause state.
    private slots: void TogglePause();

    */

    /// \internal
    /// \brief Pointer to private data.
    private: std::unique_ptr<PlotWindowPrivate> dataPtr;
  };
}
}
}
#endif