/*
 * Copyright (C) 2018 Open Source Robotics Foundation
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
#ifndef IGNITION_GUI_APPLICATION_HH_
#define IGNITION_GUI_APPLICATION_HH_

#include <memory>

#include "ignition/gui/qt.h"
#include "ignition/gui/Export.hh"

namespace ignition
{
  namespace gui
  {
    class ApplicationPrivate;

    /// \brief
    class IGNITION_GUI_VISIBLE Application : public QGuiApplication
    {
      Q_OBJECT

      // Documentation inherited
      public: Application(int &_argc, char **_argv);

      /// \brief Destructor
      public: virtual ~Application();

      /// \brief Get the QML engine
      /// \return Pointer to QML engine
      public: QQmlApplicationEngine *Engine() const;

      /// \internal
      /// \brief Private data pointer
      private: std::unique_ptr<ApplicationPrivate> dataPtr;
    };

    /// \brief Get current running application, this is a cast og qGuiApp.
    /// \return Pointer to running application, or nullptr if none is running.
    IGNITION_GUI_VISIBLE
    Application *App();
  }
}
#endif
