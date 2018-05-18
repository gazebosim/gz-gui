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

#include <iostream>

#include <QQmlContext>
#include <QQuickStyle>
#include <QQuickWidget>

#include <ignition/common/PluginMacros.hh>

#include "HelloQml.hh"

// inline void initResources() { Q_INIT_RESOURCE(resources); }

using namespace ignition;
using namespace gui;

/////////////////////////////////////////////////
HelloQml::HelloQml()
  : Plugin()
{
  // TODO: make this work
  // initResources();

  // Use material style
  QQuickStyle::setStyle("Material");

  // Instantiate widget described in QML
  auto w = new QQuickWidget();
  w->setSource(QUrl::fromLocalFile("../HelloQml.qml"));

  // Allow the QML code to reference slots in this plugin
  w->rootContext()->setContextProperty("HelloQml", this);

  // Put the widget in a layout as a child of this widget
  auto layout = new QHBoxLayout;
  layout->addWidget(w);
  this->setLayout(layout);
}

/////////////////////////////////////////////////
HelloQml::~HelloQml()
{
}

/////////////////////////////////////////////////
void HelloQml::OnButton()
{
  std::cout << "Hello, qml!" << std::endl;
}

// Register this plugin
IGN_COMMON_REGISTER_SINGLE_PLUGIN(ignition::gui::HelloQml,
                                  ignition::gui::Plugin);
