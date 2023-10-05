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

#include "WorldControl.hh"

#include <string>

#include <gz/msgs/boolean.pb.h>
#include <gz/msgs/world_control.pb.h>
#include <gz/msgs/world_stats.pb.h>

#include <gz/common/Console.hh>
#include <gz/common/StringUtils.hh>
#include <gz/plugin/Register.hh>
#include <gz/transport/Node.hh>

#include "gz/gui/Application.hh"
#include "gz/gui/Helpers.hh"
#include "gz/gui/GuiEvents.hh"
#include "gz/gui/MainWindow.hh"

namespace gz::gui::plugins
{
class WorldControlPrivate
{
  /// \brief Send the world control event or call the control service.
  /// \param[in] _msg Message to send.
  public: void SendEventMsg(const gz::msgs::WorldControl &_msg);

  /// \brief Message holding latest world statistics
  public: gz::msgs::WorldStatistics msg;

  /// \brief Service to send world control requests
  public: std::string controlService;

  /// \brief Mutex to protect msg
  public: std::recursive_mutex mutex;

  /// \brief Communication node
  public: gz::transport::Node node;

  /// \brief The multi step value
  public: unsigned int multiStep = 1u;

  /// \brief True for paused
  public: bool pause{true};

  /// \brief The paused state of the most recently received world stats msg
  /// (true for paused)
  public: bool lastStatsMsgPaused{true};

  /// \brief Whether server communication should occur through an event (true)
  /// or service (false). The service option was used by default for
  /// gz-gui7 and earlier, and now uses the event by default in gz-gui8.
  public: bool useEvent{true};
};

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
    gzerr << "Null plugin element." << std::endl;
    return;
  }

  // If no elements were set, show all buttons. We assume that the user never
  // wants to hide all buttons. This happens for example when the plugin is
  // inserted from the menu.
  if (_pluginElem->NoChildren())
  {
    this->PluginItem()->setProperty("showPlay", true);
    this->PluginItem()->setProperty("showStep", true);
  }

  // World name from window, to construct default topics and services
  std::string worldName;
  auto worldNames = gui::worldNames();
  if (!worldNames.empty())
    worldName = worldNames[0].toStdString();

  // For world control requests
  auto serviceElem = _pluginElem->FirstChildElement("service");
  if (nullptr != serviceElem && nullptr != serviceElem->GetText())
    this->dataPtr->controlService = serviceElem->GetText();

  // Service specified with different world name
  auto parts = common::Split(this->dataPtr->controlService, '/');
  if (!worldName.empty() &&
      parts.size() == 4 &&
      parts[0] == "" &&
      parts[1] == "world" &&
      parts[2] != worldName &&
      parts[3] == "control")
  {
    gzwarn << "Ignoring service [" << this->dataPtr->controlService
            << "], world name different from [" << worldName
            << "]. Fix or remove your <service> tag." << std::endl;

    this->dataPtr->controlService = "/world/" + worldName + "/control";
  }

  // Service unspecified, use world name
  if (this->dataPtr->controlService.empty())
  {
    if (worldName.empty())
    {
      gzerr << "Must specify a <service> for world control requests, or set "
             << "the MainWindow's [worldNames] property." << std::endl;
      return;
    }

    this->dataPtr->controlService = "/world/" + worldName + "/control";
  }
  this->dataPtr->controlService = transport::TopicUtils::AsValidTopic(
      this->dataPtr->controlService);

  if (this->dataPtr->controlService.empty())
  {
    gzerr << "Failed to create valid control service for world [" << worldName
           << "]" << std::endl;
  }

  gzmsg << "Using world control service [" << this->dataPtr->controlService
         << "]" << std::endl;

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
      this->dataPtr->lastStatsMsgPaused = startPaused;
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

  // Service specified with different world name
  parts = common::Split(statsTopic, '/');
  if (!worldName.empty() &&
      parts.size() == 4 &&
      parts[0] == "" &&
      parts[1] == "world" &&
      parts[2] != worldName &&
      parts[3] == "stats")
  {
    gzwarn << "Ignoring topic [" << statsTopic
            << "], world name different from [" << worldName
            << "]. Fix or remove your <stats_topic> tag." << std::endl;

    statsTopic = "/world/" + worldName + "/stats";
  }

  if (statsTopic.empty() && !worldName.empty())
  {
    statsTopic = "/world/" + worldName + "/stats";
  }

  statsTopic = transport::TopicUtils::AsValidTopic(statsTopic);
  if (!statsTopic.empty())
  {
    // Subscribe to world_stats
    if (!this->dataPtr->node.Subscribe(statsTopic,
        &WorldControl::OnWorldStatsMsg, this))
    {
      gzerr << "Failed to subscribe to [" << statsTopic << "]" << std::endl;
    }
    else
    {
      gzmsg << "Listening to stats on [" << statsTopic << "]" << std::endl;
    }
  }
  else
  {
    gzerr << "Failed to create valid topic for world [" << worldName << "]"
           << std::endl;
  }

  if (auto elem = _pluginElem->FirstChildElement("use_event"))
    elem->QueryBoolText(&this->dataPtr->useEvent);

  if (this->dataPtr->useEvent)
    gzdbg << "Using an event to share WorldControl msgs with the server\n";
  else
    gzdbg << "Using a service to share WorldControl msgs with the server\n";
}

