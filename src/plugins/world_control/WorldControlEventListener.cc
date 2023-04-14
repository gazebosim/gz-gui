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

#include "WorldControlEventListener.hh"

using namespace ignition;
using namespace gui;

WorldControlEventListener::WorldControlEventListener()
{
  gz::gui::App()->findChild<
    gz::gui::MainWindow *>()->installEventFilter(this);
}

WorldControlEventListener::~WorldControlEventListener() = default;

bool WorldControlEventListener::eventFilter(QObject *_obj, QEvent *_event)
{
  if (_event->type() == gz::gui::events::WorldControl::kType)
  {
    auto worldControlEvent =
      reinterpret_cast<gui::events::WorldControl *>(_event);
    if (worldControlEvent)
    {
      this->listenedToPlay = !worldControlEvent->WorldControlInfo().pause();
      this->listenedToPause = worldControlEvent->WorldControlInfo().pause();
      this->listenedToStep =
        worldControlEvent->WorldControlInfo().multi_step() > 0u;
    }
  }

  // Standard event processing
  return QObject::eventFilter(_obj, _event);
}
