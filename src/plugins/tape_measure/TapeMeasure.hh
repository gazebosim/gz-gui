/*
 * Copyright (C) 2020 Open Source Robotics Foundation
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

#ifndef GZ_GUI_TAPEMEASURE_HH_
#define GZ_GUI_TAPEMEASURE_HH_

#include <gz/utils/ImplPtr.hh>
#include <memory>

#include <gz/gui/Plugin.hh>
#include <gz/math/Vector3.hh>
#include <gz/math/Color.hh>

namespace gz::gui::plugins
{
  /// \brief Provides buttons for the tape measure tool.
  class TapeMeasure : public gz::gui::Plugin
  {
    Q_OBJECT

    /// \brief Constructor
    public: TapeMeasure();

    /// \brief Destructor
    public: ~TapeMeasure() override;

    // Documentation inherited
    public: void LoadConfig(const tinyxml2::XMLElement *_pluginElem) override;

    /// \brief Deletes the marker with the provided id within the
    /// "tape_measure" namespace.
    /// \param[in] _id The id of the marker
    public: void DeleteMarker(int _id);

    /// \brief Resets all of the relevant data for this plugin.  Called when
    /// the user clicks the reset button and when the user starts a new
    /// measurement.
    public: void Reset();

    /// \brief Starts a new measurement.  Erases any previous measurement in
    /// progress or already made.
    public: void Measure();

    /// \brief Draws a point marker.  Called to display the start and end
    /// point of the tape measure.
    /// \param[in] _id The id of the marker
    /// \param[in] _point The x, y, z coordinates of where to place the marker
    /// \param[in] _color The rgba color to set the marker
    public: void DrawPoint(int _id,
                gz::math::Vector3d &_point,
                gz::math::Color &_color);

    /// \brief Draws a line marker.  Called to display the line between the
    /// start and end point of the tape measure.
    /// \param[in] _id The id of the marker
    /// \param[in] _startPoint The x, y, z coordinates of the line start point
    /// \param[in] _endPoint The x, y, z coordinates of the line end point
    /// \param[in] _color The rgba color to set the marker
    public: void DrawLine(int _id,
                gz::math::Vector3d &_startPoint,
                gz::math::Vector3d &_endPoint,
                gz::math::Color &_color);

    /// \brief Callback in Qt thread when the new measurement button is
    /// clicked.
    public slots: void OnMeasure();

    /// \brief Callback in Qt thread when the reset button is clicked.
    public slots: void OnReset();

    /// \brief Callback in Qt thread to get the distance to display in the
    /// gui window.
    /// \return The distance between the start and end point of the measurement
    public slots: double Distance();

    // Documentation inherited
    protected: bool eventFilter(QObject *_obj, QEvent *_event) override;

    /// \brief Signal fired when a new tape measure distance is set.
    signals: void newDistance();

    /// \internal
    /// \brief Pointer to private data.
    private: GZ_UTILS_UNIQUE_IMPL_PTR(dataPtr)
  };
}  // namespace gz::gui::plugins
#endif  // GZ_GUI_PLUGINS_TAPEMEASURE_HH_
