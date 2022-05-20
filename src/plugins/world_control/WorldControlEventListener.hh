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
#ifndef GZ_GUI_WORLDCONTROLEVENTLISTENER_HH_
#define GZ_GUI_WORLDCONTROLEVENTLISTENER_HH_

#include "gz/gui/Application.hh"
#include "gz/gui/Export.hh"
#include "gz/gui/GuiEvents.hh"
#include "gz/gui/MainWindow.hh"
#include "gz/gui/qt.h"

namespace gz
{
namespace gui
{
  /// \brief Helper class for testing listening to events emitted by the
  /// WorldControl plugin. This is used for testing the event behavior of
  /// the WorldControl plugin.
  class WorldControlEventListener : public QObject
  {
    Q_OBJECT

    /// \brief Constructor
    public: WorldControlEventListener();

    /// \brief Destructor
    public: virtual ~WorldControlEventListener() override;

    // Documentation inherited
    protected: bool eventFilter(QObject *_obj, QEvent *_event) override;

    /// \brief Whether a play event has been received (true) or not (false)
    public: bool listenedToPlay{false};

    /// \brief Whether a pause event has been received (true) or not (false)
    public: bool listenedToPause{false};

    /// \brief Whether a pause event has been received (true) or not (false)
    public: bool listenedToStep{false};
  };
}
}

#endif
