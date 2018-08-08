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
#include <ignition/rendering/Text.hh>
#include <ignition/transport.hh>

#include "ignition/gui/display_plugins/RealtimeFactorDisplay.hh"

namespace ignition
{
namespace gui
{
namespace display_plugins
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
    public: ignition::rendering::TextPtr realtimeFactorText = nullptr;
  };
}
}
}

using namespace ignition;
using namespace gui;
using namespace display_plugins;

/////////////////////////////////////////////////
RealtimeFactorDisplay::RealtimeFactorDisplay()
  : DisplayPlugin(), dataPtr(new RealtimeFactorDisplayPrivate)
{
  this->title = "Real time factor";
}

/////////////////////////////////////////////////
RealtimeFactorDisplay::~RealtimeFactorDisplay()
{
}

/////////////////////////////////////////////////
void RealtimeFactorDisplay::Initialize(
  const tinyxml2::XMLElement */*_pluginElem*/)
{
  // Subscribe to world_stats
  std::string topic = "/world_stats";
  if (!this->dataPtr->node.Subscribe(topic,
      &RealtimeFactorDisplay::OnWorldStatsMsg, this))
  {
    ignerr << "Failed to subscribe to [" << topic << "]" << std::endl;
  }

  if (auto scenePtr = this->Scene().lock())
  {
    this->dataPtr->realtimeFactorText = scenePtr->CreateText();
    // By default the Visual is attached to the RootVisual.
    // Remove it from the RootVisual so it can be attached to a Camera instead.
    auto root = scenePtr->RootVisual();
    root->RemoveChild(this->Visual());

    // Loop through the children looking for a Camera.
    bool cameraFound = false;
    for (unsigned int i = 0; i < root->ChildCount(); ++i)
    {
      auto camera = std::dynamic_pointer_cast<rendering::Camera>(
        root->ChildByIndex(i));
      if (camera)
      {
        if (!cameraFound)
        {
          cameraFound = true;
          camera->AddChild(this->Visual());
          continue;
        }
        ignwarn << "Multiple cameras found for scene [" << scenePtr->Name() <<
          "]. Real time factor display will be attached to the first camera "
          "and may be visible from other cameras." << std::endl;
      }
    }
  }
  else
  {
    ignerr << "Scene invalid. Real time factor display not initialized."
      << std::endl;
    return;
  }
  this->dataPtr->realtimeFactorText->SetTextString("Real time factor: ? %");
  this->dataPtr->realtimeFactorText->SetShowOnTop(true);
  this->dataPtr->realtimeFactorText->SetCharHeight(0.2);
  this->dataPtr->realtimeFactorText->SetTextAlignment(
    ignition::rendering::TextHorizontalAlign::RIGHT,
    ignition::rendering::TextVerticalAlign::CENTER);

  // TODO(dhood): Configurable properties
  this->Visual()->AddGeometry(this->dataPtr->realtimeFactorText);

  // TODO(dhood): Don't hard-code these offsets.
  // Coordinate axes of the camera are: positive X is into the scene, positive
  // Y is to the left, and positive Z is up.
  this->Visual()->SetLocalPosition(5,-0.8,-1.);
}

/////////////////////////////////////////////////
std::string RealtimeFactorDisplay::Type() const
{
  return "RealtimeFactorDisplay";
}

/////////////////////////////////////////////////
void RealtimeFactorDisplay::ProcessMsg()
{
  if (nullptr == this->dataPtr->realtimeFactorText)
  {
    return;
  }
  std::lock_guard<std::recursive_mutex> lock(this->dataPtr->mutex);

  if (this->dataPtr->msg.has_real_time_factor())
  {
    // RTF as a percentage.
    double rtf = this->dataPtr->msg.real_time_factor() * 100;
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2);
    ss << "Real time factor: " << rtf << "%";
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
  ignition::gui::display_plugins::RealtimeFactorDisplay,
  ignition::gui::DisplayPlugin)
