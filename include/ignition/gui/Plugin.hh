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
      /// \brief Constructor
      public: Plugin() {}

      /// \brief Load function
      ///
      /// Called when a plugin is first created.
      /// This function should not be blocking.
      public: virtual void Load() {}

      /// \brief Get title
      /// \return Plugin title.
      public: virtual std::string Title() {return this->title;}

      // \brief must be defined to support style sheets
      public: virtual void paintEvent(QPaintEvent *)
      {
        QStyleOption opt;
        opt.init(this);
        QPainter p(this);
        style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
      }

      /// \brief Title to be displayed on top of plugin.
      private: std::string title = "GUI plugin";
    };
  }
}

#endif
