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

#ifndef IGNITION_GUI_NUMBERWIDGET_HH_
#define IGNITION_GUI_NUMBERWIDGET_HH_

#include <memory>
#include <string>

#include "ignition/gui/qt.h"
#include "ignition/gui/Enums.hh"
#include "ignition/gui/PropertyWidget.hh"
#include "ignition/gui/System.hh"

namespace ignition
{
  namespace gui
  {
    class NumberWidgetPrivate;

    /// \brief A widget which holds a number property such as a double, int or
    /// uint.
    class IGNITION_GUI_VISIBLE NumberWidget : public PropertyWidget
    {
      Q_OBJECT

      /// \brief Constructor
      /// \param[in] _key Property key value, such as "length", which will be
      /// displayed next to the spin box which holds the value.
      /// \param[in] _type The value type, which will determine characteristics
      /// of the spin box, such as decimal places and range. Defaults to DOUBLE.
      public: NumberWidget(const std::string &_key,
                           const NumberType _type = NumberType::NONE);

      /// \brief Destructor
      public: ~NumberWidget();

      /// \brief Inherited from PropertyWidget.
      ///
      /// Value will be handled if its type matches the widget's type (double,
      /// int or uint).
      ///
      /// A double input can be constructed as follows:
      /// QVariant value(0.5);
      ///
      /// \param[in] _value New value.
      /// \return True if succesfull.
      public: bool SetValue(const QVariant _value);

      /// \brief Inherited from PropertyWidget.
      ///
      /// Returns a variant containing the widget's current value. Its type will
      /// be either double, int or uint depending on the widget's type.
      ///
      /// An int value can be retrieved as follows:
      /// int value = this->Value().toInt();
      ///
      /// \return Widget's current number value.
      public: QVariant Value() const;

      /// \internal
      /// \brief Pointer to private data.
      private: std::unique_ptr<NumberWidgetPrivate> dataPtr;
    };
  }
}
#endif
