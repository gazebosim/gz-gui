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

#ifndef IGNITION_GUI_DISPLAY_PLUGIN_HH_
#define IGNITION_GUI_DISPLAY_PLUGIN_HH_

#include <memory>

#include <ignition/rendering.hh>

#include "ignition/gui/qt.h"
#include "ignition/gui/Plugin.hh"

namespace ignition
{
namespace gui
{
  class DisplayPluginPrivate;

  class DisplayPlugin : public Plugin
  {
    Q_OBJECT

    /// \brief Constructor
    public: DisplayPlugin();

    /// \brief Destructor
    public: virtual ~DisplayPlugin();

    // Documentation inherited
    public: virtual void LoadConfig(const tinyxml2::XMLElement *_pluginElem)
        override;

    /// \brief Initialize the display plugin.
    public: virtual void Initialize(const tinyxml2::XMLElement *_pluginElem);

    /// \brief Create the widget for standard display plugins' properties.
    public: virtual QWidget* CreateStandardProperties();

    /// \brief Create the widget for the plugin's properties.
    public: virtual QWidget* CreateProperties();

    /// \brief Returns the visual for the display plugin.
    protected: ignition::rendering::VisualPtr Visual();

    /// \brief Returns the visual for the display plugin.
    protected: ignition::rendering::ScenePtr Scene();

    /// \brief Callback when the visibility checkbox is changed.
    private slots: virtual void OnVisibilityChange(bool _value);

    /// \internal
    /// \brief Pointer to private data.
    private: std::unique_ptr<DisplayPluginPrivate> dataPtr;
  };
}
}

#endif
