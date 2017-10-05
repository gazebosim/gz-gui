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

#ifndef IGNITION_GUI_DESIGNERPLUGIN_HH_
#define IGNITION_GUI_DESIGNERPLUGIN_HH_

#include <ignition/gui/qt.h>
#include <ignition/gui/Plugin.hh>

namespace ignition
{
  namespace gui
  {
    namespace Ui
    {
      class DesignerPlugin;
    }

    /// \broef Example demonstrating how to use a Qt Designer UI (.ui) file to
    /// generate an Ignition GUI plugin.
    class DesignerPlugin : public Plugin
    {
      Q_OBJECT

      /// \brief Constructor
      public: explicit DesignerPlugin();

      /// \brief Destructor
      public: virtual ~DesignerPlugin();

      /// \brief Callback trigged when the button is clicked. The function name
      /// must follows Qt's  auto-connect convention:
      ///     on_<object name>_<signal name>(<signal parameters>)
      protected slots: void on_helloButton_clicked();

      /// \brief Class generated from DesignerPlugin.ui.
      private: Ui::DesignerPlugin *ui;
    };
  }
}

#endif
