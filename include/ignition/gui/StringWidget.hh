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

#ifndef IGNITION_GUI_STRINGWIDGET_HH_
#define IGNITION_GUI_STRINGWIDGET_HH_

#include <memory>
#include <string>

#include "ignition/gui/qt.h"
#include "ignition/gui/PropertyWidget.hh"
#include "ignition/gui/System.hh"

namespace ignition
{
  namespace gui
  {
    class StringWidgetPrivate;

    /// \brief A widget which holds a boolean property.
    class IGNITION_GUI_VISIBLE StringWidget : public PropertyWidget
    {
      Q_OBJECT

      /// \brief Constructor
      /// \param[in] _key Property key.
      /// \param[in] _type "line" or "plain"
      public: StringWidget(const std::string &_key,
                           const std::string &_type = "line");

      /// \brief Destructor
      public: ~StringWidget();

      // Documentation inherited
      public: bool SetValue(const QVariant _value);

      // Documentation inherited
      public: QVariant Value() const;

      /// \internal
      /// \brief Pointer to private data.
      private: std::unique_ptr<StringWidgetPrivate> dataPtr;
    };
  }
}
#endif
