/*
 * Copyright (C) 2021 Open Source Robotics Foundation
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

#include "ignition/gui/GuiEvents.hh"

class ignition::gui::events::LeftClickOnScene::Implementation
{
  /// \brief Mouse event
  public: common::MouseEvent mouse;
};

class ignition::gui::events::RightClickOnScene::Implementation
{
  /// \brief Mouse event
  public: common::MouseEvent mouse;
};

using namespace ignition;
using namespace gui;
using namespace events;

/////////////////////////////////////////////////
RightClickOnScene::RightClickOnScene(const common::MouseEvent &_mouse)
    : QEvent(kType), dataPtr(utils::MakeImpl<Implementation>())
{
  this->dataPtr->mouse = _mouse;
}

/////////////////////////////////////////////////
const common::MouseEvent &RightClickOnScene::Mouse() const
{
  return this->dataPtr->mouse;
}

/////////////////////////////////////////////////
LeftClickOnScene::LeftClickOnScene(const common::MouseEvent &_mouse)
    : QEvent(kType), dataPtr(utils::MakeImpl<Implementation>())
{
  this->dataPtr->mouse = _mouse;
}

/////////////////////////////////////////////////
const common::MouseEvent &LeftClickOnScene::Mouse() const
{
  return this->dataPtr->mouse;
}

