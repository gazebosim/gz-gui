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
#include <ignition/transport/Node.hh>

#include "ignition/gui/plugins/Responder.hh"

namespace ignition
{
namespace gui
{
namespace plugins
{
  class ResponderPrivate
  {
    /// \brief Holds request message type
    public: QLineEdit *reqTypeEdit;

    /// \brief Holds response message type
    public: QLineEdit *resTypeEdit;

    /// \brief Holds response
    public: QTextEdit *resEdit;

    /// \brief Holds request
    public: QTextEdit *reqEdit;

    /// \brief Holds service
    public: QLineEdit *serviceEdit;

    /// \brief Button to serve
    public: QPushButton *serveButton;

    /// \brief Button to stop
    public: QPushButton *stopButton;

    /// \brief Keep track of latest service advertised so it's possible to
    /// unadvertise later
    public: std::string lastService;

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
Responder::Responder()
  : Plugin(), dataPtr(new ResponderPrivate)
{
}

/////////////////////////////////////////////////
Responder::~Responder()
{
}

/////////////////////////////////////////////////
void Responder::LoadConfig(const tinyxml2::XMLElement */*_pluginElem*/)
{
  if (this->title.empty())
    this->title = "Responder";

  // Response
  this->dataPtr->resTypeEdit = new QLineEdit("ignition.msgs.Int32");
  this->dataPtr->resEdit = new QTextEdit("data: 123");
  this->dataPtr->serviceEdit = new QLineEdit("/request");

  this->dataPtr->serveButton = new QPushButton("Serve response");
  this->connect(this->dataPtr->serveButton, SIGNAL(clicked()), this,
      SLOT(OnServe()));

  this->dataPtr->stopButton = new QPushButton("Stop serving response");
  this->dataPtr->stopButton->hide();
  this->connect(this->dataPtr->stopButton, SIGNAL(clicked()), this,
      SLOT(OnStop()));

  // Request
  this->dataPtr->reqTypeEdit = new QLineEdit("ignition.msgs.StringMsg");

  this->dataPtr->reqEdit = new QTextEdit("N/A");
  this->dataPtr->reqEdit->setEnabled(false);

  // Layout
  auto layout = new QGridLayout();
  layout->addWidget(new QLabel("Response"), 0, 0, 1, 2);
  layout->addWidget(this->dataPtr->resEdit, 1, 0, 1, 2);
  layout->addWidget(new QLabel("Request type: "), 2, 0);
  layout->addWidget(this->dataPtr->reqTypeEdit, 2, 1);
  layout->addWidget(new QLabel("Response type: "), 3, 0);
  layout->addWidget(this->dataPtr->resTypeEdit, 3, 1);
  layout->addWidget(new QLabel("Service: "), 4, 0);
  layout->addWidget(this->dataPtr->serviceEdit, 4, 1);
  layout->addWidget(this->dataPtr->serveButton, 5, 0, 1, 2);
  layout->addWidget(this->dataPtr->stopButton, 5, 0, 1, 2);
  layout->addWidget(new QLabel("Latest request "), 6, 0, 1, 2);
  layout->addWidget(this->dataPtr->reqEdit, 7, 0, 1, 2);
  this->setLayout(layout);
  this->setMinimumWidth(400);
}

/////////////////////////////////////////////////
void Responder::UpdateRequest(QString _req)
{
  // Thread danger?
  this->dataPtr->reqEdit->setPlainText(_req);
}

/////////////////////////////////////////////////
void Responder::OnServe()
{
  auto service = this->dataPtr->serviceEdit->text().toStdString();
  auto reqType = this->dataPtr->reqTypeEdit->text().toStdString();
  auto resType = this->dataPtr->resTypeEdit->text().toStdString();
  auto resData = this->dataPtr->resEdit->toPlainText().toStdString();
  this->res = ignition::msgs::Factory::New(resType, resData.c_str());

  if (!this->res)
  {
    ignerr << "Unable to create request of type[" << resType << "] "
           << "with data[" << resData << "].\n";
    return;
  }

  bool advertised = false;

  // It would be really awesome to generate the template from the strings.
  // Ign-transport needs to know the callback types at compile time, but maybe
  // there is a way...
  if (reqType == "ignition.msgs.StringMsg" &&
      resType == "ignition.msgs.StringMsg")
  {
    advertised = this->dataPtr->node.Advertise(service,
        &Responder::Callback<ignition::msgs::StringMsg,
                             ignition::msgs::StringMsg>, this);
  }
  else if (reqType == "ignition.msgs.StringMsg" &&
           resType == "ignition.msgs.Int32")
  {
    advertised = this->dataPtr->node.Advertise(service,
        &Responder::Callback<ignition::msgs::StringMsg,
                             ignition::msgs::Int32>, this);
  }
  else if (reqType == "ignition.msgs.Int32" &&
           resType == "ignition.msgs.StringMsg")
  {
    advertised = this->dataPtr->node.Advertise(service,
        &Responder::Callback<ignition::msgs::Int32,
                             ignition::msgs::StringMsg>, this);
  }
  else if (reqType == "ignition.msgs.Int32" &&
           resType == "ignition.msgs.Int32")
  {
    advertised = this->dataPtr->node.Advertise(service,
        &Responder::Callback<ignition::msgs::Int32,
                             ignition::msgs::Int32>, this);
  }
  else
  {
    ignerr << "Unhandled combination: Request[" << reqType << "] Response ["
           << resType << "]" << std::endl;
  }

  if (advertised)
  {
    this->dataPtr->serveButton->hide();
    this->dataPtr->stopButton->show();
    this->dataPtr->lastService = service;
  }

  this->dataPtr->resEdit->setEnabled(!advertised);
  this->dataPtr->reqTypeEdit->setEnabled(!advertised);
  this->dataPtr->resTypeEdit->setEnabled(!advertised);
  this->dataPtr->serviceEdit->setEnabled(!advertised);
}

/////////////////////////////////////////////////
void Responder::OnStop()
{
  // Unsubscribe
  // Ign-transport issue: no API to retrieve whole list of services by this
  // node?
  this->dataPtr->node.UnadvertiseSrv(this->dataPtr->lastService);

  this->dataPtr->serveButton->show();
  this->dataPtr->stopButton->hide();

  this->dataPtr->resEdit->setEnabled(true);
  this->dataPtr->reqTypeEdit->setEnabled(true);
  this->dataPtr->resTypeEdit->setEnabled(true);
  this->dataPtr->serviceEdit->setEnabled(true);
}

// Register this plugin
IGNITION_ADD_PLUGIN(ignition::gui::plugins::Responder,
                    ignition::gui::Plugin)
