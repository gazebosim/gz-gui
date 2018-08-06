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

#ifndef IGNITION_GUI_TEST_TESTDISPLAYPLUGIN_HH_
#define IGNITION_GUI_TEST_TESTDISPLAYPLUGIN_HH_

#include <string>

#ifndef Q_MOC_RUN
  #include <ignition/gui/qt.h>
  #include <ignition/gui/DisplayPlugin.hh>
#endif

namespace ignition
{
  namespace gui
  {
    class TestDisplayPlugin : public DisplayPlugin
    {
      Q_OBJECT

      /// \brief Constructor
      public: TestDisplayPlugin();

      /// \brief Destructor
      public: virtual ~TestDisplayPlugin();

      /// \brief Destructor
      public: virtual std::string Type() const override;
    };
  }
}

#endif
