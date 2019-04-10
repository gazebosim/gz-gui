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

#ifndef IGNITION_GUI_BOOLWIDGET_HH_
#define IGNITION_GUI_BOOLWIDGET_HH_

#include <memory>
#include <string>

#include "ignition/gui/qt.h"
#include "ignition/gui/PropertyWidget.hh"
#include "ignition/gui/Export.hh"

namespace ignition
{
  namespace gui
  {
    class BoolWidgetPrivate;

    /// \brief A widget which holds a boolean property.
    class IGNITION_GUI_VISIBLE BoolWidget : public PropertyWidget
    {
      Q_OBJECT

      /// \brief Constructor
      /// \param[in] _key Property key value, such as "enable", which will be
      /// displayed next to the checkboxes representing the value.
      public: explicit BoolWidget(const std::string &_key);

      /// \brief Destructor
      public: ~BoolWidget();

      /// \brief Inherited from PropertyWidget.
      ///
      /// Value will be handled if it holds a boolean value.
      ///
      /// An input can be constructed as follows:
      /// QVariant value(true);
      ///
      /// \param[in] _value New value.
      /// \return True if succesfull.
      public: bool SetValue(const QVariant _value) override;

      /// \brief Inherited from PropertyWidget.
      ///
      /// Returns a variant containing the widget's current bool value.
      ///
      /// The value can be retrieved as follows:
      /// bool value = this->Value().toBool();
      ///
      /// \return Widget's current value.
      public: QVariant Value() const override;

      /// \internal
      /// \brief Pointer to private data.
      private: std::unique_ptr<BoolWidgetPrivate> dataPtr;
    };
  }
}
#endif
