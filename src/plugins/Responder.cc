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

    /// \brief Holds service
    public: QLineEdit *serviceEdit;

    /// \brief Button to serve
    public: QPushButton *serveButton;

    public: std::unique_ptr<google::protobuf::Message> res;

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

  // Populate with default values.
  this->dataPtr->reqTypeEdit = new QLineEdit("ignition.msgs.StringMsg");
  this->dataPtr->resTypeEdit = new QLineEdit("ignition.msgs.Int32");
  this->dataPtr->resEdit = new QTextEdit("data: 123");
  this->dataPtr->serviceEdit = new QLineEdit("/request");

  this->dataPtr->serveButton = new QPushButton("Serve response");
  this->connect(this->dataPtr->serveButton, SIGNAL(clicked()), this,
      SLOT(OnServe()));

  auto layout = new QGridLayout();
  layout->addWidget(new QLabel("Request type: "), 0, 0);
  layout->addWidget(this->dataPtr->reqTypeEdit, 0, 1);
  layout->addWidget(new QLabel("Response type: "), 1, 0);
  layout->addWidget(this->dataPtr->resTypeEdit, 1, 1);
  layout->addWidget(new QLabel("Response: "), 2, 0);
  layout->addWidget(this->dataPtr->resEdit, 2, 1);
  layout->addWidget(new QLabel("Service: "), 3, 0);
  layout->addWidget(this->dataPtr->serviceEdit, 3, 1);
  layout->addWidget(this->dataPtr->serveButton, 4, 0, 1, 2);
  this->setLayout(layout);
}

/////////////////////////////////////////////////
void Responder::OnServe()
{
  auto service = this->dataPtr->serviceEdit->text().toStdString();
  auto reqType = this->dataPtr->reqTypeEdit->text().toStdString();
  auto resType = this->dataPtr->resTypeEdit->text().toStdString();
  auto resData = this->dataPtr->resEdit->toPlainText().toStdString();
  this->dataPtr->res = ignition::msgs::Factory::New(resType, resData.c_str());
  if (!this->dataPtr->res)
  {
    ignerr << "Unable to create request of type[" << resType << "] "
           << "with data[" << resData << "].\n";
    return;
  }

  if (reqType == "ignition.msgs.StringMsg" &&
      resType == "ignition.msgs.Int32")
  {

    std::function<void(const ignition::msgs::StringMsg &,
                             ignition::msgs::Int32 &, bool &)> cb =
                   [this](const ignition::msgs::StringMsg &/*_req*/,
                             ignition::msgs::Int32 &_res,
                   bool &_result)
    {
      _res.CopyFrom(*this->dataPtr->res);
      _result = true;
    };

    if (!this->dataPtr->node.Advertise(service, cb))
    {
      ignerr << "Error advertising service [" << service << "]" << std::endl;
    }
    else
    {
      this->dataPtr->serveButton->setText("Serving response");
      this->dataPtr->resEdit->setEnabled(false);
      this->dataPtr->reqTypeEdit->setEnabled(false);
      this->dataPtr->resTypeEdit->setEnabled(false);
      this->dataPtr->serviceEdit->setEnabled(false);
    }
  }
  else
  {
    ignerr << "Unhandled combination: Request[" << reqType << "] Response ["
           << resType << "]" << std::endl;
  }
}

// Register this plugin
IGN_COMMON_REGISTER_SINGLE_PLUGIN(ignition::gui::plugins::Responder,
                                  ignition::gui::Plugin)
