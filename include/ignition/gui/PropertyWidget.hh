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

#ifndef IGNITION_GUI_PROPERTYWIDGET_HH_
#define IGNITION_GUI_PROPERTYWIDGET_HH_

#include <map>
#include <string>
#include <vector>

#include "ignition/gui/qt.h"
#include "ignition/gui/System.hh"

namespace ignition
{
  namespace gui
  {
    /// \brief Widget which holds a property.
    class IGNITION_GUI_VISIBLE PropertyWidget : public QFrame
    {
      Q_OBJECT

      /// \brief Constructor;
      public: PropertyWidget() {}

      /// \brief Get value from widget.
      /// \return Value of the widget as a QVariant.
      public: virtual QVariant Value() const = 0;

      /// \brief Update widget with new value.
      /// \param[in] _value Value to set to.
      /// \return True if the update completed successfully.
      public: virtual bool SetValue(const QVariant _value) = 0;

      /// \brief Signal that the value has changed.
      /// \param[in] _value New value.
      signals: void ValueChanged(QVariant _value);

      /// \brief Callback when an internal widget's value has changed.
      protected slots: void OnValueChanged();

      /// \brief Widget's key value, such as "mass" or "color".
      public: std::string key;

      /// \brief Widget's scoped name based on its parents.
      public: std::string scopedName;

      /// \brief Map a widget to the label holding its unit value.
      public: std::map<QWidget *, QLabel *> mapWidgetToUnit;

      /// \brief Level of how nested the widget is.
      public: unsigned int level;
    };
  }
}
#endif
