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

#include <algorithm>
#include <iostream>
#include <ignition/common/Console.hh>
#include <ignition/common/PluginMacros.hh>
#include <ignition/common/Time.hh>
#include <map>

#include "ignition/gui/plugins/TimePanel.hh"

namespace ignition
{
namespace gui
{
namespace plugins
{
  class TimePanelPrivate
  {
    public: QLabel *simTime;
    public: QLabel *realTime;
    public: ignition::msgs::Diagnostics msg;

    /// \brief Mutex to protect msg
    public: std::mutex mutex;

    /// \brief tools for setting up a subscriber
    public: ignition::transport::Node node;
  };
}
}
}

using namespace ignition;
using namespace gui;
using namespace plugins;

/////////////////////////////////////////////////
TimePanel::TimePanel()
  : Plugin(), dataPtr(new TimePanelPrivate)
{
  this->title = "Time panel";

  // Play button
  auto playButton = new QPushButton("Play");
  this->connect(playButton, SIGNAL(clicked()), this, SLOT(OnPlay()));
  this->connect(this, SIGNAL(Playing()), playButton, SLOT(show()));
  this->connect(this, SIGNAL(Paused()), playButton, SLOT(hide()));

  // FIXME: Assuming start playing
  playButton->hide();

  // Pause button
  auto pauseButton = new QPushButton("Pause");
  this->connect(pauseButton, SIGNAL(clicked()), this, SLOT(OnPause()));
  this->connect(this, SIGNAL(Playing()), pauseButton, SLOT(hide()));
  this->connect(this, SIGNAL(Paused()), pauseButton, SLOT(show()));

  // Times
  this->dataPtr->simTime = new QLabel("N/A");
  this->dataPtr->realTime = new QLabel("N/A");

  // Layout
  auto mainLayout = new QGridLayout();

  mainLayout->addWidget(playButton, 0, 0, 2, 1);
  mainLayout->addWidget(pauseButton, 0, 0, 2, 1);

  mainLayout->addWidget(new QLabel("Sim time"), 0, 1);
  mainLayout->addWidget(this->dataPtr->simTime, 0, 2);

  mainLayout->addWidget(new QLabel("Real time"), 1, 1);
  mainLayout->addWidget(this->dataPtr->realTime, 1, 2);

  mainLayout->setSizeConstraint(QLayout::SetFixedSize);
  this->setLayout(mainLayout);

  // Subscribe to diagnostics
  std::string topic = "diagnostics";
  if (!this->dataPtr->node.Subscribe(topic, &TimePanel::OnDiagnosticsMsg,
      this))
  {
    ignwarn << "Unable to subscribe to diagnostics" << std::endl;
  }
}

/////////////////////////////////////////////////
TimePanel::~TimePanel()
{
}

/////////////////////////////////////////////////
void TimePanel::ProcessMsg()
{
  std::lock_guard<std::mutex> lock(this->dataPtr->mutex);

  ignition::common::Time time;
  time.sec = this->dataPtr->msg.sim_time().sec();
  time.nsec = this->dataPtr->msg.sim_time().nsec();

  this->dataPtr->simTime->setText(QString::fromStdString(
        time.FormattedString()));

  time.sec = this->dataPtr->msg.real_time().sec();
  time.nsec = this->dataPtr->msg.real_time().nsec();

  this->dataPtr->realTime->setText(QString::fromStdString(
        time.FormattedString()));
}

/////////////////////////////////////////////////
void TimePanel::OnDiagnosticsMsg(const ignition::msgs::Diagnostics &_msg)
{
  std::lock_guard<std::mutex> lock(this->dataPtr->mutex);
  this->dataPtr->msg.CopyFrom(_msg);
  QMetaObject::invokeMethod(this, "ProcessMsg");
}

/////////////////////////////////////////////////
void TimePanel::OnPlay()
{
  std::function<void(const ignition::msgs::Empty &, const bool)> cb =
      [this](const ignition::msgs::Empty &_rep, const bool _result)
  {
    if (_result)
      QMetaObject::invokeMethod(this, "Paused");
    else
      QMetaObject::invokeMethod(this, "Playing");
  };

  ignition::msgs::WorldControl req;
  req.set_pause(false);
  this->dataPtr->node.Request("/world_control", req, cb);
}

/////////////////////////////////////////////////
void TimePanel::OnPause()
{
  std::function<void(const ignition::msgs::Empty &, const bool)> cb =
      [this](const ignition::msgs::Empty &_rep, const bool _result)
  {
    if (_result)
      QMetaObject::invokeMethod(this, "Playing");
    else
      QMetaObject::invokeMethod(this, "Paused");
  };

  ignition::msgs::WorldControl req;
  req.set_pause(true);
  this->dataPtr->node.Request("/world_control", req, cb);
}

// Register this plugin
IGN_COMMON_REGISTER_SINGLE_PLUGIN(ignition::gui::plugins::TimePanel,
                                  ignition::gui::Plugin);
