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

#ifndef IGNITION_GUI_VARIABLEPILLCONTAINER_TEST_HH_
#define IGNITION_GUI_VARIABLEPILLCONTAINER_TEST_HH_

#include <chrono>
#include <thread>

#include "ignition/gui/qt.h"
#include "ignition/gui/System.hh"
#include "ignition/gui/VariablePill.hh"

namespace ignition
{
  namespace gui
  {
    class ColorWidgetPrivate;

    /// \brief A widget which holds a color property.
    class IGNITION_GUI_VISIBLE VarPillContainerTest : public QObject
    {
      Q_OBJECT

      /// \brief Constructor
      /// Note that unlike other similar widgets, a color widget doesn't hold
      /// its own key value. Instead, it can be placed within another widget,
      /// such as a CollapsibleWidget, which will display a key value.
      public: VarPillContainerTest(VariablePill *_pill1, VariablePill *_pill2)
        : pill1(_pill1),
          pill2(_pill2)
      {
        this->connect(this, SIGNAL(MouseMove(const unsigned int)),
                      this, SLOT(OnMouseMove(const unsigned int)));
      };

      /// \brief Destructor
      public: ~VarPillContainerTest()
      {
      };

      /// \brief ToDo
      signals: void MouseMove(const unsigned int _offset);

      /// \brief ToDo
      private slots: void OnMouseMove(const unsigned int _offset)
      {
        if (_offset <= 0)
          this->OnMouseRelease();

        std::cout << "OnMouseMove (" << _offset << ")" << std::endl;
        QPoint center(this->pill1->width() * 0.5 + _offset,
                      this->pill1->height() * 0.5);

        auto mouseMoveEvent = new QMouseEvent(QEvent::MouseMove,
          center, Qt::NoButton, Qt::LeftButton, Qt::NoModifier);
        QCoreApplication::postEvent(this->pill1, mouseMoveEvent);
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
        QTimer::singleShot(30, this, SIGNAL(OnMouseMove(_offset - 5)));
        QCoreApplication::processEvents();
      };

      /// \brief ToDo
      private slots: void OnMouseRelease()
      {
        std::cout << "OnMouseRelease()" << std::endl;
        QPoint center(this->pill1->width() * 0.5,
                      this->pill1->height() * 0.5);

        auto mouseMoveEvent = new QMouseEvent(QEvent::MouseButtonRelease,
          center, Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
        QCoreApplication::postEvent(this->pill1, mouseMoveEvent);
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
        QCoreApplication::processEvents();
      };

      /// \brief ToDo
      private: VariablePill *pill1;

      /// \brief ToDo
      private: VariablePill *pill2;
    };
  }
}
#endif
