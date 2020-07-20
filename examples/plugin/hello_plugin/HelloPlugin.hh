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

#ifndef IGNITION_GUI_HELLOPLUGIN_HH_
#define IGNITION_GUI_HELLOPLUGIN_HH_

#include <string>

#include <ignition/gui/qt.h>
#include <ignition/gui/Plugin.hh>

namespace ignition
{
  namespace gui
  {
    class HelloPlugin : public Plugin
    {
      Q_OBJECT

      /// \brief Constructor
      public: HelloPlugin();

      /// \brief Destructor
      public: virtual ~HelloPlugin();

      /// \brief Called by Ignition GUI when plugin is instantiated.
      /// \param[in] _pluginElem XML configuration for this plugin.
      public: virtual void LoadConfig(const tinyxml2::XMLElement *_pluginElem)
          override;

      /// \brief Callback trigged when the button is pressed.
      protected slots: void OnButton();

      /// \brief Message to be printed when button is pressed.
      private: std::string message{"Hello, plugin!"};
    };
  }
}

#endif
