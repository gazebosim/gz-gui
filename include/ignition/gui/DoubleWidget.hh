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

#ifndef IGNITION_GUI_DOUBLEWIDGET_HH_
#define IGNITION_GUI_DOUBLEWIDGET_HH_

#include <memory>

#include "ignition/gui/qt.h"
#include "ignition/gui/MessageWidget.hh"
#include "ignition/gui/System.hh"

namespace ignition
{
  namespace gui
  {
    class DoubleWidgetPrivate;

    /// \brief A widget which holds a double property.
    class IGNITION_GUI_VISIBLE DoubleWidget : public PropertyWidget
    {
      Q_OBJECT

      /// \brief Constructor
      /// \param[in] _key Property key.
      /// \param[in] _level Depth level, in case nested.
      public: DoubleWidget(const std::string &_key,
                           const unsigned int _level = 0);

      /// \brief Destructor
      public: ~DoubleWidget();

      /// \brief Update widget with new double value.
      /// \param[in] _value Value to set to.
      /// \return True if the update completed successfully.
      public: bool SetValue(const double _value);

      /// \brief Get double value from widget.
      /// \return Value of the widget.
      public: double Value() const;

      /// \brief Signal that the value has changed.
      /// \param[in] _value New double value.
      signals: void ValueChanged(const double _value);

      /// \brief Callback when an internal widget's value has changed.
      private slots: void OnValueChanged();

      /// \internal
      /// \brief Pointer to private data.
      private: std::unique_ptr<DoubleWidgetPrivate> dataPtr;
    };
  }
}
#endif