/////////////////////////////////////////////////
void WorldControl::ProcessMsg()
{
  std::lock_guard<std::recursive_mutex> lock(this->dataPtr->mutex);

  // ignore the message if it's associated with a step
  const auto &header = this->dataPtr->msg.header();
  if (this->dataPtr->msg.stepping() ||
      // (deprecated) Remove this check in Gazebo H
      ((header.data_size() > 0) && (header.data(0).key() == "step")))
  {
    return;
  }

  // If the pause state of the message doesn't match the pause state of this
  // plugin, then play/pause must have occurred elsewhere (for example, the
  // command line). If the pause state of the message matches the pause state
  // of this plugin, but the pause state of the message differs from the
  // previous message's pause state, this means that a pause/play request from
  // this plugin has been registered by the server
  if (this->dataPtr->msg.paused() &&
      (!this->dataPtr->pause || !this->dataPtr->lastStatsMsgPaused))
    this->paused();
  else if (!this->dataPtr->msg.paused() &&
      (this->dataPtr->pause || this->dataPtr->lastStatsMsgPaused))
    this->playing();

  this->dataPtr->pause = this->dataPtr->msg.paused();
  this->dataPtr->lastStatsMsgPaused = this->dataPtr->msg.paused();
}

/////////////////////////////////////////////////
void WorldControl::OnWorldStatsMsg(const msgs::WorldStatistics &_msg)
{
  std::lock_guard<std::recursive_mutex> lock(this->dataPtr->mutex);

  this->dataPtr->msg.CopyFrom(_msg);
  QMetaObject::invokeMethod(this, "ProcessMsg");
}

/////////////////////////////////////////////////
void WorldControl::OnPlay()
{
  msgs::WorldControl msg;
  msg.set_pause(false);
  this->dataPtr->pause = false;
  this->dataPtr->SendEventMsg(msg);
}

/////////////////////////////////////////////////
void WorldControl::OnPause()
{
  msgs::WorldControl msg;
  msg.set_pause(true);
  this->dataPtr->pause = true;

  this->dataPtr->SendEventMsg(msg);
}

/////////////////////////////////////////////////
void WorldControl::OnReset()
{
  msgs::WorldControl msg;
  auto msgReset = new msgs::WorldReset();
  msgReset->set_all(true);
  msg.set_pause(true);
  msg.set_allocated_reset(msgReset);

  this->dataPtr->SendEventMsg(msg);
}

/////////////////////////////////////////////////
void WorldControl::OnStepCount(const unsigned int _steps)
{
  this->dataPtr->multiStep = _steps;
}

/////////////////////////////////////////////////
void WorldControl::OnStep()
{
  msgs::WorldControl msg;
  msg.set_pause(this->dataPtr->pause);
  msg.set_multi_step(this->dataPtr->multiStep);

  this->dataPtr->SendEventMsg(msg);
}

/////////////////////////////////////////////////
void WorldControlPrivate::SendEventMsg(const msgs::WorldControl &_msg)
{
  if (this->useEvent)
  {
    gui::events::WorldControl event(_msg);
    App()->sendEvent(App()->findChild<MainWindow *>(), &event);
  }
  else
  {
    std::function<void(const msgs::Boolean &, const bool)> cb =
        [](const msgs::Boolean &/*_rep*/, const bool /*_result*/)
    {
      // the service CB is empty because updates are handled in
      // WorldControl::ProcessMsg
    };
    this->node.Request(this->controlService, _msg, cb);
  }
}
}  // namespace gz::gui::plugins

// Register this plugin
GZ_ADD_PLUGIN(gz::gui::plugins::WorldControl,
              gz::gui::Plugin)
