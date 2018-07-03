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

#include "ignition/gui/plugins/WorldControl.hh"

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
  if (auto serviceElem = _pluginElem->FirstChildElement("service"))
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
    auto hasPlay = false;
    playElem->QueryBoolText(&hasPlay);

    if (hasPlay)
    {
//          QLabel *stepLabel = new QLabel(tr("Steps:"));
//          QSpinBox *stepSpinBox = new QSpinBox;
//          stepSpinBox->setRange(1, 9999);
//
//          this->connect(this, &WorldControl::Playing,
//            [=]()
//            {
//              stepButton->setDisabled(true);
//              stepSpinBox->setDisabled(true);
//            });
//          this->connect(this, &WorldControl::Paused,
//            [=]()
//            {
//              stepButton->setDisabled(false);
//              stepSpinBox->setDisabled(false);
//            });
//          this->connect(stepSpinBox,
//            static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
//            [=](int _newValue) {this->dataPtr->multiStep = _newValue;});

      auto startPaused = false;
      if (auto pausedElem = _pluginElem->FirstChildElement("start_paused"))
      {
        pausedElem->QueryBoolText(&startPaused);
      }
      if (startPaused)
        this->Paused();
      else
        this->Playing();
    }
  }

  // Subscribe to world stats
  std::string statsTopic;
  if (auto statsTopicElem = _pluginElem->FirstChildElement("stats_topic"))
    statsTopic = statsTopicElem->GetText();

  if (statsTopic.empty())
  {
    ignwarn << "No statsTopic, play/pause button status will not be updated."
           << std::endl;
  }
  else
  {
    // Subscribe to world_stats
    if (!this->dataPtr->node.Subscribe(statsTopic, &WorldControl::OnWorldStatsMsg,
        this))
    {
      ignerr << "Failed to subscribe to [" << statsTopic << "]" << std::endl;
    }
  }
}

/////////////////////////////////////////////////
void WorldControl::ProcessMsg()
{
  std::lock_guard<std::recursive_mutex> lock(this->dataPtr->mutex);

  if (this->dataPtr->msg.paused())
    this->Paused();
  else
    this->Playing();
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
      QMetaObject::invokeMethod(this, "Playing");
  };

  ignition::msgs::WorldControl req;
  req.set_pause(false);
  this->dataPtr->node.Request(this->dataPtr->controlService, req, cb);
}

/////////////////////////////////////////////////
void WorldControl::OnPause()
{
  std::function<void(const ignition::msgs::Boolean &, const bool)> cb =
      [this](const ignition::msgs::Boolean &/*_rep*/, const bool _result)
  {
    if (_result)
      QMetaObject::invokeMethod(this, "Paused");
  };

  ignition::msgs::WorldControl req;
  req.set_pause(true);
  this->dataPtr->node.Request(this->dataPtr->controlService, req, cb);
}

/////////////////////////////////////////////////
void WorldControl::OnStep()
{
  std::function<void(const ignition::msgs::Boolean &, const bool)> cb =
      [this](const ignition::msgs::Boolean &/*_rep*/, const bool /*_result*/)
  {
  };

  ignition::msgs::WorldControl req;
  req.set_multi_step(this->dataPtr->multiStep);
  this->dataPtr->node.Request(this->dataPtr->controlService, req, cb);
}

// Register this plugin
IGN_COMMON_REGISTER_SINGLE_PLUGIN(ignition::gui::plugins::WorldControl,
                                  ignition::gui::Plugin)
