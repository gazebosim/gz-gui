/*
 * Copyright (C) 2021 Open Robotics
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

#include "ShutdownButton.hh"

#include <csignal>
#include <string>

#include <ignition/common/Console.hh>
#include <ignition/common/Time.hh>
#include <ignition/common/StringUtils.hh>
#include <ignition/plugin/Register.hh>

#include "ignition/gui/Helpers.hh"

namespace ignition
{
namespace gui
{
namespace plugins
{
  class ShutdownButtonPrivate
  {
    /// \brief Service to send server control requests
    public: std::string controlService;

    /// \brief Communication node
    public: ignition::transport::Node node;
  };
}
}
}

using namespace ignition;
using namespace gui;
using namespace plugins;

/////////////////////////////////////////////////
ShutdownButton::ShutdownButton()
  : Plugin(), dataPtr(new ShutdownButtonPrivate)
{
}

/////////////////////////////////////////////////
ShutdownButton::~ShutdownButton()
{
}

/////////////////////////////////////////////////
void ShutdownButton::LoadConfig(const tinyxml2::XMLElement *_pluginElem)
{
  // Default name in case user didn't define one
  if (this->title.empty())
    this->title = "Shutdown";

  // Create elements from configuration
  if (!_pluginElem)
  {
    ignerr << "Null plugin element." << std::endl;
    return;
  }

  // For world control requests
  this->dataPtr->controlService = "/server_control";
  auto serviceElem = _pluginElem->FirstChildElement("service");
  if (nullptr != serviceElem && nullptr != serviceElem->GetText())
    this->dataPtr->controlService = serviceElem->GetText();

  this->dataPtr->controlService = transport::TopicUtils::AsValidTopic(
      this->dataPtr->controlService);

  if (this->dataPtr->controlService.empty())
  {
    ignerr << "Failed to create valid server service" << std::endl;
    return;
  }

  ignmsg << "Using server control service [" << this->dataPtr->controlService
         << "]" << std::endl;
}

/////////////////////////////////////////////////
void ShutdownButton::OnStop()
{
  std::function<void(const ignition::msgs::Boolean &, const bool)> cb =
      [](const ignition::msgs::Boolean &/*_rep*/, const bool /*_result*/)
  {
    ignlog << "Request to shutdown the simulation server sent. Stopping client "
           << "now." << std::endl;
    std::raise(SIGTERM);
  };

  ignition::msgs::ServerControl req;
  req.set_stop(true);
  const auto success = this->dataPtr->node.Request(
    this->dataPtr->controlService, req, cb);

  if (!success)
  {
    ignerr << "Calling service [" << this->dataPtr->controlService << "] to "
           << "stop the server failed. Please check that the <service> of the "
           << "ShutdownButton is configured correctly and that the server is "
           << "running in the same IGN_PARTITION and with the same "
           << "configuration of IGN_TRANSPORT_TOPIC_STATISTICS." << std::endl;
  }
}

// Register this plugin
IGNITION_ADD_PLUGIN(ignition::gui::plugins::ShutdownButton,
                    ignition::gui::Plugin)
