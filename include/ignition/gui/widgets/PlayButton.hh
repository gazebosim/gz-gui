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

#ifndef IGNITION_GUI_WIDGETS_PLAYBUTTON_HH_
#define IGNITION_GUI_WIDGETS_PLAYBUTTON_HH_

#include <memory>

#include "ignition/gui/qt.h"
#include "ignition/gui/System.hh"

namespace ignition
{
namespace gui
{
namespace widgets
{
  class PlayButtonPrivate;

  class IGNITION_GUI_VISIBLE PlayButton : public QWidget
  {
    Q_OBJECT

    /// \brief Constructor
    /// \param[in] _parent Parent widget, commonly a PlayButtonPanel.
    public: PlayButton(QWidget *_parent = 0);

    /// \brief Destructor
    public: virtual ~PlayButton();

    /// \brief Callback when button is toggled.
    /// \param[in] _checked True for play, false for pause.
    public slots: void OnButtonToggled(const bool _checked);

    /// \internal
    /// \brief Pointer to private data.
    private: std::unique_ptr<PlayButtonPrivate> dataPtr;
  };
}
}
}

#endif
