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

namespace gz
{
  namespace gui
  {
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
      public: void SetDefaultConfig(const std::string &_config);

      /// \brief Write dialog config
      /// \param[in] _path config path
      /// \param[in] _attribute XMLElement attribute name
      /// \param[in] _value XMLElement attribute value
      /// \return true if written to config file
      public: bool UpdateConfigAttribute(
        const std::string &_path, const std::string &_attribute,
        const bool _value) const;

      /// \brief Gets a config attribute value.
      /// It will return an empty string if the config file or the attribute
      /// don't exist.
      /// \param[in] _path config path
      /// \param[in] _attribute attribute name
      /// \return attribute value as string
      public: std::string ReadConfigAttribute(const std::string &_path,
        const std::string &_attribute) const;

      /// \internal
      /// \brief Private data pointer
      private: std::unique_ptr<DialogPrivate> dataPtr;
    };
  }
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif
