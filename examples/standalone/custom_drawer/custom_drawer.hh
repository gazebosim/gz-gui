/*
 * Copyright (C) 2018 Open Source Robotics Foundation
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

#ifndef GZ_GUI_EXAMPLES_STANDALONE_CUSTOMDRAWER_HH_
#define GZ_GUI_EXAMPLES_STANDALONE_CUSTOMDRAWER_HH_

#include <iostream>

#ifndef Q_MOC_RUN
  #include <gz/gui/qt.h>
#endif

namespace gz
{
  namespace gui
  {
    /// \brief Object holding actions which can be triggered from the custom
    /// drawer.
    class CustomActions : public QObject
    {
      Q_OBJECT

      /// \brief Demonstrates a C++ function which can be called from the
      /// drawer's QML code.
      public: Q_INVOKABLE void cppActionFromQml() const
        {
          std::cout << "C++ action called from QML" << std::endl;
        }
    };
  }
}

#endif

