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

#include "ignition/gui/plugins/TopicEcho.hh"

namespace ignition
{
namespace gui
{
namespace plugins
{
  class TopicEchoPrivate
  {
    /// \brief Holds the topic name
    public: QLineEdit *topicEdit;

    /// \brief Button to start echoing a new topic
    public: QPushButton *echoButton;

    /// \brief A scolling list of text data.
    public: QListWidget *msgList;

    /// \brief Size of the text buffer. The size is the number of
    /// messages.
    public: int bufferSize = 10;

    /// \brief Flag used to pause message parsing.
    public: bool paused = false;

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
TopicEcho::TopicEcho()
  : Plugin(), dataPtr(new TopicEchoPrivate)
{
}

/////////////////////////////////////////////////
TopicEcho::~TopicEcho()
{
}

/////////////////////////////////////////////////
void TopicEcho::LoadConfig(const tinyxml2::XMLElement */*_pluginElem*/)
{
  if (this->title.empty())
    this->title = "Topic echo";

  this->dataPtr->echoButton = new QPushButton("Echo");
  this->connect(this->dataPtr->echoButton, SIGNAL(clicked()), this,
      SLOT(OnEcho()));

  this->dataPtr->topicEdit = new QLineEdit("/echo");
  this->connect(this->dataPtr->topicEdit, &QLineEdit::textChanged,
      [=](const QString &)
      {
        this->Stop();
        this->dataPtr->echoButton->setEnabled(true);
        this->dataPtr->echoButton->setText("Echo");
      });

  this->dataPtr->msgList = new QListWidget();
  this->dataPtr->msgList->setVerticalScrollMode(
      QAbstractItemView::ScrollPerPixel);

  auto bufferSpin = new QSpinBox();
  bufferSpin->setMinimum(1);
  bufferSpin->setValue(this->dataPtr->bufferSize);
  this->connect(bufferSpin, SIGNAL(valueChanged(int)), this,
      SLOT(OnBuffer(int)));

  auto pauseCheck = new QCheckBox();
  pauseCheck->setChecked(this->dataPtr->paused);
  this->connect(pauseCheck, SIGNAL(clicked(bool)), this, SLOT(OnPause(bool)));

  auto layout = new QGridLayout();
  layout->addWidget(new QLabel("Topic: "), 0, 0);
  layout->addWidget(this->dataPtr->topicEdit, 0, 1, 1, 2);
  layout->addWidget(this->dataPtr->echoButton, 0, 3);
  layout->addWidget(this->dataPtr->msgList, 1, 0, 1, 4);
  layout->addWidget(new QLabel("Buffer: "), 2, 0);
  layout->addWidget(bufferSpin, 2, 1);
  layout->addWidget(new QLabel("Pause: "), 2, 2);
  layout->addWidget(pauseCheck, 2, 3);
  this->setLayout(layout);

  this->connect(this, SIGNAL(AddMsg(QString)), this, SLOT(OnAddMsg(QString)),
          Qt::QueuedConnection);
}

/////////////////////////////////////////////////
void TopicEcho::Stop()
{
  std::lock_guard<std::mutex> lock(this->dataPtr->mutex);

  // Erase all previous messages
  this->dataPtr->msgList->clear();

  // Unsubscribe
  for (auto const &sub : this->dataPtr->node.SubscribedTopics())
    this->dataPtr->node.Unsubscribe(sub);
}

/////////////////////////////////////////////////
void TopicEcho::OnEcho()
{
  this->Stop();

  std::lock_guard<std::mutex> lock(this->dataPtr->mutex);

  // Subscribe to new topic
  auto topic = this->dataPtr->topicEdit->text().toStdString();
  if (!this->dataPtr->node.Subscribe(topic, &TopicEcho::OnMessage, this))
  {
    ignerr << "Invalid topic [" << topic << "]" << std::endl;
  }
  else
  {
    this->dataPtr->echoButton->setText("Echoing");
    this->dataPtr->echoButton->setEnabled(false);
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
  this->dataPtr->msgList->addItem(_msg);

  // Remove items if the list is too long.
  while (this->dataPtr->msgList->count() > this->dataPtr->bufferSize)
    delete this->dataPtr->msgList->takeItem(0);
}

/////////////////////////////////////////////////
void TopicEcho::OnPause(bool _value)
{
  std::lock_guard<std::mutex> lock(this->dataPtr->mutex);
  this->dataPtr->paused = _value;
}

/////////////////////////////////////////////////
void TopicEcho::OnBuffer(int _value)
{
  std::lock_guard<std::mutex> lock(this->dataPtr->mutex);

  this->dataPtr->bufferSize = _value;

  // Remove and item if the list is too long.
  while (this->dataPtr->msgList->count() > this->dataPtr->bufferSize)
    delete this->dataPtr->msgList->takeItem(0);
}

// Register this plugin
IGN_COMMON_REGISTER_SINGLE_PLUGIN(ignition::gui::plugins::TopicEcho,
                                  ignition::gui::Plugin)

