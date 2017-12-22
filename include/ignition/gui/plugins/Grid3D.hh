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

#ifndef IGNITION_GUI_PLUGINS_GRID3D_HH_
#define IGNITION_GUI_PLUGINS_GRID3D_HH_

#include <memory>

#include "ignition/gui/qt.h"
#include "ignition/gui/Object3DPlugin.hh"

namespace ignition
{
namespace gui
{
namespace plugins
{
  class Grid3DPrivate;

  /// \brief Manages grids in an Ignition Rendering scene. This plugin can be
  /// used for:
  /// * Adding grids
  /// * Introspecting grids
  /// * Editing grids
  /// * Deleting grids
  ///
  /// ## Configuration
  ///
  /// * <engine> : Optional render engine name, defaults to 'ogre'.
  /// * <scene> : Optional scene name, defaults to 'scene'. If a scene with the
  ///             given name doesn't exist, the plugin is not initialized.
  /// * <auto_close> : Set to true so the plugin closes after grids given by
  ///                  <insert> tags are added to the scene.
  /// * <insert> : One grid will be inserted at startup for each <insert> tag.
  ///   * <cell_count> : Number of cells in the horizontal direction, defaults
  ///                    to 20.
  ///   * <vertical_cell_count> : Number of cells in the vertical direction,
  ///                             defaults to 0;
  ///   * <cell_length> : Length of each cell, defaults to 1.
  ///   * <pose> : Grid pose, defaults to the origin.
  ///   * <color> : Grid color, defaults to (0.7, 0.7, 0.7, 1.0)
  class Grid3D : public Object3DPlugin
  {
    Q_OBJECT

    /// \brief Constructor
    public: Grid3D();

    /// \brief Destructor
    public: virtual ~Grid3D();

    // Documentation inherited
    public: virtual void LoadConfig(const tinyxml2::XMLElement *_pluginElem)
        override;

    // Documentation inherited
    protected: virtual bool Delete(const rendering::ObjectPtr &_obj)
        override;

    // Documentation inherited
    protected: virtual void Add() override;

    /// \brief
    protected: virtual bool Change(const rendering::ObjectPtr &_obj,
        const std::string &_property, const QVariant &_value) override;

    // Documentation inherited
    protected: virtual void Refresh() override;

    /// \internal
    /// \brief Pointer to private data.
    private: std::unique_ptr<Grid3DPrivate> dataPtr;
  };
}
}
}

#endif
