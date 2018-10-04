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
#include <mutex>
#include <string>

#include <ignition/common/Console.hh>
#include <ignition/plugin/Register.hh>
#include <ignition/transport/Node.hh>

#include "ignition/gui/MessageWidget.hh"
#include "ignition/gui/plugins/TopicInterface.hh"

namespace ignition
{
namespace gui
{
namespace plugins
{
  class TopicInterfacePrivate
  {
    /// \brief Pointer to message widget.
    public: MessageWidget *msgWidget = nullptr;

    /// \brief Mutex to protect message buffer.
    public: std::mutex mutex;

    /// \brief Node for communication.
    public: ignition::transport::Node node;

    /// \brief List of widgets which should be hidden
    public: std::vector<std::string> hideWidgets;

    /// \brief Whether the whole widget should be read-only.
    public: bool readOnly;

    /// \brief Latest received message
    public: google::protobuf::Message *msg{nullptr};

    /// \brief The Ignition Transport topic to subscribe.
    public: std::string topic = "/echo";
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

  // Layout
  auto layout = new QVBoxLayout();
  this->setLayout(layout);

  // Parameters from SDF
  if (_pluginElem)
  {
    if (auto topicElem = _pluginElem->FirstChildElement("topic"))
      this->dataPtr->topic = topicElem->GetText();
    if (this->dataPtr->topic.empty())
    {
      ignwarn << "Topic not specified, subscribing to [/echo]." << std::endl;
    }

    std::string msgType;
    if (auto typeElem = _pluginElem->FirstChildElement("message_type"))
      msgType = typeElem->GetText();
    if (msgType.empty())
    {
      ignwarn << "Message type not specified, widget will be constructed "
              << "according to the first message received on topic ["
              << this->dataPtr->topic << "]." << std::endl;
    }

    // Global read-only
    this->dataPtr->readOnly = false;
    if (_pluginElem->Attribute("read_only"))
      _pluginElem->QueryBoolAttribute("read_only", &this->dataPtr->readOnly);

    // Visibility per widget
    for (auto hideWidgetElem = _pluginElem->FirstChildElement("hide");
        hideWidgetElem != nullptr;
        hideWidgetElem = hideWidgetElem->NextSiblingElement("hide"))
    {
      this->dataPtr->hideWidgets.push_back(hideWidgetElem->GetText());
    }

    // Message widget
    if (!msgType.empty())
    {
      auto msg = ignition::msgs::Factory::New(msgType, "");
      if (!msg)
      {
        ignerr << "Unable to create message of type[" << msgType << "] "
          << "widget will be initialized when a message is received."
          << std::endl;
      }
      else
      {
        this->dataPtr->msg = msg->New();
        this->dataPtr->msg->CopyFrom(*msg);
        this->CreateWidget();
      }
    }
  }

  // Subscribe
  if (!this->dataPtr->node.Subscribe(this->dataPtr->topic,
        &TopicInterface::OnMessage, this))
  {
    ignerr << "Failed to subscribe to topic [" << this->dataPtr->topic
           << "]" << std::endl;
  }

  // Connect
  this->connect(this, SIGNAL(MessageReceived()), this, SLOT(OnMessageImpl()));
}

/////////////////////////////////////////////////
void TopicInterface::OnMessage(const google::protobuf::Message &_msg)
{
  std::lock_guard<std::mutex> lock(this->dataPtr->mutex);

  this->dataPtr->msg = _msg.New();
  this->dataPtr->msg->CopyFrom(_msg);
  this->MessageReceived();
}

/////////////////////////////////////////////////
void TopicInterface::OnMessageImpl()
{
  std::lock_guard<std::mutex> lock(this->dataPtr->mutex);

  if (!this->dataPtr->msgWidget)
    this->CreateWidget();
  else
    this->dataPtr->msgWidget->UpdateFromMsg(this->dataPtr->msg);
}

/////////////////////////////////////////////////
void TopicInterface::CreateWidget()
{
  this->dataPtr->msgWidget = new MessageWidget(this->dataPtr->msg);

  for (const auto &w : this->dataPtr->hideWidgets)
    this->dataPtr->msgWidget->SetPropertyVisible(w, false);

  this->dataPtr->msgWidget->SetReadOnly(this->dataPtr->readOnly);

  // Needed for drag and drop.
  this->dataPtr->msgWidget->SetTopic(this->dataPtr->topic);

  // Scroll area
  auto scrollArea = new QScrollArea();
  scrollArea->setWidget(this->dataPtr->msgWidget);
  scrollArea->setWidgetResizable(true);

  this->layout()->addWidget(scrollArea);
}

// Register this plugin
IGNITION_ADD_PLUGIN(ignition::gui::plugins::TopicInterface,
                    ignition::gui::Plugin)

