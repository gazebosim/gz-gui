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

#include "WorldControl.hh"

namespace ignition
{
namespace gui
{
namespace plugins
{
  class WorldControlPrivate
  {
    /// \brief Message holding latest world statistics
    public: ignition::msgs::WorldStatistics msg;

    /// \brief Service to send world control requests
    public: std::string controlService;

    /// \brief Mutex to protect msg
    public: std::recursive_mutex mutex;

    /// \brief Communication node
    public: ignition::transport::Node node;

    /// \brief The multi step value
    public: unsigned int multiStep = 1u;

    /// \brief True for paused
    public: bool pause{true};
  };
}
}
}

using namespace ignition;
using namespace gui;
using namespace plugins;

/////////////////////////////////////////////////
WorldControl::WorldControl()
  : Plugin(), dataPtr(new WorldControlPrivate)
{
}

/////////////////////////////////////////////////
WorldControl::~WorldControl()
{
}

/////////////////////////////////////////////////
void WorldControl::LoadConfig(const tinyxml2::XMLElement *_pluginElem)
{
  // Default name in case user didn't define one
  if (this->title.empty())
    this->title = "World control";

  // Create elements from configuration
  if (!_pluginElem)
  {
    ignerr << "Null plugin element." << std::endl;
    return;
  }

  // For world control requests
  auto serviceElem = _pluginElem->FirstChildElement("service");
  if (nullptr != serviceElem && nullptr != serviceElem->GetText())
    this->dataPtr->controlService = serviceElem->GetText();

  if (this->dataPtr->controlService.empty())
  {
    ignerr << "Must specify a service for world control requests."
           << std::endl;
    return;
  }

  // Play / pause buttons
  if (auto playElem = _pluginElem->FirstChildElement("play_pause"))
  {
    auto has = false;
    playElem->QueryBoolText(&has);
    this->PluginItem()->setProperty("showPlay", has);

    if (has)
    {
      auto startPaused = this->dataPtr->pause;
      if (auto pausedElem = _pluginElem->FirstChildElement("start_paused"))
      {
        pausedElem->QueryBoolText(&startPaused);
      }
      this->dataPtr->pause = startPaused;
      if (startPaused)
        this->paused();
      else
        this->playing();
    }
  }

  // Step buttons
  if (auto stepElem = _pluginElem->FirstChildElement("step"))
  {
    auto has = false;
    stepElem->QueryBoolText(&has);
    this->PluginItem()->setProperty("showStep", has);
  }

  // Subscribe to world stats
  std::string statsTopic;
  auto statsTopicElem = _pluginElem->FirstChildElement("stats_topic");
  if (nullptr != statsTopicElem && nullptr != statsTopicElem->GetText())
    statsTopic = statsTopicElem->GetText();

  if (!statsTopic.empty())
  {
    // Subscribe to world_stats
    if (!this->dataPtr->node.Subscribe(statsTopic,
        &WorldControl::OnWorldStatsMsg, this))
    {
      ignerr << "Failed to subscribe to [" << statsTopic << "]" << std::endl;
    }
  }
}

/////////////////////////////////////////////////
void WorldControl::ProcessMsg()
{
  std::lock_guard<std::recursive_mutex> lock(this->dataPtr->mutex);

  if (!this->dataPtr->pause && this->dataPtr->msg.paused())
    this->paused();
  else if (this->dataPtr->pause && !this->dataPtr->msg.paused())
    this->playing();
  this->dataPtr->pause = this->dataPtr->msg.paused();
}

/////////////////////////////////////////////////
void WorldControl::OnWorldStatsMsg(const ignition::msgs::WorldStatistics &_msg)
{
  std::lock_guard<std::recursive_mutex> lock(this->dataPtr->mutex);

  this->dataPtr->msg.CopyFrom(_msg);
  QMetaObject::invokeMethod(this, "ProcessMsg");
}

/////////////////////////////////////////////////
void WorldControl::OnPlay()
{
  std::function<void(const ignition::msgs::Boolean &, const bool)> cb =
      [this](const ignition::msgs::Boolean &/*_rep*/, const bool _result)
  {
    if (_result)
      QMetaObject::invokeMethod(this, "playing");
  };

  ignition::msgs::WorldControl req;
  req.set_pause(false);
  this->dataPtr->pause = false;
  this->dataPtr->node.Request(this->dataPtr->controlService, req, cb);
}

/////////////////////////////////////////////////
void WorldControl::OnPause()
{
  std::function<void(const ignition::msgs::Boolean &, const bool)> cb =
      [this](const ignition::msgs::Boolean &/*_rep*/, const bool _result)
  {
    if (_result)
      QMetaObject::invokeMethod(this, "paused");
  };

  ignition::msgs::WorldControl req;
  req.set_pause(true);
  this->dataPtr->pause = true;
  this->dataPtr->node.Request(this->dataPtr->controlService, req, cb);
}

/////////////////////////////////////////////////
void WorldControl::OnStepCount(const unsigned int _steps)
{
  this->dataPtr->multiStep = _steps;
}

/////////////////////////////////////////////////
void WorldControl::OnStep()
{
  std::function<void(const ignition::msgs::Boolean &, const bool)> cb =
      [](const ignition::msgs::Boolean &/*_rep*/, const bool /*_result*/)
  {
  };

  ignition::msgs::WorldControl req;
  req.set_pause(this->dataPtr->pause);
  req.set_multi_step(this->dataPtr->multiStep);
  this->dataPtr->node.Request(this->dataPtr->controlService, req, cb);
}

// Register this plugin
IGNITION_ADD_PLUGIN(ignition::gui::plugins::WorldControl,
                    ignition::gui::Plugin)
