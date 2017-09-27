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

#ifndef IGNITION_GUI_COLLAPSIBLEWIDGET_HH_
#define IGNITION_GUI_COLLAPSIBLEWIDGET_HH_

#include "ignition/gui/qt.h"
#include "ignition/gui/PropertyWidget.hh"
#include "ignition/gui/System.hh"

namespace ignition
{
  namespace gui
  {
    /// \brief A collapsible widget that holds a child widget.
    class IGNITION_GUI_VISIBLE CollapsibleWidget : public QWidget
    {
      Q_OBJECT

      /// \brief Constructor
      /// \param[in] _key
      /// \param[in] _childWidget
      /// \param[in] _level
      public: CollapsibleWidget(const std::string &_key,
          PropertyWidget *_childWidget, const int _level);

      /// \brief Callback that collapses or expands the child widget.
      /// _param[in] _checked True for expanded.
      public slots: void Toggle(bool _checked);

      /// \brief Child widget that can be collapsed or expanded.
      public: QWidget *childWidget;
    };
  }
}
#endif
