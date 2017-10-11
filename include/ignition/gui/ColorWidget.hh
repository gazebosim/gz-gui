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

#ifndef IGNITION_GUI_COLORWIDGET_HH_
#define IGNITION_GUI_COLORWIDGET_HH_

#include <memory>

#include "ignition/gui/qt.h"
#include "ignition/gui/PropertyWidget.hh"
#include "ignition/gui/System.hh"

namespace ignition
{
  namespace gui
  {
    class ColorWidgetPrivate;

    /// \brief A widget which holds a color property.
    class IGNITION_GUI_VISIBLE ColorWidget : public PropertyWidget
    {
      Q_OBJECT

      /// \brief Constructor
      /// Note that unlike other similar widgets, a pose 3d widget doesn't hold
      /// its own key value. Instead, it can be placed within another widget,
      /// such as a CollapsibleWidget, which will display a key value.
      public: ColorWidget();

      /// \brief Destructor
      public: ~ColorWidget();

      /// \brief Inherited from PropertyWidget.
      ///
      /// Value will be handled if it holds a math::Color value.
      ///
      /// An input can be constructed as follows:
      /// QVariant value = QVariant::fromValue(ignition::math::Color::Zero);
      ///
      /// \param[in] _value New value.
      /// \return True if succesfull.
      public: bool SetValue(const QVariant _value) override;

      /// \brief Inherited from PropertyWidget.
      ///
      /// Returns a variant containing the widget's current math::Color value.
      ///
      /// A pose value can be retrieved as follows:
      /// auto value = this->Value().value<ignition::math::Color>();
      ///
      /// \return Widget's current value.
      public: QVariant Value() const override;

      /// \brief Callback which opens a color dialog.
      private slots: void OnCustomColorDialog();

      /// \internal
      /// \brief Pointer to private data.
      private: std::unique_ptr<ColorWidgetPrivate> dataPtr;
    };
  }
}
#endif
