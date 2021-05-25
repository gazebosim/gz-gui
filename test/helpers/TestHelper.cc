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

#include "TestHelper.hh"

namespace ignition
{
namespace gui
{
TestHelper::TestHelper()
{
  App()->findChild<MainWindow *>()->installEventFilter(this);
}

bool TestHelper::eventFilter(QObject *_obj, QEvent *_event)
{
  if (this->forwardEvent)
    this->forwardEvent(_event);

  // Standard event processing
  return QObject::eventFilter(_obj, _event);
}
}
}
