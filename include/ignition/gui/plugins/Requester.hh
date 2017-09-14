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

#ifndef IGNITION_GUI_PLUGINS_REQUESTER_HH_
#define IGNITION_GUI_PLUGINS_REQUESTER_HH_

#include <memory>

#include "ignition/gui/qt.h"
#include "ignition/gui/Plugin.hh"

namespace ignition
{
namespace gui
{
namespace plugins
{
  class RequesterPrivate;

  /// \brief Widget which sends a custom Ignition transport service request.
  class Requester : public Plugin
  {
    Q_OBJECT

    /// \brief Constructor
    public: Requester();

    /// \brief Destructor
    public: virtual ~Requester();

    // Documentation inherited
    public: virtual void LoadConfig(const tinyxml2::XMLElement *_pluginElem);

    /// \brief Callback when request button is pressed.
    public slots: void OnRequest();

    /// \internal
    /// \brief Pointer to private data.
    private: std::unique_ptr<RequesterPrivate> dataPtr;
  };
}
}
}

#endif
