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

    /// \brief A widget which holds a boolean property.
    class IGNITION_GUI_VISIBLE EnumWidget : public PropertyWidget
    {
      Q_OBJECT

      /// \brief Constructor
      /// \param[in] _key Property key.
      /// \param[in] _level Depth level, in case nested.
      public: EnumWidget(const std::string &_key,
                         const std::vector<std::string> &_values,
                         const unsigned int _level = 0);

      /// \brief Destructor
      public: ~EnumWidget();

      // Documentation inherited
      public: bool SetValue(const QVariant _value);

      // Documentation inherited
      public: QVariant Value() const;

      /// \brief Add an item to a child enum widget.
      /// \param[in] _itemText Enum text value.
      /// \return True if the item is added successfully.
      public: bool AddItem(const std::string &_itemText);

      /// \brief Remove an item from a child enum widget.
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
