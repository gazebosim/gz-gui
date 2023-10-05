/*
 * Copyright (C) 2021 Open Source Robotics Foundation
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

#include "NavSatMap.hh"

#include <algorithm>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

#include <gz/common/Console.hh>
#include <gz/plugin/Register.hh>
#include <gz/transport/Node.hh>

#include "gz/gui/Application.hh"

namespace gz::gui::plugins
{
class NavSatMapPrivate
{
  /// \brief List of topics publishing navSat messages.
  public: QStringList topicList;

  /// \brief Holds data to set as the next navSat
  public: msgs::NavSat navSatMsg;

  /// \brief Node for communication.
  public: transport::Node node;

  /// \brief Mutex for accessing navSat data
  public: std::recursive_mutex navSatMutex;
};

/////////////////////////////////////////////////
NavSatMap::NavSatMap()
  : Plugin(), dataPtr(new NavSatMapPrivate)
{
}

/////////////////////////////////////////////////
NavSatMap::~NavSatMap()
{
}

/////////////////////////////////////////////////
void NavSatMap::LoadConfig(const tinyxml2::XMLElement *_pluginElem)
{
  // Default name in case user didn't define one
  if (this->title.empty())
    this->title = "Navigation satellite map";

  std::string topic;
  bool topicPicker = true;

  // Read configuration
  if (_pluginElem)
  {
    if (auto topicElem = _pluginElem->FirstChildElement("topic"))
      topic = topicElem->GetText();

    if (auto pickerElem = _pluginElem->FirstChildElement("topic_picker"))
      pickerElem->QueryBoolText(&topicPicker);
  }

  if (topic.empty() && !topicPicker)
  {
    gzwarn << "Can't hide topic picker without a default topic." << std::endl;
    topicPicker = true;
  }

  this->PluginItem()->setProperty("showPicker", topicPicker);

  if (!topic.empty())
  {
    this->SetTopicList({QString::fromStdString(topic)});
    this->OnTopic(QString::fromStdString(topic));
  }
  else
    this->OnRefresh();
}

/////////////////////////////////////////////////
void NavSatMap::ProcessMessage()
{
  std::lock_guard<std::recursive_mutex> lock(this->dataPtr->navSatMutex);

  this->newMessage(this->dataPtr->navSatMsg.latitude_deg(),
      this->dataPtr->navSatMsg.longitude_deg());
}

/////////////////////////////////////////////////
void NavSatMap::OnMessage(const msgs::NavSat &_msg)
{
  std::lock_guard<std::recursive_mutex> lock(this->dataPtr->navSatMutex);
  this->dataPtr->navSatMsg = _msg;

  // Signal to main thread that the navSat changed
  QMetaObject::invokeMethod(this, "ProcessMessage");
}

/////////////////////////////////////////////////
void NavSatMap::OnTopic(const QString _topic)
{
  auto topic = _topic.toStdString();
  if (topic.empty())
    return;

  // Unsubscribe
  auto subs = this->dataPtr->node.SubscribedTopics();
  for (auto sub : subs)
    this->dataPtr->node.Unsubscribe(sub);

  // Subscribe to new topic
  if (!this->dataPtr->node.Subscribe(topic, &NavSatMap::OnMessage,
      this))
  {
    gzerr << "Unable to subscribe to topic [" << topic << "]" << std::endl;
  }
}

/////////////////////////////////////////////////
void NavSatMap::OnRefresh()
{
  // Clear
  this->dataPtr->topicList.clear();

  // Get updated list
  std::vector<std::string> allTopics;
  this->dataPtr->node.TopicList(allTopics);
  for (auto topic : allTopics)
  {
    std::vector<transport::MessagePublisher> publishers;
    std::vector<transport::MessagePublisher> subscribers;
    this->dataPtr->node.TopicInfo(topic, publishers, subscribers);
    for (auto pub : publishers)
    {
      if (pub.MsgTypeName() == "gz.msgs.NavSat")
      {
        this->dataPtr->topicList.push_back(QString::fromStdString(topic));
        break;
      }
    }
  }

  // Select first one
  if (this->dataPtr->topicList.count() > 0)
    this->OnTopic(this->dataPtr->topicList.at(0));
  this->TopicListChanged();
}

/////////////////////////////////////////////////
QStringList NavSatMap::TopicList() const
{
  return this->dataPtr->topicList;
}

/////////////////////////////////////////////////
void NavSatMap::SetTopicList(const QStringList &_topicList)
{
  this->dataPtr->topicList = _topicList;
  this->TopicListChanged();
}
}  // namespace gz::gui::plugins

// Register this plugin
GZ_ADD_PLUGIN(gz::gui::plugins::NavSatMap,
              gz::gui::Plugin)
