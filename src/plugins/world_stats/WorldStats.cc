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
#include <ignition/plugin/Register.hh>
#include <ignition/common/Time.hh>

#include "WorldStats.hh"

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

    /// \brief Holds real time factor
    public: QString realTimeFactor;

    /// \brief Holds sim time
    public: QString simTime;

    /// \brief Holds real time
    public: QString realTime;

    /// \brief Holds iterations
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
    this->title = "World stats";

  // Create elements from configuration
  if (!_pluginElem)
  {
    ignerr << "Null plugin element." << std::endl;
    return;
  }

  // Subscribe
  std::string topic;
  auto topicElem = _pluginElem->FirstChildElement("topic");
  if (nullptr != topicElem && nullptr != topicElem->GetText())
    topic = topicElem->GetText();

  if (topic.empty())
  {
    ignerr << "Must specify a topic to subscribe to world statistics."
           << std::endl;
    return;
  }

  if (!this->dataPtr->node.Subscribe(topic, &WorldStats::OnWorldStatsMsg,
      this))
  {
    ignerr << "Failed to subscribe to [" << topic << "]" << std::endl;
    return;
  }

  // Sim time
  if (auto simTimeElem = _pluginElem->FirstChildElement("sim_time"))
  {
    auto has = false;
    simTimeElem->QueryBoolText(&has);
    this->PluginItem()->setProperty("showSimTime", has);

    this->SetSimTime("N/A");
  }

  // Real time
  if (auto realTimeElem = _pluginElem->FirstChildElement("real_time"))
  {
    auto has = false;
    realTimeElem->QueryBoolText(&has);
    this->PluginItem()->setProperty("showRealTime", has);

    this->SetRealTime("N/A");
  }

  // Real time factor
  if (auto realTimeFactorElem =
        _pluginElem->FirstChildElement("real_time_factor"))
  {
    auto has = false;
    realTimeFactorElem->QueryBoolText(&has);
    this->PluginItem()->setProperty("showRealTimeFactor", has);

    this->SetRealTimeFactor("N/A");
  }

  // Iterations
  if (auto iterationsElem =
        _pluginElem->FirstChildElement("iterations"))
  {
    auto has = false;
    iterationsElem->QueryBoolText(&has);
    this->PluginItem()->setProperty("showIterations", has);

    this->SetIterations("N/A");
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
IGNITION_ADD_PLUGIN(ignition::gui::plugins::WorldStats,
                    ignition::gui::Plugin)
