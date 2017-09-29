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

#ifndef IGNITION_GUI_DENSITYWIDGET_HH_
#define IGNITION_GUI_DENSITYWIDGET_HH_

#include <memory>
#include <string>

#include "ignition/gui/qt.h"
#include "ignition/gui/PropertyWidget.hh"
#include "ignition/gui/System.hh"

namespace ignition
{
  namespace gui
  {
    class DensityWidgetPrivate;

    /// \brief A widget which holds a density property.
    class IGNITION_GUI_VISIBLE DensityWidget : public PropertyWidget
    {
      Q_OBJECT

      /// \brief Constructor
      /// \param[in] _key Property key.
      public: DensityWidget(const std::string &_key);

      /// \brief Destructor
      public: ~DensityWidget();

      // Documentation inherited
      public: bool SetValue(const QVariant _value);

      // Documentation inherited
      public: QVariant Value() const;

      /// \brief Callback when the density combo box is changed.
      /// \param[in] _text New density type in string.
      private slots: void OnComboBoxChanged();

      /// \brief Callback when the density spin box is changed.
      /// \param[in] _text New density value in string.
      private slots: void OnSpinBoxChanged();

      /// \internal
      /// \brief Pointer to private data.
      private: std::unique_ptr<DensityWidgetPrivate> dataPtr;
    };
  }
}
#endif
