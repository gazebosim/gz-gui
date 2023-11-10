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

#include "WorldStats.hh"

#include <string>

#include <gz/msgs/world_stats.pb.h>

#include <gz/common/Console.hh>
#include <gz/common/StringUtils.hh>
#include <gz/math/Helpers.hh>
#include <gz/plugin/Register.hh>
#include <gz/transport/Node.hh>
#include <gz/utils/ImplPtr.hh>

#include "gz/gui/Helpers.hh"

namespace gz::gui::plugins
{
class WorldStats::Implementation
{
  /// \brief Message holding latest world statistics
  public: gz::msgs::WorldStatistics msg;

  /// \brief Mutex to protect msg
  public: std::recursive_mutex mutex;

  /// \brief Communication node
  public: gz::transport::Node node;

  /// \brief Holds real time factor
  public: QString realTimeFactor;

  /// \brief Holds sim time
  public: QString simTime;

  /// \brief Holds real time
  public: QString realTime;

  /// \brief Holds iterations
  public: QString iterations;

  /// \brief Time delayed version if simTime used for computing a low-pass
  /// filtered RTF
  public: std::optional<double> simTimeDelayed;

  /// \brief Time delayed version if realTime used for computing a low-pass
  /// filtered RTF
  public: std::optional<double> realTimeDelayed;
};

/////////////////////////////////////////////////
WorldStats::WorldStats()
  : dataPtr(gz::utils::MakeUniqueImpl<Implementation>())
{
}

/////////////////////////////////////////////////
WorldStats::~WorldStats() = default;

/////////////////////////////////////////////////
void WorldStats::LoadConfig(const tinyxml2::XMLElement *_pluginElem)
{
  // Default name in case user didn't define one
  if (this->title.empty())
    this->title = "World stats";

  // Create elements from configuration
  if (!_pluginElem)
  {
    gzerr << "Null plugin element." << std::endl;
    return;
  }

  // If no elements were set, show everything. We assume that the user never
  // wants to hide everything. This happens for example when the plugin is
  // inserted from the menu.
  if (_pluginElem->NoChildren())
  {
    this->PluginItem()->setProperty("showSimTime", true);
    this->PluginItem()->setProperty("showRealTime", true);
    this->PluginItem()->setProperty("showRealTimeFactor", true);
    this->PluginItem()->setProperty("showIterations", true);
  }

  // World name from window, to construct default topics and services
  std::string worldName;
  auto worldNames = gui::worldNames();
  if (!worldNames.empty())
    worldName = worldNames[0].toStdString();

  // Subscribe
  std::string topic;
  auto topicElem = _pluginElem->FirstChildElement("topic");
  if (nullptr != topicElem && nullptr != topicElem->GetText())
    topic = topicElem->GetText();

  // Service specified with different world name
  auto parts = common::Split(topic, '/');
  if (!worldName.empty() &&
      parts.size() == 4 &&
      parts[0] == "" &&
      parts[1] == "world" &&
      parts[2] != worldName &&
      parts[3] == "stats")
  {
    gzwarn << "Ignoring topic [" << topic
            << "], world name different from [" << worldName
            << "]. Fix or remove your <topic> tag." << std::endl;

    topic = "/world/" + worldName + "/stats";
  }

  if (topic.empty())
  {
    if (worldName.empty())
    {
      gzerr << "Must specify a <topic> to subscribe to world statistics, or "
             << "set the MainWindow's [worldNames] property." << std::endl;
      return;
    }

    topic = "/world/" + worldName + "/stats";
  }

  topic = transport::TopicUtils::AsValidTopic(topic);
  if (topic.empty())
  {
    gzerr << "Failed to create valid topic for world [" << worldName << "]"
           << std::endl;
    return;
  }

  if (!this->dataPtr->node.Subscribe(topic, &WorldStats::OnWorldStatsMsg,
      this))
  {
    gzerr << "Failed to subscribe to [" << topic << "]" << std::endl;
    return;
  }

  gzmsg << "Listening to stats on [" << topic << "]" << std::endl;

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

  std::chrono::steady_clock::time_point simTimePoint;
  std::chrono::steady_clock::time_point realTimePoint;

  if (this->dataPtr->msg.has_sim_time())
  {
    simTimePoint = math::secNsecToTimePoint(
        this->dataPtr->msg.sim_time().sec(),
        this->dataPtr->msg.sim_time().nsec());
    this->SetSimTime(QString::fromStdString(
      math::timePointToString(simTimePoint)));
  }

  if (this->dataPtr->msg.has_real_time())
  {
    realTimePoint = math::secNsecToTimePoint(
        this->dataPtr->msg.real_time().sec(),
        this->dataPtr->msg.real_time().nsec());
    this->SetRealTime(QString::fromStdString(
      math::timePointToString(realTimePoint)));
  }

  {
    const double simTimeCount =
        static_cast<double>(simTimePoint.time_since_epoch().count());
    const double realTimeCount =
        static_cast<double>(realTimePoint.time_since_epoch().count());

    if (realTimeCount > 0)
    {
      constexpr double kAlpha = 0.9;
      this->dataPtr->simTimeDelayed =
        kAlpha * this->dataPtr->simTimeDelayed.value_or(simTimeCount) +
        (1.0 - kAlpha) * simTimeCount;

      this->dataPtr->realTimeDelayed =
        kAlpha * this->dataPtr->realTimeDelayed.value_or(realTimeCount) +
        (1.0 - kAlpha) * realTimeCount;

      // Compute the average sim and real times.
      const double realTimeFiltered =
          realTimeCount - (*this->dataPtr->realTimeDelayed);
      const double simTimeFiltered =
          simTimeCount - (*this->dataPtr->simTimeDelayed);

      // RTF, only compute this if the realTime count is greater than zero. The
      // realtTime count could be zero if simulation was started paused.
      if (realTimeFiltered > 0)
      {
        const double rtf =
            math::precision(simTimeFiltered / realTimeFiltered, 4) * 100;
        this->SetRealTimeFactor(QString::number(rtf, 'f', 2) + " %");
      }
    }
    else
    {
      // RTF as a percentage.
      double rtf = this->dataPtr->msg.real_time_factor() * 100;
      this->SetRealTimeFactor(QString::number(rtf, 'f', 2) + " %");
    }
  }

  {
    this->SetIterations(QString::number(this->dataPtr->msg.iterations()));
  }
}

/////////////////////////////////////////////////
void WorldStats::OnWorldStatsMsg(const msgs::WorldStatistics &_msg)
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
}  // namespace gz::gui::plugins

// Register this plugin
GZ_ADD_PLUGIN(gz::gui::plugins::WorldStats,
              gz::gui::Plugin)
