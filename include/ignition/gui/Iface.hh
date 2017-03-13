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
#ifndef IGNITION_GUI_IFACE_HH_
#define IGNITION_GUI_IFACE_HH_

#include "ignition/gui/System.hh"

namespace ignition
{
  namespace gui
  {
    /// \brief Load the graphical interface.
    /// \return True on success.
    IGNITION_GUI_VISIBLE
    bool load();

    /// \brief Run the graphical interface.
    /// \param[in] _argc Arguments count.
    /// \param[in] _argv Arguments values.
    /// \return True on success.
    IGNITION_GUI_VISIBLE
    bool run(int _argc, char **_argv);

    /// \brief Stop the graphical interface.
    IGNITION_GUI_VISIBLE
    void stop();
  }
}
#endif
