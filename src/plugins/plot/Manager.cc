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

#include <memory>
#include <mutex>
#include <vector>

#include <ignition/msgs.hh>
#include <ignition/common/URI.hh>
#include <ignition/common/Util.hh>

#include "ignition/gui/plugins/plot/Curve.hh"
#include "ignition/gui/plugins/plot/IncrementalPlot.hh"
#include "ignition/gui/plugins/plot/Plot.hh"
#include "ignition/gui/plugins/plot/Manager.hh"
#include "ignition/gui/plugins/plot/TopicCurveHandler.hh"

using namespace ignition;
using namespace gui;
using namespace plugins;
using namespace plot;

namespace ignition
{
namespace gui
{
namespace plugins
{
namespace plot
{
  /// \brief Private data for the Manager class
  class ManagerPrivate
  {
    /// \brief Mutex to protect plot manager updates.
    public: std::mutex mutex;

    /// \brief Handler for updating topic curves
    public: TopicCurveHandler topicCurve;
  };
}
}
}
}

/////////////////////////////////////////////////
Manager::Manager()
  : dataPtr(new ManagerPrivate())
{
}

/////////////////////////////////////////////////
Manager::~Manager()
{
}

/////////////////////////////////////////////////
void Manager::AddTopicCurve(const std::string &_topic, CurveWeakPtr _curve)
{
  this->dataPtr->topicCurve.AddCurve(_topic, _curve);
}

/////////////////////////////////////////////////
void Manager::RemoveTopicCurve(CurveWeakPtr _curve)
{
  this->dataPtr->topicCurve.RemoveCurve(_curve);
}
