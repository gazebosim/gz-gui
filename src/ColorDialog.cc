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

#include <ignition/common/Console.hh>
#include "ignition/gui/Iface.hh"
#include "ignition/gui/ColorDialog.hh"

namespace ignition
{
  namespace gui
  {
    class ColorDialogPrivate
    {
    };
  }
}

using namespace ignition;
using namespace gui;

/////////////////////////////////////////////////
ColorDialog::ColorDialog()
  : dataPtr(new ColorDialogPrivate)
{
  auto mainLayout = new QGridLayout();
  mainLayout->addWidget(new QLabel("Colors!"));

  this->setLayout(mainLayout);

  setPrimaryColor("red");
}

/////////////////////////////////////////////////
ColorDialog::~ColorDialog()
{
}

