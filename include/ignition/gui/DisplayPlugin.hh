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

  /// \brief Base class for plugins that draw on the render scene.
  class DisplayPlugin : public Plugin
  {
    Q_OBJECT

    /// \brief Constructor
    public: DisplayPlugin();

    /// \brief Destructor
    public: virtual ~DisplayPlugin();

    // Documentation inherited
    public: virtual void LoadConfig(const tinyxml2::XMLElement *_pluginElem)
        override final;

    /// \brief Initialize the display plugin.
    public: virtual void Initialize(const tinyxml2::XMLElement *_pluginElem);

    /// \brief Create the widget for all of the display plugin's properties.
    public: QWidget *CreateProperties() const;

    /// \brief Create the widget for the properties specific to this plugin.
    public: virtual QWidget *CreateCustomProperties() const;

    /// \brief Returns the visual for the display plugin.
    protected: ignition::rendering::VisualPtr Visual() const;

    /// \brief Returns the scene for the display plugin.
    /// A weak pointer is returned because the scene may have been deleted
    /// since the display was created.
    /// /return Weak pointer to the scene.
    protected: std::weak_ptr<ignition::rendering::Scene> Scene() const;

    /// \brief Callback when the visibility checkbox is changed.
    /// \param[in] _value New visibility state (true if visible).
    private slots: virtual void OnVisibilityChange(const bool _value);

    /// \internal
    /// \brief Pointer to private data.
    private: std::unique_ptr<DisplayPluginPrivate> dataPtr;
  };
}
}

#endif
