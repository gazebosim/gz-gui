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

#include "ignition/gui/qt.h"
#include "ignition/gui/Export.hh"

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

      /// \internal
      /// \brief Private data pointer
      private: std::unique_ptr<DialogPrivate> dataPtr;
    };
  }
}

#endif
