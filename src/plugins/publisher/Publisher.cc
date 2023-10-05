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

#include <gz/utils/ImplPtr.hh>
#include <iostream>
#include <gz/common/Console.hh>
#include <gz/msgs/Utility.hh>
#include <gz/plugin/Register.hh>
#include <gz/transport/Node.hh>

#include "Publisher.hh"

namespace gz::gui::plugins
{
class Publisher::Implementation
{
  /// \brief Message type
  public: QString msgType = "gz.msgs.StringMsg";

  /// \brief Message contents
  public: QString msgData = "data: \"Hello\"";

  /// \brief Topic
  public: QString topic = "/echo";

  /// \brief Frequency
  public: double frequency = 1.0;

  /// \brief Timer to keep publishing
  public: QTimer *timer;

  /// \brief Node for communication
  public: gz::transport::Node node;

  /// \brief Publisher
  public: gz::transport::Node::Publisher pub;
};

/////////////////////////////////////////////////
Publisher::Publisher()
  : dataPtr(gz::utils::MakeUniqueImpl<Implementation>())
{
}

/////////////////////////////////////////////////
Publisher::~Publisher() = default;

/////////////////////////////////////////////////
void Publisher::LoadConfig(const tinyxml2::XMLElement *_pluginElem)
{
  if (this->title.empty())
    this->title = "Publisher";

  // Parameters from SDF
  if (_pluginElem)
  {
    auto typeElem = _pluginElem->FirstChildElement("message_type");
    if (nullptr != typeElem && nullptr != typeElem->GetText())
      this->dataPtr->msgType = typeElem->GetText();

    auto msgElem = _pluginElem->FirstChildElement("message");
    if (nullptr != msgElem && nullptr != msgElem->GetText())
      this->dataPtr->msgData = msgElem->GetText();

    auto topicElem = _pluginElem->FirstChildElement("topic");
    if (nullptr != topicElem && nullptr != topicElem->GetText())
      this->dataPtr->topic = topicElem->GetText();

    if (auto frequencyElem = _pluginElem->FirstChildElement("frequency"))
      frequencyElem->QueryDoubleText(&this->dataPtr->frequency);
  }

  this->dataPtr->timer = new QTimer(this);
}

/////////////////////////////////////////////////
void Publisher::OnPublish(const bool _checked)
{
  if (!_checked)
  {
    if (this->dataPtr->timer != nullptr)
    {
      this->dataPtr->timer->stop();
      this->disconnect(this->dataPtr->timer, 0, 0, 0);
    }
    this->dataPtr->pub = transport::Node::Publisher();
    return;
  }

  auto topic = this->dataPtr->topic.toStdString();
  auto msgType = this->dataPtr->msgType.toStdString();
  auto msgData = this->dataPtr->msgData.toStdString();

  // Check it's possible to create message
  auto msg = msgs::Factory::New(msgType, msgData);
  if (!msg || (msg->DebugString() == "" && msgData != ""))
  {
    gzerr << "Unable to create message of type[" << msgType << "] "
      << "with data[" << msgData << "].\n";
    // TODO(anyone): notify error and uncheck switch
    return;
  }

  // Advertise the topic
  this->dataPtr->pub = this->dataPtr->node.Advertise(topic, msgType);

  if (!this->dataPtr->pub)
  {
    gzerr << "Unable to publish on topic[" << topic << "] "
      << "with message type[" << msgType << "].\n";
    // TODO(anyone): notify error and uncheck switch
    return;
  }

  // Zero frequency, publish once
  if (this->dataPtr->frequency < 0.00001)
  {
    this->dataPtr->pub.Publish(*msg);
    // TODO(anyone): notify error and uncheck switch
    return;
  }

  this->dataPtr->timer->setInterval(1000/this->dataPtr->frequency);
  this->connect(this->dataPtr->timer, &QTimer::timeout, [=]()
  {
    auto newMsg = msgs::Factory::New(msgType, msgData);
    this->dataPtr->pub.Publish(*newMsg);
  });
  this->dataPtr->timer->start();
}

/////////////////////////////////////////////////
QString Publisher::MsgType() const
{
  return this->dataPtr->msgType;
}

/////////////////////////////////////////////////
void Publisher::SetMsgType(const QString &_msgType)
{
  this->dataPtr->msgType = _msgType;
  this->MsgTypeChanged();
}

/////////////////////////////////////////////////
QString Publisher::MsgData() const
{
  return this->dataPtr->msgData;
}

/////////////////////////////////////////////////
void Publisher::SetMsgData(const QString &_msgData)
{
  this->dataPtr->msgData = _msgData;
  this->MsgDataChanged();
}

/////////////////////////////////////////////////
QString Publisher::Topic() const
{
  return this->dataPtr->topic;
}

/////////////////////////////////////////////////
void Publisher::SetTopic(const QString &_topic)
{
  this->dataPtr->topic = _topic;
  this->TopicChanged();
}

/////////////////////////////////////////////////
double Publisher::Frequency() const
{
  return this->dataPtr->frequency;
}

/////////////////////////////////////////////////
void Publisher::SetFrequency(const double _frequency)
{
  this->dataPtr->frequency = _frequency;
  this->FrequencyChanged();
}
}  // namespace gz::gui::plugins

// Register this plugin
GZ_ADD_PLUGIN(gz::gui::plugins::Publisher,
              gz::gui::Plugin)
