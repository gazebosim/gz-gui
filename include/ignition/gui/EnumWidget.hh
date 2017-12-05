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

#ifndef IGNITION_GUI_ENUMWIDGET_HH_
#define IGNITION_GUI_ENUMWIDGET_HH_

#include <memory>
#include <string>
#include <vector>

#include "ignition/gui/qt.h"
#include "ignition/gui/PropertyWidget.hh"
#include "ignition/gui/System.hh"

namespace ignition
{
  namespace gui
  {
    class EnumWidgetPrivate;

    /// \brief A widget which holds an enum property, which consists of a
    /// list of possible string values.
    class IGNITION_GUI_VISIBLE EnumWidget : public PropertyWidget
    {
      Q_OBJECT

      /// \brief Constructor
      /// \param[in] _key Property key value, such as "type", which will be
      /// displayed next to the dropdown which holds the enum options.
      /// \param[in] _values Vector of possible values for the enum.
      public: EnumWidget(const std::string &_key,
          const std::vector<std::string> &_values = std::vector<std::string>());

      /// \brief Destructor
      public: ~EnumWidget();

      /// \brief Inherited from PropertyWidget.
      ///
      /// Value will be handled if the variant contains an std::string.
      ///
      /// An input can be constructed as follows:
      /// QVariant value = QVariant::fromValue(std::string("enum option"));
      ///
      /// \param[in] _value New value as a string.
      /// \return True if succesfull.
      public: bool SetValue(const QVariant _value) override;

      /// \brief Inherited from PropertyWidget.
      ///
      /// Returns a variant containing the widget's current value as an
      /// std::string.
      ///
      /// A value can be retrieved in string format as follows:
      /// std::string value = this->Value().value<std::string>();
      ///
      /// \return Widget's current value.
      public: QVariant Value() const override;

      /// \brief Add a new possible value to the dropdown.
      /// \param[in] _itemText Enum text value.
      /// \return True if the item is added successfully.
      public: bool AddItem(const std::string &_itemText);

      /// \brief Remove an item from the dropdown.
      /// \param[in] _itemText Text of the enum value.
      /// \return True if the item is removed successfully.
      public: bool RemoveItem(const std::string &_itemText);

      /// \brief Remove all items from widget.
      /// \return True if successful.
      public: bool Clear();

      /// \internal
      /// \brief Pointer to private data.
      private: std::unique_ptr<EnumWidgetPrivate> dataPtr;
    };
  }
}
#endif
