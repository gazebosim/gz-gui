/*
 * Copyright (C) 2018 Open Source Robotics Foundation
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

#ifndef IGNITION_GUI_PLUGINS_DISPLAYS_HH_
#define IGNITION_GUI_PLUGINS_DISPLAYS_HH_

#include <memory>

#include "ignition/gui/qt.h"
#include "ignition/gui/Plugin.hh"

namespace ignition
{
namespace gui
{
namespace plugins
{
  class DisplaysPrivate;

  /// \brief A widget for loading display plugins.
  ///
  ///This plugin can be used for adding and configuring display plugins.
  ///
  /// ## Configuration
  ///
  /// * \<engine\> : Optional render engine name, defaults to 'ogre'.
  /// * \<scene\> : Optional scene name, defaults to 'scene'. If a scene with
  ///               the given name doesn't exist, the plugin is not initialized.
  class Displays : public Plugin
  {
    Q_OBJECT

    /// \brief Constructor
    public: Displays();

    /// \brief Destructor
    public: virtual ~Displays();

    // Documentation inherited
    public: virtual void LoadConfig(const tinyxml2::XMLElement *_pluginElem)
        override;

    /// \internal
    /// \brief Pointer to private data.
    private: std::unique_ptr<DisplaysPrivate> dataPtr;
  };
}
}
}

#endif
