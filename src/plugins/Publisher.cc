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
#include <ignition/msgs.hh>
#include <ignition/transport/Node.hh>

#include "ignition/gui/Iface.hh"
#include "ignition/gui/plugins/Publisher.hh"

namespace ignition
{
namespace gui
{
namespace plugins
{
  class PublisherPrivate
  {
    /// \brief Holds message type
    public: QLineEdit *msgTypeEdit;

    /// \brief Holds message contents
    public: QTextEdit *msgEdit;

    /// \brief Holds topic
    public: QLineEdit *topicEdit;

    /// \brief Holds frequency
    public: QDoubleSpinBox *freqSpin;

    /// \brief Timer to keep publishing
    public: QTimer *timer;

    /// \brief Button to publish
    public: QPushButton *publishButton;

    /// \brief Node for communication
    public: ignition::transport::Node node;

    /// \brief Publisher
    public: ignition::transport::Node::Publisher pub;
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
  QQmlComponent component(qmlEngine(),
      QString(":/Publisher/Publisher.qml"));
  this->item = qobject_cast<QQuickItem *>(component.create());
  if (!this->item)
  {
    ignerr << "Null plugin QQuickItem!" << std::endl;
    return;
  }

  qmlEngine()->rootContext()->setContextProperty("Publisher", this);

  this->LoadConfig(nullptr);
}

/////////////////////////////////////////////////
Publisher::~Publisher()
{
}

/////////////////////////////////////////////////
QQuickItem *Publisher::Item() const
{
  return this->item;
}

/////////////////////////////////////////////////
void Publisher::LoadConfig(const tinyxml2::XMLElement *_pluginElem)
{
  if (this->title.empty())
    this->title = "Publisher";

  // Default values
  std::string msgType("ignition.msgs.StringMsg");
  std::string msg("data: \"Hello\"");
  std::string topic("/echo");
  double frequency(1.0);

  // Parameters from SDF
  if (_pluginElem)
  {
    if (auto typeElem = _pluginElem->FirstChildElement("message_type"))
      msgType = typeElem->GetText();

    if (auto msgElem = _pluginElem->FirstChildElement("message"))
      msg = msgElem->GetText();

    if (auto topicElem = _pluginElem->FirstChildElement("topic"))
      topic = topicElem->GetText();

    if (auto frequencyElem = _pluginElem->FirstChildElement("frequency"))
      frequencyElem->QueryDoubleText(&frequency);
  }

  // Populate with default values.
//  this->dataPtr->msgTypeEdit = new QLineEdit(QString::fromStdString(msgType));
//  this->dataPtr->msgTypeEdit->setObjectName("msgTypeEdit");
//  this->dataPtr->msgEdit = new QTextEdit(QString::fromStdString(msg));
//  this->dataPtr->msgEdit->setObjectName("msgEdit");
//  this->dataPtr->topicEdit = new QLineEdit(QString::fromStdString(topic));
//  this->dataPtr->topicEdit->setObjectName("topicEdit");
//
//  auto freqLabel = new QLabel("Frequency");
//  freqLabel->setToolTip("Set to zero to publish once");
//
//  this->dataPtr->freqSpin = new QDoubleSpinBox();
//  this->dataPtr->freqSpin->setObjectName("frequencySpinBox");
//  this->dataPtr->freqSpin->setMinimum(0);
//  this->dataPtr->freqSpin->setMaximum(1000);
//  this->dataPtr->freqSpin->setValue(frequency);
//
//  this->dataPtr->publishButton = new QPushButton("Publish");
//  this->dataPtr->publishButton->setObjectName("publishButton");
//  this->dataPtr->publishButton->setCheckable(true);
//  this->dataPtr->publishButton->setMinimumWidth(200);
//  this->connect(this->dataPtr->publishButton, SIGNAL(toggled(bool)), this,
//      SLOT(OnPublish(bool)));
//
//  auto layout = new QGridLayout();
//  layout->addWidget(new QLabel("Message type: "), 0, 0);
//  layout->addWidget(this->dataPtr->msgTypeEdit, 0, 1, 1, 3);
//  layout->addWidget(new QLabel("Message: "), 1, 0);
//  layout->addWidget(this->dataPtr->msgEdit, 1, 1, 1, 3);
//  layout->addWidget(new QLabel("Topic: "), 2, 0);
//  layout->addWidget(this->dataPtr->topicEdit, 2, 1, 1, 3);
//  layout->addWidget(freqLabel, 3, 0);
//  layout->addWidget(this->dataPtr->freqSpin, 3, 1);
//  layout->addWidget(new QLabel("Hz"), 3, 2);
//  layout->addWidget(this->dataPtr->publishButton, 3, 3);
//  this->setLayout(layout);

  this->dataPtr->timer = new QTimer(this);
}

/////////////////////////////////////////////////
void Publisher::OnPublish(const bool _checked)
{
  if (!_checked)
  {
//    this->dataPtr->publishButton->setText("Publish");
//    if (this->dataPtr->timer != nullptr)
//    {
//      this->dataPtr->timer->stop();
//      this->disconnect(this->dataPtr->timer, 0, 0, 0);
//    }
//    this->dataPtr->pub = ignition::transport::Node::Publisher();
    return;
  }

//  auto topic = this->dataPtr->topicEdit->text().toStdString();
//  auto msgType = this->dataPtr->msgTypeEdit->text().toStdString();
//  auto msgData = this->dataPtr->msgEdit->toPlainText().toStdString();
//  auto freq = this->dataPtr->freqSpin->value();

  std::string msgType("ignition.msgs.StringMsg");
  std::string msgData("data: \"Hello\"");
  std::string topic("/echo");
  double freq(1.0);

  // Check it's possible to create message
  auto msg = ignition::msgs::Factory::New(msgType, msgData);
  if (!msg || (msg->DebugString() == "" && msgData != ""))
  {
    ignerr << "Unable to create message of type[" << msgType << "] "
      << "with data[" << msgData << "].\n";
//    this->dataPtr->publishButton->setChecked(false);
    return;
  }

  // Advertise the topic
  this->dataPtr->pub = this->dataPtr->node.Advertise(topic, msgType);

  if (!this->dataPtr->pub)
  {
    ignerr << "Unable to publish on topic[" << topic << "] "
      << "with message type[" << msgType << "].\n";
//    this->dataPtr->publishButton->setChecked(false);
    return;
  }

  // Zero frequency, publish once
  if (freq < 0.00001)
  {
    this->dataPtr->pub.Publish(*msg);
//    this->dataPtr->publishButton->setChecked(false);
    return;
  }

//  this->dataPtr->publishButton->setText("Stop publishing");
  this->dataPtr->timer->setInterval(1000/freq);
  this->connect(this->dataPtr->timer, &QTimer::timeout, [=]()
  {
    auto newMsg = ignition::msgs::Factory::New(msgType, msgData);
    this->dataPtr->pub.Publish(*newMsg);
  });
  this->dataPtr->timer->start();
}

// Register this plugin
IGN_COMMON_REGISTER_SINGLE_PLUGIN(ignition::gui::plugins::Publisher,
                                  ignition::gui::Plugin)
