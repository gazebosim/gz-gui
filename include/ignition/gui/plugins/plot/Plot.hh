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

#include <list>
#include <memory>

#include "ignition/gui/Plugin.hh"
#include "ignition/gui/System.hh"

namespace ignition
{
namespace gui
{
namespace plugins
{
namespace plot
{
  // Forward declarations.
  class Canvas;
  class PlotPrivate;

  /// \brief Widget that allow plotting of Ignition Transport topics.
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

    /// \brief Add a new canvas.
    public: Canvas *AddCanvas();

    /// \brief Get a list of all the plots
    /// \return A list of all the plots.
    public: std::list<Canvas *> Plots();

    /// \brief Remove a plot canvas
    /// \param[in] _canvas Canvas to remove
    public: void RemoveCanvas(Canvas *_canvas);

    /// \brief Get the number of canvases in this plot window.
    /// \return Number of canvases
    public: unsigned int CanvasCount() const;

    /// \brief Clear and remove all canvases
    public: void Clear();

    /// \brief Restart plotting. A new plot curve will be created for each
    /// variable in the plot. Existing plot curves will no longer be updated.
    public: void Restart();

    // Documentation inherited
    protected slots: void ShowContextMenu(const QPoint &_pos) override;

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

    // Private data
    private: std::unique_ptr<PlotPrivate> dataPtr;
  };
}
}
}
}
#endif
