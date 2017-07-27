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

#include "ignition/gui/plugins/Requester.hh"

namespace ignition
{
namespace gui
{
namespace plugins
{
  class RequesterPrivate
  {
    /// \brief Holds request message type
    public: QLineEdit *reqTypeEdit;

    /// \brief Holds response message type
    public: QLineEdit *resTypeEdit;

    /// \brief Holds timeout
    public: QSpinBox *timeoutSpin;

    /// \brief Holds request
    public: QTextEdit *reqEdit;

    /// \brief Holds service
    public: QLineEdit *serviceEdit;
  };
}
}
}

using namespace ignition;
using namespace gui;
using namespace plugins;

/////////////////////////////////////////////////
Requester::Requester()
  : Plugin(), dataPtr(new RequesterPrivate)
{
}

/////////////////////////////////////////////////
Requester::~Requester()
{
}

/////////////////////////////////////////////////
void Requester::LoadConfig(const tinyxml2::XMLElement */*_pluginElem*/)
{
  if (this->title.empty())
    this->title = "Requester";

  // Populate with default values.
  this->dataPtr->reqTypeEdit = new QLineEdit("ignition.msgs.StringMsg");
  this->dataPtr->resTypeEdit = new QLineEdit("ignition.msgs.Int32");
  this->dataPtr->reqEdit = new QTextEdit("data: \"Hello\"");
  this->dataPtr->serviceEdit = new QLineEdit("/request");

  this->dataPtr->timeoutSpin = new QSpinBox();
  this->dataPtr->timeoutSpin->setMaximum(10000);
  this->dataPtr->timeoutSpin->setValue(1000);

  auto requestButton = new QPushButton("Request");
  this->connect(requestButton, SIGNAL(clicked()), this, SLOT(OnRequest()));

  auto layout = new QGridLayout();
  layout->addWidget(new QLabel("Request type: "), 0, 0);
  layout->addWidget(this->dataPtr->reqTypeEdit, 0, 1);
  layout->addWidget(new QLabel("Request: "), 1, 0);
  layout->addWidget(this->dataPtr->reqEdit, 1, 1);
  layout->addWidget(new QLabel("Response type: "), 2, 0);
  layout->addWidget(this->dataPtr->resTypeEdit, 2, 1);
  layout->addWidget(new QLabel("Service: "), 3, 0);
  layout->addWidget(this->dataPtr->serviceEdit, 3, 1);
  layout->addWidget(new QLabel("Timeout: "), 4, 0);
  layout->addWidget(this->dataPtr->timeoutSpin, 4, 1);
  layout->addWidget(requestButton, 5, 0, 1, 2);
  this->setLayout(layout);
}

/////////////////////////////////////////////////
void Requester::OnRequest()
{
  auto service = this->dataPtr->serviceEdit->text().toStdString();
  auto reqType = this->dataPtr->reqTypeEdit->text().toStdString();
  auto resType = this->dataPtr->resTypeEdit->text().toStdString();
  auto timeout = this->dataPtr->timeoutSpin->value();
  auto reqData = this->dataPtr->reqEdit->toPlainText().toStdString();

  // Create the request, and populate the field with _reqData
  auto req = ignition::msgs::Factory::New(reqType, reqData.c_str());
  if (!req)
  {
    ignerr << "Unable to create request of type[" << reqType << "] "
           << "with data[" << reqData << "].\n";
    return;
  }

  // Create the response.
  auto rep = ignition::msgs::Factory::New(resType);
  if (!rep)
  {
    ignerr << "Unable to create response of type[" << resType << "].\n";
    return;
  }

  // Create the node.
  ignition::transport::Node node;
  bool result;

  // Request the service.
  bool executed = node.Request(service, *req, timeout, *rep, result);
  if (executed)
  {
    if (result)
      std::cout << rep->DebugString() << std::endl;
    else
      std::cout << "Service call failed" << std::endl;
  }
  else
    std::cerr << "Service call timed out" << std::endl;
}

// Register this plugin
IGN_COMMON_REGISTER_SINGLE_PLUGIN(ignition::gui::plugins::Requester,
                                  ignition::gui::Plugin)
