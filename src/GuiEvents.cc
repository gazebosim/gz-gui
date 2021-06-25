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

namespace ignition
{
  namespace gui
  {
    namespace events
    {
      class KeyOnScenePrivate
      {
        /// \brief The key event that the user pressed within the scene.
        public: ignition::common::KeyEvent key;
      };
    }
  }
}

using namespace ignition;
using namespace gui;
using namespace events;

/////////////////////////////////////////////////
KeyReleaseOnScene::KeyReleaseOnScene(
  const ignition::common::KeyEvent &_key)
    : QEvent(kType), dataPtr(new KeyOnScenePrivate)
{
  this->dataPtr->key = _key;
}

/////////////////////////////////////////////////
KeyReleaseOnScene::~KeyReleaseOnScene()
{
}

/////////////////////////////////////////////////
ignition::common::KeyEvent KeyReleaseOnScene::Key() const
{
  return this->dataPtr->key;
}

/////////////////////////////////////////////////
KeyPressOnScene::KeyPressOnScene(
  const ignition::common::KeyEvent &_key)
    : QEvent(kType), dataPtr(new KeyOnScenePrivate)
{
  this->dataPtr->key = _key;
}

/////////////////////////////////////////////////
KeyPressOnScene::~KeyPressOnScene()
{
}

/////////////////////////////////////////////////
ignition::common::KeyEvent KeyPressOnScene::Key() const
{
  return this->dataPtr->key;
}
