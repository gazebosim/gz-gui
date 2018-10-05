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
#include <ignition/plugin/Register.hh>
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

    /// \brief Holds response
    public: QTextEdit *resEdit;

    /// \brief Holds success status
    public: QLabel *successLabel;

    /// \brief Holds timeout status
    public: QLabel *timeoutLabel;

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

  // Request
  this->dataPtr->reqTypeEdit = new QLineEdit("ignition.msgs.StringMsg");
  this->dataPtr->resTypeEdit = new QLineEdit("ignition.msgs.Int32");
  this->dataPtr->reqEdit = new QTextEdit("data: \"Hello\"");
  this->dataPtr->serviceEdit = new QLineEdit("/request");

  this->dataPtr->timeoutSpin = new QSpinBox();
  this->dataPtr->timeoutSpin->setMaximum(10000);
  this->dataPtr->timeoutSpin->setValue(1000);

  auto requestButton = new QPushButton("Request");
  this->connect(requestButton, SIGNAL(clicked()), this, SLOT(OnRequest()));

  // Response
  this->dataPtr->resEdit = new QTextEdit("N/A");
  this->dataPtr->resEdit->setEnabled(false);

  this->dataPtr->successLabel = new QLabel("N/A");

  this->dataPtr->timeoutLabel = new QLabel("No");

  // Layout
  auto layout = new QGridLayout();
  layout->addWidget(new QLabel("Request"), 0, 0, 1, 2);
  layout->addWidget(this->dataPtr->reqEdit, 1, 0, 1, 2);
  layout->addWidget(new QLabel("Request type: "), 2, 0);
  layout->addWidget(this->dataPtr->reqTypeEdit, 2, 1);
  layout->addWidget(new QLabel("Response type: "), 3, 0);
  layout->addWidget(this->dataPtr->resTypeEdit, 3, 1);
  layout->addWidget(new QLabel("Service: "), 4, 0);
  layout->addWidget(this->dataPtr->serviceEdit, 4, 1);
  layout->addWidget(new QLabel("Timeout: "), 5, 0);
  layout->addWidget(this->dataPtr->timeoutSpin, 5, 1);
  layout->addWidget(requestButton, 6, 0, 1, 2);
  layout->addWidget(new QLabel("Latest response "), 7, 0, 1, 2);
  layout->addWidget(this->dataPtr->resEdit, 8, 0, 1, 2);
  layout->addWidget(new QLabel("Success: "), 9, 0);
  layout->addWidget(this->dataPtr->successLabel, 9, 1);
  layout->addWidget(new QLabel("Timed out: "), 10, 0);
  layout->addWidget(this->dataPtr->timeoutLabel, 10, 1);
  this->setLayout(layout);
  this->setMinimumWidth(400);
}

/////////////////////////////////////////////////
void Requester::OnRequest()
{
  auto service = this->dataPtr->serviceEdit->text().toStdString();
  auto reqType = this->dataPtr->reqTypeEdit->text().toStdString();
  auto resType = this->dataPtr->resTypeEdit->text().toStdString();
  auto timeout = this->dataPtr->timeoutSpin->value();
  auto reqData = this->dataPtr->reqEdit->toPlainText().toStdString();

  // Create the request
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

  // Update GUI before we block
  this->dataPtr->successLabel->setText("N/A");
  this->dataPtr->resEdit->setPlainText("");
  this->dataPtr->timeoutLabel->setText("Waiting...");
  QApplication::processEvents();

  // Request the service, this will block
  ignition::transport::Node node;
  bool success;
  bool executed = node.Request(service, *req, timeout, *rep, success);
  if (executed)
  {
    QString successStr = success ? "Yes" : "No";
    this->dataPtr->successLabel->setText(successStr);
    this->dataPtr->timeoutLabel->setText("No");

    if (success)
    {
      this->dataPtr->resEdit->setPlainText(QString::fromStdString(
          rep->DebugString()));
    }
  }
  else
  {
    this->dataPtr->timeoutLabel->setText("Yes");
  }
}

// Register this plugin
IGNITION_ADD_PLUGIN(ignition::gui::plugins::Requester,
                    ignition::gui::Plugin)
