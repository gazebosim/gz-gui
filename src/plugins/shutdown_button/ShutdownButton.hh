/*
 * Copyright (C) 2021 Open Robotics
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

#ifndef IGNITION_GUI_PLUGINS_SHUTDOWNBUTTON_HH_
#define IGNITION_GUI_PLUGINS_SHUTDOWNBUTTON_HH_

#include <memory>

#ifdef _MSC_VER
#pragma warning(push, 0)
#endif
#include <ignition/msgs.hh>
#ifdef _MSC_VER
#pragma warning(pop)
#endif
#include <ignition/transport.hh>

#include "ignition/gui/Plugin.hh"

#ifndef _WIN32
#  define ShutdownButton_EXPORTS_API
#else
#  if (defined(ShutdownButton_EXPORTS))
#    define ShutdownButton_EXPORTS_API __declspec(dllexport)
#  else
#    define ShutdownButton_EXPORTS_API __declspec(dllimport)
#  endif
#endif

namespace ignition
{
namespace gui
{
namespace plugins
{
  class ShutdownButtonPrivate;

  /// \brief This plugin provides a shutdown button.
  ///
  /// ## Configuration
  ///
  /// * \<service\> : Service for server control, optional. If not present,
  ///               the plugin will use /server_control
  class ShutdownButton_EXPORTS_API ShutdownButton: public ignition::gui::Plugin
  {
    Q_OBJECT

    /// \brief Constructor
    public: ShutdownButton();

    /// \brief Destructor
    public: virtual ~ShutdownButton();

    // Documentation inherited
    public: void LoadConfig(const tinyxml2::XMLElement *_pluginElem);

    /// \brief Callback in Qt thread when close button is clicked.
    public slots: void OnStop();

    // Private data
    private: std::unique_ptr<ShutdownButtonPrivate> dataPtr;
  };
}
}
}

#endif
