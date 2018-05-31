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

#include "ignition/gui/qt.h"
#include "ignition/gui/Export.hh"

namespace ignition
{
  namespace gui
  {
    /// \brief Gui plugin
    class IGNITION_GUI_VISIBLE Dialog : public QObject
    {
      Q_OBJECT

      public: Dialog();

      /// \brief Signals that this dialog is about to be closed.
      signals: void Closing();

      // Documentation inherited
//      protected: void reject() override;
    };
  }
}

#endif
