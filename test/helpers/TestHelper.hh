/*
 * Copyright (C) 2021 Open Source Robotics Foundation
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
#ifndef IGNITION_GUI_TESTHELPER_HH_
#define IGNITION_GUI_TESTHELPER_HH_

#include <ignition/gui/Application.hh>
#include <ignition/gui/Export.hh>
#include <ignition/gui/MainWindow.hh>

#ifndef _WIN32
#  define TestHelper_EXPORTS_API
#else
#  if (defined(TestHelper_EXPORTS))
#    define TestHelper_EXPORTS_API __declspec(dllexport)
#  else
#    define TestHelper_EXPORTS_API __declspec(dllimport)
#  endif
#endif

namespace ignition
{
namespace gui
{
/// \brief
class TestHelper_EXPORTS_API TestHelper : public QObject
{
  Q_OBJECT

  /// \brief Constructor
  public: TestHelper();

  /// \brief Destructor
  public: ~TestHelper() = default;

  /// \brief Documentation inherited
  public: bool eventFilter(QObject *_obj, QEvent *_event) override;

  public: std::function<void (QEvent *)> forwardEvent;
};
}
}

#endif
