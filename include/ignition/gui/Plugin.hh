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
#ifndef IGNITION_GUI_PLUGIN_HH_
#define IGNITION_GUI_PLUGIN_HH_

#include <tinyxml2.h>
#include <memory>
#include <string>

#include <ignition/common/PluginMacros.hh>

#include "ignition/gui/qt.h"
#include "ignition/gui/Export.hh"

namespace ignition
{
  namespace gui
  {
    class PluginPrivate;

    /// \brief Gui plugin
    class IGNITION_GUI_VISIBLE Plugin : public QObject
    {
      Q_OBJECT

      public: IGN_COMMON_SPECIALIZE_INTERFACE(ignition::gui::Plugin)

      /// \brief Constructor
      public: Plugin();

      /// \brief Destructor
      public: virtual ~Plugin();

      /// \brief Load the plugin with a configuration file.
      /// This loads the default parameters and then calls LoadConfig(), which
      /// should be overridden to load custom parameters.
      ///
      /// Called when a plugin is first created.
      /// This function should not be blocking.
      ///
      /// \sa Load
      /// \param[in] _pluginElem Element containing configuration
      public: void Load(const tinyxml2::XMLElement *_pluginElem);

      /// \brief Get the configuration XML as a string
      /// \return Config element
      public: virtual std::string ConfigStr() const;

      /// \brief
      public: QQuickItem *Item() const;

      /// \brief Load the plugin with a configuration file. Override this
      /// on custom plugins to handle custom configurations.
      ///
      /// Called when a plugin is first created.
      /// This function should not be blocking.
      ///
      /// \sa Load
      /// \param[in] _pluginElem Element containing configuration
      protected: virtual void LoadConfig(
          const tinyxml2::XMLElement */*_pluginElem*/) {}

      /// \brief Get title
      /// \return Plugin title.
      public: virtual std::string Title() const {return this->title;}

      /// \brief Get whether the title bar is displayed
      /// \return True if it is displayed
      public: virtual bool HasTitlebar() {return this->hasTitlebar;}

      /// \brief Get the value of the the `delete_later` attribute from the
      /// configuration file, which defaults to false.
      /// \return The value of `delete_later`.
      public: bool DeleteLaterRequested() const;

      /// \brief Show context menu
      /// \param [in] _pos Click position
      protected slots: virtual void ShowContextMenu(const QPoint &_pos);

      // Documentation inherited
//      protected: void changeEvent(QEvent *_e) override;

      /// \brief Wait until the plugin has a parent, then close and delete the
      /// parent.
      protected: void DeleteLater();

      /// \brief Title to be displayed on top of plugin.
      protected: std::string title = "";

      /// \brief True if the plugin should have a title bar, false otherwise.
      protected: bool hasTitlebar = true;

      /// \brief XML configuration
      protected: std::string configStr;

      /// \internal
      /// \brief Pointer to private data
      private: std::unique_ptr<PluginPrivate> dataPtr;
    };
  }
}

#endif
