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

#include <ignition/msgs.hh>

#include "ignition/gui/qt.h"
#include "ignition/gui/interfaces/ign/IgnPublisher.hh"
#include "ignition/gui/widgets/PlayButton.hh"

namespace ignition
{
namespace gui
{
namespace widgets
{
  /// \internal
  /// \brief Private data for the PlayButton class
  class PlayButtonPrivate
  {
    /// \brief Play/pause button
    public: QPushButton *playButton;

    public: interfaces::IgnPublisher *playPub;
  };
}
}
}

using namespace ignition;
using namespace gui;
using namespace widgets;

/////////////////////////////////////////////////
PlayButton::PlayButton(QWidget *_parent)
  : QWidget(_parent), dataPtr(new PlayButtonPrivate)
{
  this->dataPtr->playButton = new QPushButton("Play");
  this->dataPtr->playButton->setCheckable(true);
  this->connect(this->dataPtr->playButton, SIGNAL(toggled(const bool)), this,
      SLOT(OnButtonToggled(const bool)));

  auto mainLayout = new QVBoxLayout();
  mainLayout->addWidget(this->dataPtr->playButton);
  this->setLayout(mainLayout);

  this->dataPtr->playPub = new interfaces::IgnPublisher();
}

/////////////////////////////////////////////////
PlayButton::~PlayButton()
{
}

/////////////////////////////////////////////////
void PlayButton::OnButtonToggled(const bool _checked)
{
  ignition::msgs::Boolean msg;
  msg.set_data(_checked);
  this->dataPtr->playPub->Publish(msg);
}

