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

#ifndef GZ_GUI_DIALOG_HH_
#define GZ_GUI_DIALOG_HH_

#include <memory>
#include <string>

#include "gz/gui/qt.h"
#include "gz/gui/Export.hh"

#ifdef _WIN32
// Disable warning C4251 which is triggered by
// std::unique_ptr
#pragma warning(push)
#pragma warning(disable: 4251)
#endif

namespace gz::gui
{
/// Forward declarations
class DialogPrivate;

/// \brief Gui plugin
class GZ_GUI_VISIBLE Dialog : public QObject
{
  Q_OBJECT

  /// \brief Constructor
  public: Dialog();

  /// \brief Destructor
  public: virtual ~Dialog();

  /// \brief Get the QtQuick window created by this object
  /// \return Pointer to the QtQuick window
  public: QQuickWindow *QuickWindow() const;

  /// \brief Get the root quick item of this window
  /// \return Pointer to the item
  public: QQuickItem *RootItem() const;

  /// \brief Store dialog default config
  /// \param[in] _config XML config as string
  /// \deprecated Introduce deprecation warnings on v7.
  public: void SetDefaultConfig(const std::string &_config);

  /// \brief Update an attribute on an XML file. The attribute belongs to
  /// a `<dialog>` element that has a `name` attrbute matching this dialog's
  /// name, i.e.
  ///
  /// `<dialog name="dialog_name" attribute="value"/>`
  ///
  /// If a dialog element with this dialog's name doesn't exist yet, one
  /// will be created.
  ///
  /// \param[in] _path File path. File must already exist, this function
  /// will not create a new file.
  /// \param[in] _attribute XMLElement attribute name
  /// \param[in] _value XMLElement attribute value
  /// \return True if written to config file
  public: bool UpdateConfigAttribute(
    const std::string &_path, const std::string &_attribute,
    const bool _value) const;

  /// \brief Gets an attribute value from an XML file. The attribute belongs
  /// to a `<dialog>` element that has a `name` attribute matching this
  /// dialog's name.
  /// It will return an empty string if the file or the attribute
  /// don't exist.
  /// \param[in] _path File path
  /// \param[in] _attribute attribute name
  /// \return Attribute value as string
  public: std::string ReadConfigAttribute(const std::string &_path,
    const std::string &_attribute) const;

  /// \internal
  /// \brief Private data pointer
  private: std::unique_ptr<DialogPrivate> dataPtr;
};
}  // namespace gz::gui

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif  // GZ_GUI_DIALOG_HH_
