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

#include <iostream>

#include "HelloPlugin.hh"

using namespace ignition;
using namespace gui;

/////////////////////////////////////////////////
HelloPlugin::HelloPlugin()
  : GUIPlugin()
{
  // Create a push button, and connect it to the OnButton function
  auto button = new QPushButton(tr("Hello, plugin!"));
  connect(button, SIGNAL(clicked()), this, SLOT(OnButton()));

  // Create the layout to hold the button
  auto layout = new QHBoxLayout;
  layout->addWidget(button);

  // Use the layout
  this->setLayout(layout);
}

/////////////////////////////////////////////////
HelloPlugin::~HelloPlugin()
{
}

/////////////////////////////////////////////////
void HelloPlugin::OnButton()
{
  std::cout << "Hello, plugin!" << std::endl;
}

// Register this plugin
IGN_REGISTER_GUI_PLUGIN(HelloPlugin)

