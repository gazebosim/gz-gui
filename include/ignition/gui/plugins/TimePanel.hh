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

#ifndef IGNITION_GUI_PLUGINS_GUIDIAGNOSTICS_HH_
#define IGNITION_GUI_PLUGINS_GUIDIAGNOSTICS_HH_

#include <memory>

#include <ignition/gui/qt.h>
#include <ignition/gui/Plugin.hh>
#include <ignition/transport.hh>
#include <ignition/msgs.hh>

namespace ignition
{
namespace gui
{
namespace plugins
{
  class TimePanelPrivate;

  /// \brief This plugin provides a time panel which may:
  /// * Display simulation time
  /// * Display real time
  /// * Have a play / pause button
  class TimePanel: public ignition::gui::Plugin
  {
    Q_OBJECT

    /// \brief Constructor
    public: TimePanel();

    /// \brief Destructor
    public: virtual ~TimePanel();

    // Documentation inherited
    public: void LoadConfig(const tinyxml2::XMLElement *_pluginElem);

    /// \brief Callback in main thread when diagnostics come in
    public slots: void ProcessMsg();

    /// \brief Callback in Qt thread when play button is clicked.
    public slots: void OnPlay();

    /// \brief Callback in Qt thread when pause button is clicked.
    public slots: void OnPause();

    /// \brief Notify that it's now playing.
    signals: void Playing();

    /// \brief Notify that it's now paused.
    signals: void Paused();

    /// \brief Update simulation time.
    signals: void SetSimTime(QString _time);

    /// \brief Update real time.
    signals: void SetRealTime(QString _time);

    /// \brief Subscriber callback when new world statistics are received
    private: void OnWorldStatsMsg(const ignition::msgs::WorldStatistics &_msg);

    // Private data
    private: std::unique_ptr<TimePanelPrivate> dataPtr;
  };
}
}
}

#endif
