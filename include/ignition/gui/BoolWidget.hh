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

#include "ignition/gui/qt.h"
#include "ignition/gui/MessageWidget.hh"
#include "ignition/gui/System.hh"

namespace ignition
{
  namespace gui
  {
    class BoolWidgetPrivate;

    /// \brief A widget generated from a google protobuf message.
    class IGNITION_GUI_VISIBLE BoolWidget : public PropertyWidget
    {
      Q_OBJECT

      /// \brief Constructor
      /// \param[in] _key Property key.
      /// \param[in] _level Depth level, in case nested.
      public: BoolWidget(const std::string &_key,
                         const unsigned int _level = 0);

      /// \brief Destructor
      public: ~BoolWidget();

      /// \brief Update widget with new bool value.
      /// \param[in] _value Value to set to.
      /// \return True if the update completed successfully.
      public: bool SetValue(const bool _value);

      /// \brief Get bool value from widget.
      /// \return Value of the widget.
      public: bool Value() const;

      /// \brief Signal that the value has changed.
      /// \param[in] _value New bool value.
      signals: void ValueChanged(const bool _value);

      /// \brief Callback when an internal widget's value has changed.
      private slots: void OnValueChanged();

      /// \internal
      /// \brief Pointer to private data.
      private: std::unique_ptr<BoolWidgetPrivate> dataPtr;
    };
  }
}
#endif
