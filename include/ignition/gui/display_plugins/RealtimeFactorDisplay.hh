/*
 * Copyright (C) 2018 Open Source Robotics Foundation
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

#ifndef IGNITION_GUI_DISPLAY_PLUGINS_REALTIMEFACTORDISPLAY_HH_
#define IGNITION_GUI_DISPLAY_PLUGINS_REALTIMEFACTORDISPLAY_HH_

#include <ignition/msgs/world_stats.pb.h>

#include <memory>
#include <string>

#include "ignition/gui/DisplayPlugin.hh"
#include "ignition/gui/qt.h"

namespace ignition
{
namespace gui
{
namespace display_plugins
{
  class RealtimeFactorDisplayPrivate;

  /// \brief Display the realtime factor on an Ignition Rendering scene.
  class RealtimeFactorDisplay : public DisplayPlugin
  {
    Q_OBJECT

    /// \brief Constructor
    public: RealtimeFactorDisplay();

    /// \brief Destructor
    public: virtual ~RealtimeFactorDisplay();

    // Documentation inherited
    public: virtual void Initialize(const tinyxml2::XMLElement *_pluginElem)
        override;

    // Documentation inherited
    public: virtual std::string Type() const override;

    /// \brief Callback in main thread when diagnostics come in
    public slots: void ProcessMsg();

    /// \brief Subscriber callback when new world statistics are received
    private: void OnWorldStatsMsg(const ignition::msgs::WorldStatistics &_msg);

    /// \internal
    /// \brief Pointer to private data.
    private: std::unique_ptr<RealtimeFactorDisplayPrivate> dataPtr;
  };
}
}
}

#endif
