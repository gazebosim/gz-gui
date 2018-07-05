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

#include <iomanip>

#include <ignition/common/Console.hh>
#include <ignition/common/PluginMacros.hh>
#include <ignition/common/Time.hh>
#include <ignition/rendering/Text.hh>
#include <ignition/transport.hh>

#include "ignition/gui/plugins/displays/RealtimeFactorDisplay.hh"

namespace ignition
{
namespace gui
{
namespace plugins
{
namespace displays
{
  class RealtimeFactorDisplayPrivate
  {
    /// \brief Message holding latest world statistics
    public: ignition::msgs::WorldStatistics msg;

    /// \brief Mutex to protect msg
    public: std::recursive_mutex mutex;

    /// \brief Communication node
    public: ignition::transport::Node node;

    /// \brief The text display
    // TODO(dhood): Make an overlay
    public: ignition::rendering::TextPtr realtimeFactorText;
  };
}
}
}
}

using namespace ignition;
using namespace gui;
using namespace plugins;
using namespace displays;

/////////////////////////////////////////////////
RealtimeFactorDisplay::RealtimeFactorDisplay()
  : DisplayPlugin(), dataPtr(new RealtimeFactorDisplayPrivate)
{
}

/////////////////////////////////////////////////
RealtimeFactorDisplay::~RealtimeFactorDisplay()
{
}

/////////////////////////////////////////////////
void RealtimeFactorDisplay::Initialize(const tinyxml2::XMLElement *_pluginElem)
{
  (void)_pluginElem;
  this->title = "Realtime factor";

  // Subscribe to world_stats
  std::string topic = "/world_stats";
  if (!this->dataPtr->node.Subscribe(topic,
      &RealtimeFactorDisplay::OnWorldStatsMsg, this))
  {
    ignerr << "Failed to subscribe to [" << topic << "]" << std::endl;
  }

  this->dataPtr->realtimeFactorText = this->Scene()->CreateText();
  this->dataPtr->realtimeFactorText->SetTextString("Realtime factor: ? %");
  this->dataPtr->realtimeFactorText->SetShowOnTop(true);

  auto mat = this->Scene()->CreateMaterial();
  // TODO(dhood): Configurable properties
  this->Visual()->AddGeometry(this->dataPtr->realtimeFactorText);
  this->Visual()->SetMaterial(mat);
}

/////////////////////////////////////////////////
void RealtimeFactorDisplay::OnVisibilityChange(bool _value)
{
  // TODO(dhood): remove this once parent visual has setVisible
  if (_value)
  {
    this->Visual()->Material()->SetTransparency(0.);
  }
  else
  {
    // TODO(dhood): this doesn't make TextPtr invisible
    this->Visual()->Material()->SetTransparency(1.);
  }
}

/////////////////////////////////////////////////
void RealtimeFactorDisplay::ProcessMsg()
{
  std::lock_guard<std::recursive_mutex> lock(this->dataPtr->mutex);

  if (this->dataPtr->msg.has_real_time_factor())
  {
    // RTF as a percentage.
    double rtf = this->dataPtr->msg.real_time_factor() * 100;
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2);
    ss << "Realtime factor: " << rtf << "%";
    this->dataPtr->realtimeFactorText->SetTextString(ss.str());
  }
}

/////////////////////////////////////////////////
void RealtimeFactorDisplay::OnWorldStatsMsg(
    const ignition::msgs::WorldStatistics &_msg)
{
  std::lock_guard<std::recursive_mutex> lock(this->dataPtr->mutex);

  this->dataPtr->msg.CopyFrom(_msg);
  QMetaObject::invokeMethod(this, "ProcessMsg");
}

// Register this plugin
IGN_COMMON_REGISTER_SINGLE_PLUGIN(
  ignition::gui::plugins::displays::RealtimeFactorDisplay,
  ignition::gui::Plugin)
