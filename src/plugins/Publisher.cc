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
#include <ignition/transport/ign.hh>

#include "ignition/gui/plugins/Publisher.hh"

namespace ignition
{
namespace gui
{
namespace plugins
{
  class PublisherPrivate
  {
    public: QLineEdit *msgTypeEdit;
    public: QTextEdit *msgEdit;
    public: QLineEdit *topicEdit;
  };
}
}
}

using namespace ignition;
using namespace gui;
using namespace plugins;

/////////////////////////////////////////////////
Publisher::Publisher()
  : Plugin(), dataPtr(new PublisherPrivate)
{
}

/////////////////////////////////////////////////
Publisher::~Publisher()
{
}

/////////////////////////////////////////////////
void Publisher::LoadConfig(const tinyxml2::XMLElement *_pluginElem)
{
  if (this->title.empty())
    this->title = "Publisher";

  // Populate with default values. These can be checked with:
  // ign topic -e -t /echo
  this->dataPtr->msgTypeEdit = new QLineEdit("ignition.msgs.StringMsg");
  this->dataPtr->msgEdit = new QTextEdit("data: \"Hello\"");
  this->dataPtr->topicEdit = new QLineEdit("/echo");

  auto publishButton = new QPushButton("Publish");
  this->connect(publishButton, SIGNAL(clicked()), this, SLOT(OnPublish()));

  auto layout = new QGridLayout();
  layout->addWidget(new QLabel("Message type: "), 0, 0);
  layout->addWidget(this->dataPtr->msgTypeEdit, 0, 1);
  layout->addWidget(new QLabel("Message: "), 1, 0);
  layout->addWidget(this->dataPtr->msgEdit, 1, 1);
  layout->addWidget(new QLabel("Topic: "), 2, 0);
  layout->addWidget(this->dataPtr->topicEdit, 2, 1);
  layout->addWidget(publishButton);
  this->setLayout(layout);
}

/////////////////////////////////////////////////
void Publisher::OnPublish()
{
  auto topic = this->dataPtr->topicEdit->text().toStdString();
  auto msgType = this->dataPtr->msgTypeEdit->text().toStdString();
  auto msg = this->dataPtr->msgEdit->toPlainText().toStdString();

  cmdTopicPub(topic.c_str(), msgType.c_str(), msg.c_str());
}

// Register this plugin
IGN_COMMON_REGISTER_SINGLE_PLUGIN(ignition::gui::plugins::Publisher,
                                  ignition::gui::Plugin)
