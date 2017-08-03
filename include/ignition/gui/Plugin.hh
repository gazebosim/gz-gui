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
#include <string>

#include "ignition/gui/qt.h"
#include "ignition/gui/System.hh"

namespace ignition
{
  namespace gui
  {
    /// \brief Gui plugin
    class IGNITION_GUI_VISIBLE Plugin
        : public QWidget
    {
      Q_OBJECT

      /// \brief Constructor
      public: Plugin() {}

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
      public: virtual std::string Title() {return this->title;}

      /// \brief Get whether the title bar is displayed
      /// \return True if it is displayed
      public: virtual bool HasTitlebar() {return this->hasTitlebar;}

      /// \brief Show context menu
      /// \param [in] _pos Click position
      protected slots: void ShowContextMenu(const QPoint &_pos);

      /// \brief Title to be displayed on top of plugin.
      protected: std::string title = "";

      /// \brief True if the plugin should have a title bar, false otherwise.
      protected: bool hasTitlebar = true;
    };
  }
}

#endif
