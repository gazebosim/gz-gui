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

#ifndef IGNITION_GUI_DISPLAY_PLUGINS_GRIDDISPLAY_HH_
#define IGNITION_GUI_DISPLAY_PLUGINS_GRIDDISPLAY_HH_

#include <memory>
#include <string>

#include "ignition/gui/DisplayPlugin.hh"
#include "ignition/gui/qt.h"

namespace ignition
{
namespace gui
{
namespace display_plugins
{
  class GridDisplayPrivate;

  /// \brief A single grid in an Ignition Rendering scene.
  ///
  /// This plugin can be used for adding and configuring a single grid.
  ///
  /// ## Configuration
  ///
  /// * \<engine\> : Optional render engine name, defaults to 'ogre'.
  /// * \<scene\> : Optional scene name, defaults to 'scene'. If a scene with
  ///               the given name doesn't exist, the plugin is not initialized.
  /// * \<cell_count\> : Number of cells in the horizontal direction, defaults
  ///                    to 20.
  /// * \<vertical_cell_count\> : Number of cells in the vertical direction,
  ///                             defaults to 0.
  /// * \<cell_length\> : Length of each cell, defaults to 1.
  /// * \<pose\> : Grid pose, defaults to the origin.
  /// * \<color\> : Grid color, defaults to (0.7, 0.7, 0.7, 1.0)
  class GridDisplay : public DisplayPlugin
  {
    Q_OBJECT

    /// \brief Constructor
    public: GridDisplay();

    /// \brief Destructor
    public: virtual ~GridDisplay();

    // Documentation inherited
    public: virtual void Initialize(const tinyxml2::XMLElement *_pluginElem)
        override;

    // Documentation inherited
    public: QWidget *CreateCustomProperties() const override;

    // Documentation inherited
    public: virtual tinyxml2::XMLElement * CustomConfig(
                tinyxml2::XMLDocument */*_doc*/) const override;

    /// \brief Called when a value changes on a widget
    /// \param[in] _value New value
    private slots: void OnChange(const QVariant &_value);

    /// \internal
    /// \brief Pointer to private data.
    private: std::unique_ptr<GridDisplayPrivate> dataPtr;
  };
}
}
}

#endif
