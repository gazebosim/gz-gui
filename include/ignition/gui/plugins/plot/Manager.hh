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
#ifndef IGNITION_GUI_PLUGINS_PLOT_MANAGER_HH_
#define IGNITION_GUI_PLUGINS_PLOT_MANAGER_HH_

#include <memory>
#include <string>
#include <ignition/msgs.hh>
#include <ignition/common/SingletonT.hh>

#include "ignition/gui/plugins/plot/Types.hh"

namespace ignition
{
namespace gui
{
namespace plugins
{
namespace plot
{
  // Forward declarations.
  class ManagerPrivate;
  class Plot;

  /// \brief A class that connects simulation data with the plotting tool
  class Manager : public ignition::common::SingletonT<Manager>
  {
    /// \brief Constructor.
    public: Manager();

    /// \brief Destructor.
    public: virtual ~Manager();

    /// \brief Add a topic curve to the manager. Data received from
    /// the topic subscriber will be added to the curve
    /// \param[in] _topic Name of topic
    /// \param[in] _curve Curve that will be populated with data.
    public: void AddTopicCurve(const std::string &_topic,
                               CurveWeakPtr _curve);

    /// \brief Remove a topic curve from the manager
    /// \param[in] _curve Curve to remove.
    public: void RemoveTopicCurve(CurveWeakPtr _curve);

    /// \brief Get Human-readable name from uri-formatted variable name
    /// \param[in] _uri URI representing the variable
    /// \return Human readable name
    public: std::string HumanReadableName(const std::string &_uri) const;

    /// \brief This is a singleton class.
    private: friend class SingletonT<Manager>;

    /// \internal
    /// \brief Pointer to private data.
    private: std::unique_ptr<ManagerPrivate> dataPtr;
  };
}
}
}
}
#endif
