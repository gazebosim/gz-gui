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
#ifndef IGNITION_GUI_MAINWINDOW_HH_
#define IGNITION_GUI_MAINWINDOW_HH_

#include <memory>

#include "ignition/gui/qt.h"
#include "ignition/gui/System.hh"

namespace ignition
{
  namespace gui
  {
    // Forward declare private data
    class MainWindowPrivate;

    class IGNITION_GUI_VISIBLE MainWindow : public QMainWindow
    {
      Q_OBJECT

      /// \brief Constructor
      public: MainWindow();

      /// \brief Destructor
      public: virtual ~MainWindow();

      /// \brief Close all docks
      /// \return True if all docks have been closed
      public: bool CloseAllDocks();

      /// \brief Callback when load configuration is selected
      private slots: void OnLoadConfig();

      /// \brief Callback when save configuration is selected
      private slots: void OnSaveConfig();

      /// \brief Callback when load stylesheet is selected
      private slots: void OnLoadStylesheet();

      /// \brief Add a plugin to the window.
      /// \param [in] _plugin Plugin filename
      private slots: void OnAddPlugin(QString _plugin);

      /// \internal
      /// \brief Private data pointer
      private: std::unique_ptr<MainWindowPrivate> dataPtr;
    };
  }
}
#endif
