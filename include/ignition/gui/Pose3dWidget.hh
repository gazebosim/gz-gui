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

#ifndef IGNITION_GUI_POSE3DWIDGET_HH_
#define IGNITION_GUI_POSE3DWIDGET_HH_

#include <memory>

#include "ignition/gui/qt.h"
#include "ignition/gui/PropertyWidget.hh"
#include "ignition/gui/System.hh"

namespace ignition
{
  namespace gui
  {
    class Pose3dWidgetPrivate;

    /// \brief A widget which holds a pose 3d property.
    class IGNITION_GUI_VISIBLE Pose3dWidget : public PropertyWidget
    {
      Q_OBJECT

      /// \brief Constructor
      /// Note that unlike other similar widgets, a pose 3d widget doesn't hold
      /// its own key value. Instead, it can be placed within another widget,
      /// such as a CollapsibleWidget, which will display a key value.
      public: Pose3dWidget();

      /// \brief Destructor
      public: ~Pose3dWidget();

      /// \brief Inherited from PropertyWidget.
      ///
      /// Value will be handled if it holds a ignition::math::Pose3d value.
      ///
      /// An input can be constructed as follows:
      /// QVariant value = QVariant::fromValue(ignition::math::Pose3d::Zero);
      ///
      /// \param[in] _value New value.
      /// \return True if succesfull.
      public: bool SetValue(const QVariant _value) override;

      /// \brief Inherited from PropertyWidget.
      ///
      /// Returns a variant containing the widget's current
      /// ignition::math::Pose3d value.
      ///
      /// A pose value can be retrieved as follows:
      /// auto value = this->Value().value<ignition::math::Pose3d>();
      ///
      /// \return Widget's current value.
      public: QVariant Value() const override;

      /// \internal
      /// \brief Pointer to private data.
      private: std::unique_ptr<Pose3dWidgetPrivate> dataPtr;
    };
  }
}
#endif
