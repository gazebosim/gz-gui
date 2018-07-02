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

#include <ignition/common/Console.hh>
#include <ignition/common/PluginMacros.hh>
#include <ignition/common/Time.hh>

#include "ignition/gui/plugins/displays/RealtimeFactorDisplay.hh"

namespace ignition
{
namespace gui
{
namespace plugins
{
namespace displays
{
  class RealtimeFactorDisplayPrivate
  {
    /// \brief Message holding latest world statistics
    public: ignition::msgs::WorldStatistics msg;

    /// \brief Service to send world control requests
    public: std::string controlService;

    /// \brief Mutex to protect msg
    public: std::recursive_mutex mutex;

    /// \brief Communication node
    public: ignition::transport::Node node;

    /// \brief The multi step value
    public: unsigned int multiStep = 1u;
  };
}
}
}
}

using namespace ignition;
using namespace gui;
using namespace plugins;
using namespace displays;

/////////////////////////////////////////////////
RealtimeFactorDisplay::RealtimeFactorDisplay()
  : DisplayPlugin(), dataPtr(new RealtimeFactorDisplayPrivate)
{
}

/////////////////////////////////////////////////
RealtimeFactorDisplay::~RealtimeFactorDisplay()
{
}

/////////////////////////////////////////////////
void RealtimeFactorDisplay::Initialize(const tinyxml2::XMLElement *_pluginElem)
{
  if (this->title.empty())
    this->title = "Time panel";

}

/////////////////////////////////////////////////
QWidget* RealtimeFactorDisplay::CreateProperties()
{
  auto rtfLabel = new QLabel("Real Time Factor");
  //mainLayout->addWidget(rtfLabel, 2, 0, 1, 2);
  //mainLayout->setAlignment(rtfLabel, Qt::AlignRight);

  auto rtf = new QLabel("N/A");
  rtf->setObjectName("realTimeFactorLabel");
  rtf->setMinimumWidth(70);
  rtf->setAlignment(Qt::AlignRight);
  //mainLayout->addWidget(rtf, 2, 2);
  //mainLayout->setAlignment(rtf, Qt::AlignRight);
  this->connect(this, SIGNAL(SetRealTimeFactor(QString)), rtf,
      SLOT(setText(QString)));

  auto collapsible = new CollapsibleWidget("temp name");
  collapsible->AppendContent(rtfLabel);
  collapsible->AppendContent(rtf);

  return collapsible;

  /*
  // Spacers so widget doesn't lock the dock sizes
  auto spacerH = new QWidget();
  spacerH->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  mainLayout->addWidget(spacerH, 0, 4, 2, 1);

  auto spacerV = new QWidget();
  spacerV->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  mainLayout->addWidget(spacerV, 3, 0, 1, 5);

  this->setLayout(mainLayout);
  */
}

/////////////////////////////////////////////////
void RealtimeFactorDisplay::ProcessMsg()
{
  std::lock_guard<std::recursive_mutex> lock(this->dataPtr->mutex);

  ignition::common::Time time;

  if (this->dataPtr->msg.has_real_time_factor())
  {
    // RTF as a percentage.
    double rtf = this->dataPtr->msg.real_time_factor() * 100;

    this->SetRealTimeFactor(QString::number(rtf, 'f', 2) + " %");
  }
}

/////////////////////////////////////////////////
void RealtimeFactorDisplay::OnWorldStatsMsg(const ignition::msgs::WorldStatistics &_msg)
{
  std::lock_guard<std::recursive_mutex> lock(this->dataPtr->mutex);

  this->dataPtr->msg.CopyFrom(_msg);
  QMetaObject::invokeMethod(this, "ProcessMsg");
}

// Register this plugin
IGN_COMMON_REGISTER_SINGLE_PLUGIN(ignition::gui::plugins::RealtimeFactorDisplay,
                                  ignition::gui::Plugin)
