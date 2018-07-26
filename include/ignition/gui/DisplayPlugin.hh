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
#include <string>

#include <ignition/common/PluginMacros.hh>
#include <ignition/rendering.hh>

#include "ignition/gui/qt.h"
#include "ignition/gui/Plugin.hh"

namespace ignition
{
namespace gui
{
  class DisplayPluginPrivate;

  /// \brief Base class for plugins that draw on the render scene.
  class DisplayPlugin : public QWidget
  {
    Q_OBJECT

    public: IGN_COMMON_SPECIALIZE_INTERFACE(ignition::gui::DisplayPlugin)

    /// \brief Constructor
    public: DisplayPlugin();

    /// \brief Destructor
    public: virtual ~DisplayPlugin();

    /// \brief Load the display plugin with a configuration file.
    /// This loads the default parameters and then calls Initialize(), which
    /// should be overridden to load custom parameters.
    ///
    /// A Visual is set up. Other Visuals may be attached to this Visual
    /// Initialize() so that their visibility is automatically tied to that of
    /// the root Visual.
    ///
    /// Called when a display plugin is first created.
    /// This function should not be blocking.
    ///
    /// \sa Initialize
    /// \sa Visual
    /// \param[in] _pluginElem Element containing configuration
    public: void Load(const tinyxml2::XMLElement *_pluginElem);

    /// \brief Initialize the display plugin.
    public: virtual void Initialize(const tinyxml2::XMLElement *_pluginElem);

    /// \brief Create the widget for all of the display plugin's properties.
    public: QWidget *CreateProperties() const;

    /// \brief Create the widget for the properties specific to this plugin.
    public: virtual QWidget *CreateCustomProperties() const;

    /// \brief Get title
    /// \return Display plugin title.
    public: virtual std::string Title() const {return this->title;}

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

    /// \brief Title of display plugin.
    protected: std::string title = "";
  };
}
}

#endif
