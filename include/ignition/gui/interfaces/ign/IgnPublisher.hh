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

#ifndef IGN_GUI_INTERFACES_IGN_PUBLISHER_HH_
#define IGN_GUI_INTERFACES_IGN_PUBLISHER_HH_

#include "ignition/gui/System.hh"
#include "ignition/gui/interfaces/Publisher.hh"

namespace ignition
{
namespace gui
{
namespace interfaces
{
  /// \brief Example class
  class IGNITION_GUI_VISIBLE IgnPublisher : Publisher
  {
    /// \brief Constructor
    public: IgnPublisher();

    /// \brief Destructor
    public: ~IgnPublisher();
  };
}
}
}

#endif
