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
#include <gz/plugin/Register.hh>
#include <gz/transport/Node.hh>

#include "gz/gui/Application.hh"
#include "TopicEcho.hh"

namespace gz::gui::plugins
{
class TopicEcho::Implementation
{
  /// \brief Topic
  public: QString topic{"/echo"};

  /// \brief A list of text data.
  public: QStringListModel msgList {nullptr};

  /// \brief Size of the text buffer. The size is the number of
  /// messages.
  public: unsigned int buffer{10u};

  /// \brief Flag used to pause message parsing.
  public: bool paused{false};

  /// \brief Mutex to protect message buffer.
  public: std::mutex mutex;

  /// \brief Node for communication
  public: gz::transport::Node node;
};

/////////////////////////////////////////////////
TopicEcho::TopicEcho()
  : dataPtr(gz::utils::MakeUniqueImpl<Implementation>())
{
  // Connect model
  App()->Engine()->rootContext()->setContextProperty("TopicEchoMsgList",
      &this->dataPtr->msgList);
}

/////////////////////////////////////////////////
TopicEcho::~TopicEcho() = default;

/////////////////////////////////////////////////
void TopicEcho::LoadConfig(const tinyxml2::XMLElement * /*_pluginElem*/)
{
  if (this->title.empty())
    this->title = "Topic echo";

  this->connect(this, SIGNAL(AddMsg(QString)), this, SLOT(OnAddMsg(QString)),
          Qt::QueuedConnection);
}

/////////////////////////////////////////////////
void TopicEcho::Stop()
{
  std::lock_guard<std::mutex> lock(this->dataPtr->mutex);

  // Erase all previous messages
  this->dataPtr->msgList.removeRows(0,
      this->dataPtr->msgList.rowCount());

  // Unsubscribe
  for (auto const &sub : this->dataPtr->node.SubscribedTopics())
    this->dataPtr->node.Unsubscribe(sub);
}

/////////////////////////////////////////////////
void TopicEcho::OnEcho(const bool _checked)
{
  this->Stop();

  if (!_checked)
    return;

  std::lock_guard<std::mutex> lock(this->dataPtr->mutex);

  // Subscribe to new topic
  auto topic = this->dataPtr->topic.toStdString();
  if (!this->dataPtr->node.Subscribe(topic, &TopicEcho::OnMessage, this))
  {
    gzerr << "Invalid topic [" << topic << "]" << std::endl;
  }
}

/////////////////////////////////////////////////
void TopicEcho::OnMessage(const google::protobuf::Message &_msg)
{
  if (this->dataPtr->paused)
    return;

  std::lock_guard<std::mutex> lock(this->dataPtr->mutex);

  this->AddMsg(QString::fromStdString(_msg.DebugString()));
}

/////////////////////////////////////////////////
void TopicEcho::OnAddMsg(QString _msg)
{
  std::lock_guard<std::mutex> lock(this->dataPtr->mutex);

  // Append msg to list
  if (this->dataPtr->msgList.insertRow(this->dataPtr->msgList.rowCount()))
  {
    auto index = this->dataPtr->msgList.index(
        this->dataPtr->msgList.rowCount() - 1, 0);
    this->dataPtr->msgList.setData(index, _msg);
  }

  // Remove items if the list is too long.
  auto diff = this->dataPtr->msgList.rowCount() -
      this->dataPtr->buffer;
  this->dataPtr->msgList.removeRows(0, diff);
}

/////////////////////////////////////////////////
QString TopicEcho::Topic() const
{
  return this->dataPtr->topic;
}

/////////////////////////////////////////////////
void TopicEcho::SetTopic(const QString &_topic)
{
  this->dataPtr->topic = _topic;
  this->TopicChanged();
}

/////////////////////////////////////////////////
void TopicEcho::OnBuffer(const unsigned int _buffer)
{
  this->dataPtr->buffer = _buffer;
}

/////////////////////////////////////////////////
bool TopicEcho::Paused() const
{
  std::lock_guard<std::mutex> lock(this->dataPtr->mutex);
  return this->dataPtr->paused;
}

/////////////////////////////////////////////////
void TopicEcho::SetPaused(const bool &_paused)
{
  this->dataPtr->paused = _paused;
  this->PausedChanged();
}
}  // namespace gz::gui::plugins

// Register this plugin
GZ_ADD_PLUGIN(gz::gui::plugins::TopicEcho,
              gz::gui::Plugin)
