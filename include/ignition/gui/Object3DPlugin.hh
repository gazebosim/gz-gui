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

#ifndef IGNITION_GUI_OBJECT3DPLUGIN_HH_
#define IGNITION_GUI_OBJECT3DPLUGIN_HH_

#include <memory>
#include <string>
#include <vector>
#include <ignition/rendering/RenderTypes.hh>

#include "ignition/gui/qt.h"
#include "ignition/gui/Plugin.hh"

namespace ignition
{
namespace gui
{
  class Object3DPluginPrivate;
  class PropertyWidget;

  /// \brief Base class for plugins which provide a CRUD
  /// (create-read-update-delete) interface to 3D rendering objects.
  class Object3DPlugin : public Plugin
  {
    Q_OBJECT

    /// \brief Constructor
    public: Object3DPlugin();

    /// \brief Destructor
    public: virtual ~Object3DPlugin();

    // Documentation inherited
    public: virtual void LoadConfig(const tinyxml2::XMLElement *_pluginElem)
        override;

    /// \brief Delete the passed object.
    /// \param[in] _obj Object to be deleted.
    /// \result True if successful.
    protected: virtual bool Delete(const rendering::ObjectPtr &_obj) = 0;

    /// \brief Add an object with default parameters to the scene.
    /// \result True if successful.
    protected: virtual bool Add() = 0;

    /// \brief Handle a user request to change a property of a given object.
    /// \param[in] _obj Object to be changed.
    /// \param[in] _property Name of property to be changed.
    /// \param[in] _value New value for the property.
    /// \result True if successful.
    protected: virtual bool Change(const rendering::ObjectPtr &_obj,
        const std::string &_property, const QVariant &_value) = 0;

    /// \brief Refresh the widgets listing all objects in the scene.
    protected: virtual void Refresh() = 0;

    /// \brief Append an object to the internal list of objects.
    /// \param[in] _obj Object to be added.
    /// \param[in] _props Vector of property widgets.
    protected: void AppendObj(const rendering::ObjectPtr &_obj,
        const std::vector<PropertyWidget *> _props);

    /// \brief Called when a value changes on a widget
    /// \param[in] _value New value
    private slots: void OnChange(const QVariant &_value);

    /// \brief Callback when a delete button is pressed.
    protected slots: void OnDelete();

    /// \brief Callback when the add button is pressed.
    protected slots: void OnAdd();

    /// \brief Callback when the refresh button is pressed.
    protected slots: void OnRefresh();

    /// \brief We keep a pointer to the engine and rely on it not being
    /// destroyed, since it is a singleton.
    protected: rendering::RenderEngine *engine;

    /// \brief We keep the scene name rather than a shared pointer because we
    /// don't want to share ownership.
    protected: std::string sceneName{"scene"};

    /// \brief Keep track of objs on the scene
    protected: std::vector<rendering::ObjectPtr> objs;

    /// \brief Obj name singular
    protected: std::string typeSingular;

    /// \internal
    /// \brief Pointer to private data.
    private: std::unique_ptr<Object3DPluginPrivate> dataPtr;
  };
}
}

#endif
