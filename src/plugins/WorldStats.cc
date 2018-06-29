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

#include <ignition/common/Console.hh>
#include <ignition/common/PluginMacros.hh>
#include <ignition/common/Time.hh>

#include "ignition/gui/plugins/WorldStats.hh"

namespace ignition
{
namespace gui
{
namespace plugins
{
  class WorldStatsPrivate
  {
    /// \brief Message holding latest world statistics
    public: ignition::msgs::WorldStatistics msg;

    /// \brief Mutex to protect msg
    public: std::recursive_mutex mutex;

    /// \brief Communication node
    public: ignition::transport::Node node;

    public: QString realTimeFactor;
    public: QString simTime;
    public: QString realTime;
    public: QString iterations;
  };
}
}
}

using namespace ignition;
using namespace gui;
using namespace plugins;

/////////////////////////////////////////////////
WorldStats::WorldStats()
  : Plugin(), dataPtr(new WorldStatsPrivate)
{
}

/////////////////////////////////////////////////
WorldStats::~WorldStats()
{
}

/////////////////////////////////////////////////
void WorldStats::LoadConfig(const tinyxml2::XMLElement *_pluginElem)
{
  // Default name in case user didn't define one
  if (this->title.empty())
    this->title = "Time panel";

  // Create elements from configuration
  if (!_pluginElem)
  {
    ignerr << "Null plugin element." << std::endl;
    return;
  }

  // Subscribe
  std::string topic;
  if (auto topicElem = _pluginElem->FirstChildElement("topic"))
    topic = topicElem->GetText();

  if (topic.empty())
  {
    ignerr << "Must specify a topic to subscribe to world statistics."
           << std::endl;
    return;
  }

  // Subscribe to world_stats
  if (!this->dataPtr->node.Subscribe(topic, &WorldStats::OnWorldStatsMsg,
      this))
  {
    ignerr << "Failed to subscribe to [" << topic << "]" << std::endl;
    return;
  }

  // Sim time
  if (auto simTimeElem = _pluginElem->FirstChildElement("sim_time"))
  {
    auto hasSim = false;
    simTimeElem->QueryBoolText(&hasSim);

    // TODO
  }

  // Real time
  if (auto realTimeElem = _pluginElem->FirstChildElement("real_time"))
  {
    auto hasReal = false;
    realTimeElem->QueryBoolText(&hasReal);

    // TODO
  }

  // Real time factor
  if (auto realTimeFactorElem =
        _pluginElem->FirstChildElement("real_time_factor"))
  {
    auto hasRTF = false;
    realTimeFactorElem->QueryBoolText(&hasRTF);

    // TODO
  }
}

/////////////////////////////////////////////////
void WorldStats::ProcessMsg()
{
  std::lock_guard<std::recursive_mutex> lock(this->dataPtr->mutex);

  ignition::common::Time time;

  if (this->dataPtr->msg.has_sim_time())
  {
    time.sec = this->dataPtr->msg.sim_time().sec();
    time.nsec = this->dataPtr->msg.sim_time().nsec();

    this->SetSimTime(QString::fromStdString(time.FormattedString()));
  }

  if (this->dataPtr->msg.has_real_time())
  {
    time.sec = this->dataPtr->msg.real_time().sec();
    time.nsec = this->dataPtr->msg.real_time().nsec();

    this->SetRealTime(QString::fromStdString(time.FormattedString()));
  }

  {
    // RTF as a percentage.
    double rtf = this->dataPtr->msg.real_time_factor() * 100;
    this->SetRealTimeFactor(QString::number(rtf, 'f', 2) + " %");
  }

  {
    this->SetIterations(QString::number(this->dataPtr->msg.iterations()));
  }
}

/////////////////////////////////////////////////
void WorldStats::OnWorldStatsMsg(const ignition::msgs::WorldStatistics &_msg)
{
  std::lock_guard<std::recursive_mutex> lock(this->dataPtr->mutex);

  this->dataPtr->msg.CopyFrom(_msg);
  QMetaObject::invokeMethod(this, "ProcessMsg");
}

/////////////////////////////////////////////////
QString WorldStats::RealTimeFactor() const
{
  return this->dataPtr->realTimeFactor;
}

/////////////////////////////////////////////////
void WorldStats::SetRealTimeFactor(const QString &_realTimeFactor)
{
  this->dataPtr->realTimeFactor = _realTimeFactor;
  this->RealTimeFactorChanged();
}

/////////////////////////////////////////////////
QString WorldStats::SimTime() const
{
  return this->dataPtr->simTime;
}

/////////////////////////////////////////////////
void WorldStats::SetSimTime(const QString &_simTime)
{
  this->dataPtr->simTime = _simTime;
  this->SimTimeChanged();
}

/////////////////////////////////////////////////
QString WorldStats::RealTime() const
{
  return this->dataPtr->realTime;
}

/////////////////////////////////////////////////
void WorldStats::SetRealTime(const QString &_realTime)
{
  this->dataPtr->realTime = _realTime;
  this->RealTimeChanged();
}

/////////////////////////////////////////////////
QString WorldStats::Iterations() const
{
  return this->dataPtr->iterations;
}

/////////////////////////////////////////////////
void WorldStats::SetIterations(const QString &_iterations)
{
  this->dataPtr->iterations = _iterations;
  this->IterationsChanged();
}

// Register this plugin
IGN_COMMON_REGISTER_SINGLE_PLUGIN(ignition::gui::plugins::WorldStats,
                                  ignition::gui::Plugin)
