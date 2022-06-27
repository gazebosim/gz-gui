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

#ifndef IGNITION_GUI_DIALOG_HH_
#define IGNITION_GUI_DIALOG_HH_

#include <memory>
#include <tinyxml2.h>
#include <string>

#include "ignition/gui/qt.h"
#include "ignition/gui/Export.hh"

#ifdef _WIN32
// Disable warning C4251 which is triggered by
// std::unique_ptr
#pragma warning(push)
#pragma warning(disable: 4251)
#endif

namespace ignition
{
  namespace gui
  {
    class DialogPrivate;

    /// \brief Gui plugin
    class IGNITION_GUI_VISIBLE Dialog : public QObject
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

      /// \brief Set dilaog name
      public: void SetName(const std::string &_name);

      /// \brief Store dialog default config
      /// \param[in] _config XML config as string
      public: void SetDefaultConfig(const std::string &_config);

      /// \brief Write dialog config
      /// \param[in] _path config path
      /// \param[in] _attribute XMLElement attribute name
      /// \param[in] _attribute XMLElement attribute string value
      /// \return true if written to config file
      public: bool WriteAttribute(const std::string &_path,
        const std::string &_attribute, const std::string &_value) const;

      /// \brief Write dialog config
      /// \param[in] _path config path
      /// \param[in] _attribute XMLElement attribute name
      /// \param[in] _value XMLElement attribute boolean value
      /// \return true if written to config file
      public: bool WriteAttribute(const std::string &_path,
        const std::string &_attribute, const bool _value) const;

      /// \brief Gets a config attribute value, if not found in config
      /// \brief write the default in the config and get it.
      /// \brief creates config file if it doesn't exist.
      /// \param[in] _path config path
      /// \param[in] _attribute attribute name
      /// \return attribute value as string
      public: std::string ReadAttribute(const std::string &_path,
        const std::string  &_attribute) const;

      /// \brief Write config to path
      /// \param[in] _config config as char array
      /// \param[in] _path of the config
      /// \return true if written
      private: bool SaveConfig(const char *_config, const std::string &_path);

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
