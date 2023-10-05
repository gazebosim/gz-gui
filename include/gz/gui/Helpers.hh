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
#ifndef GZ_GUI_HELPERS_HH_
#define GZ_GUI_HELPERS_HH_

#include <string>

#include "gz/gui/Enums.hh"
#include "gz/gui/Export.hh"

namespace gz::gui
{
/// \brief Create a human readable key, capitalizing the first letter
/// and removing characters like "_".
/// \param[in] _key Non-human-readable key.
/// \return Human-readable key.
GZ_GUI_VISIBLE
std::string humanReadable(const std::string &_key);

/// \brief Returns the unit for a given key. For example, the key "mass"
/// returns "kg".
/// \param[in] _key The key.
/// \param[in] _type In case the key may have more than one type, the type
/// must be given too. For example, a prismatic joint will have different
/// units from a revolute joint.
/// \return The unit.
GZ_GUI_VISIBLE
std::string unitFromKey(const std::string &_key,
                        const std::string &_type = "");

/// \brief Returns the range for a given key. For example, the key
/// "transparency" returns min == 0, max == 1.
/// \param[in] _key The key.
/// \param[out] _min The minimum value.
/// \param[out] _max The maximum value.
GZ_GUI_VISIBLE
void rangeFromKey(const std::string &_key, double &_min, double &_max);

/// \brief Returns the string type for a given key. For example, the key
/// "innerxml" has a PLAIN_TEXT type while "name" is a LINE.
/// \param[in] _key The key.
/// \return The string type.
GZ_GUI_VISIBLE
StringType stringTypeFromKey(const std::string &_key);

/// \brief Generates a path for a file which doesn't collide with existing
/// files, by appending numbers to it (i.e. (0), (1), ...)
/// \param[in] _pathAndName Full absolute path and file name up to the
/// file extension.
/// \param[in] _extension File extension, such as "pdf".
/// \return Full path, with name and extension, which doesn't collide with
/// existing files
GZ_GUI_VISIBLE
std::string uniqueFilePath(const std::string &_pathAndName,
                           const std::string &_extension);

/// \brief The main window's "worldNames" property may be filled with a list
/// of the names of all worlds currently loaded. This information can be
/// used by plugins to choose which world to work with.
/// This helper function provides a handy access to the world names list.
/// \return List of world names, as stored in the `MainWindow`'s
/// "worldNames" property.
GZ_GUI_VISIBLE
QStringList worldNames();

/// \brief The main window's "renderEngine" property may be filled with
/// a string of the render engine gui name. This information can be
/// used by plugins to set the GUI render engine
/// This helper function provides a handy access to the render engine GUI
/// name
/// \return Name of render engine used on the GUI, as stored in the
/// `MainWindow`'s "renderEngine" property.
GZ_GUI_VISIBLE
std::string renderEngineName();

/// \brief Just like renderEngineName(), but valid entries are:
///  - opengl (default)
///  - metal (Apple only)
///  - vulkan
/// \return Name of API backend used on the GUI, as stored in the
/// `MainWindow`'s "renderEngineBackendApiName" property.
GZ_GUI_VISIBLE
std::string renderEngineBackendApiName();

/// \brief Import path for gz-gui QML modules added to the Qt resource
/// system. This helper function returns the QRC resource path where custom
/// Gazebo QML modules can be imported from. To import a Gazebo QML
/// module, add this path to the QML engine's import path list before
/// attempting to load a QML file that imports Gazebo QML modules.
/// \return Resousrce path prefix as a string
GZ_GUI_VISIBLE
const QString qmlQrcImportPath();

/// \brief Returns the first element on a QList which matches the given
/// property.
/// \param[in] _list The list to search through.
/// \param[in] _key The property key value.
/// \param[in] _value The property value.
/// \return The first matching element.
template<class T>
T findFirstByProperty(const QList<T> _list, const char *_key,
    QVariant _value)
{
  if (nullptr == _key)
    return nullptr;

  for (const auto &w : _list)
  {
    if (nullptr == w)
      continue;

    if (w->property(_key) == _value)
      return w;
  }
  return nullptr;
}
}  // namespace gz::gui
#endif  // GZ_GUI_HELPERS_HH_
