/*
 * Copyright (C) 2020 Open Source Robotics Foundation
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

#ifndef GZ_GUI_EXAMPLES_PLUGINS_MULTIPLEQML_HH_
#define GZ_GUI_EXAMPLES_PLUGINS_MULTIPLEQML_HH_

#ifndef Q_MOC_RUN
  #include <gz/gui/qt.h>
  #include <gz/gui/Plugin.hh>
#endif

class MultipleQml : public gz::gui::Plugin
{
  Q_OBJECT

  /// \brief Constructor
  public: MultipleQml();

  /// \brief Destructor
  public: virtual ~MultipleQml();

  /// \brief Callback triggered when the button is pressed.
  /// \param[in] _text Button text.
  protected slots: void OnButton(const QString &_text);
};

#endif  // GZ_GUI_EXAMPLES_PLUGINS_MULTIPLEQML_HH_
