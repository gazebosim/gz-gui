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

#include <iostream>
#include <ignition/common/Console.hh>
#include <ignition/common/PluginMacros.hh>
#include <ignition/transport/Node.hh>

#include "ignition/gui/ConfigWidget.hh"
#include "ignition/gui/plugins/TopicInterface.hh"

namespace ignition
{
namespace gui
{
namespace plugins
{
  class TopicInterfacePrivate
  {
    public: ConfigWidget *config;
    /// \brief Mutex to protect message buffer.
    public: std::mutex mutex;

    /// \brief Node for communication
    public: ignition::transport::Node node;
  };
}
}
}

using namespace ignition;
using namespace gui;
using namespace plugins;

/////////////////////////////////////////////////
TopicInterface::TopicInterface()
  : Plugin(), dataPtr(new TopicInterfacePrivate)
{
}

/////////////////////////////////////////////////
TopicInterface::~TopicInterface()
{
}

/////////////////////////////////////////////////
void TopicInterface::LoadConfig(const tinyxml2::XMLElement *_pluginElem)
{
  if (this->title.empty())
    this->title = "Topic interface";

  this->dataPtr->config = new ConfigWidget();

  auto layout = new QVBoxLayout();
  layout->addWidget(this->dataPtr->config);
  this->setLayout(layout);

  // Subscribe
  std::string topic;
  if (auto topicElem = _pluginElem->FirstChildElement("topic"))
    topic = topicElem->GetText();

  if (topic.empty())
  {
    ignwarn << "Topic not specified, subscribing to [/echo]." << std::endl;
    topic = "/echo";
  }

  if (!this->dataPtr->node.Subscribe(topic, &TopicInterface::OnMessage, this))
  {
    ignerr << "Invalid topic [" << topic << "]" << std::endl;
  }
}

/////////////////////////////////////////////////
void TopicInterface::OnMessage(const google::protobuf::Message &_msg)
{
  std::lock_guard<std::mutex> lock(this->dataPtr->mutex);

  this->dataPtr->config->Load(&_msg);
}

// Register this plugin
IGN_COMMON_REGISTER_SINGLE_PLUGIN(ignition::gui::plugins::TopicInterface,
                                  ignition::gui::Plugin)

