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

#ifndef IGNITION_GUI_INTERFACES_PUBLISHER_HH_
#define IGNITION_GUI_INTERFACES_PUBLISHER_HH_

#include <memory>
#include <ignition/common/Console.hh>
#include <ignition/msgs.hh>

#include "ignition/gui/System.hh"

namespace ignition
{
namespace gui
{
namespace interfaces
{
  /// \brief Example class
  class IGNITION_GUI_VISIBLE Publisher
  {
    /// \brief Constructor
    public: Publisher() = default;

    /// \brief Destructor
    public: virtual ~Publisher() = default;

    public: virtual bool Publish(msgs::Empty &_msg) = 0;
  };
}
}
}

#endif
