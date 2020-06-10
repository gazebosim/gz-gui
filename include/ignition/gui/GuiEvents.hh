/*
 * Copyright (C) 2020 Open Source Robotics Foundation
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
#ifndef IGNITION_GUI_GUIEVENTS_HH_
#define IGNITION_GUI_GUIEVENTS_HH_

#include <QEvent>
#include <utility>
#include <vector>
#include <ignition/math/Vector3.hh>

namespace ignition
{
  namespace gui
  {
    /// \brief Namespace for all events.
    namespace events
    {
      /// User defined events should start from QEvent::MaxUser and
      /// count down to avoid collision with ign-gazebo events

      /// \brief Event called in the render thread of a 3D scene.
      /// It's safe to make rendering calls in this event's callback.
      class Render : public QEvent
      {
        public: Render()
            : QEvent(kType)
        {
        }
        /// \brief Unique type for this event.
        static const QEvent::Type kType = QEvent::Type(QEvent::MaxUser);
      };
    }
  }
}

#endif  // IGNITION_GUI_GUIEVENTS_HH_
