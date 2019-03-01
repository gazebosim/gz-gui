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

#ifndef IGNITION_GUI_PLUGINS_WORLDCONTROL_HH_
#define IGNITION_GUI_PLUGINS_WORLDCONTROL_HH_

#include <memory>

#include <ignition/msgs.hh>
#include <ignition/transport.hh>

#include "ignition/gui/Plugin.hh"

#ifndef _WIN32
#  define WorldControl_EXPORTS_API
#else
#  if (defined(WorldControl_EXPORTS))
#    define WorldControl_EXPORTS_API __declspec(dllexport)
#  else
#    define WorldControl_EXPORTS_API __declspec(dllimport)
#  endif
#endif

namespace ignition
{
namespace gui
{
namespace plugins
{
  class WorldControlPrivate;

  /// \brief This plugin provides a world control panel which may have a
  /// play / pause and step buttons.
  ///
  /// ## Configuration
  ///
  /// * \<play_pause\> : Set to true to see a play/pause and step buttons,
  ///                    false by default.
  /// * \<start_paused\> : Set to false to start playing, false by default.
  /// * \<service\> : Service for world control, required.
  /// * \<stats_topic\> : Topic to receive world statistics, optional.
  class WorldControl_EXPORTS_API WorldControl: public ignition::gui::Plugin
  {
    Q_OBJECT

    /// \brief Constructor
    public: WorldControl();

    /// \brief Destructor
    public: virtual ~WorldControl();

    // Documentation inherited
    public: void LoadConfig(const tinyxml2::XMLElement *_pluginElem);

    /// \brief Callback in main thread when diagnostics come in
    public slots: void ProcessMsg();

    /// \brief Callback in Qt thread when play button is clicked.
    public slots: void OnPlay();

    /// \brief Callback in Qt thread when pause button is clicked.
    public slots: void OnPause();

    /// \brief Callback in Qt thread when step button is clicked.
    public slots: void OnStep();

    /// \brief Callback in Qt thread when step count is changed.
    /// \param[in] _steps New number of steps.
    public slots: void OnStepCount(const unsigned int _steps);

    /// \brief Notify that it's now playing.
    signals: void playing();

    /// \brief Notify that it's now paused.
    signals: void paused();

    /// \brief Subscriber callback when new world statistics are received
    private: void OnWorldStatsMsg(const ignition::msgs::WorldStatistics &_msg);

    // Private data
    private: std::unique_ptr<WorldControlPrivate> dataPtr;
  };
}
}
}

#endif
