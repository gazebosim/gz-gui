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

#include <ignition/common/Console.hh>

#ifndef Q_MOC_RUN
  #include <ignition/gui/Application.hh>
  #include <ignition/gui/MainWindow.hh>
  #include <ignition/gui/qt.h>
  #include "custom_drawer.hh"
#endif

//////////////////////////////////////////////////
int main(int _argc, char **_argv)
{
  // Increase verboosity so we see all messages
  gz::common::Console::SetVerbosity(4);

  // Initialize app
  gz::gui::Application app(_argc, _argv);

  // Hide original panel
  app.LoadConfig("../custom_drawer.config");

  // Let QML files use CustomActions' functions and properties
  gz::gui::CustomActions actions;
  auto context = new QQmlContext(app.Engine()->rootContext());
  context->setContextProperty("CustomActions", &actions);

  // Instantiate CustomDrawer.qml file into a component
  QQmlComponent component(app.Engine(), ":/CustomDrawer/CustomDrawer.qml");
  auto item = qobject_cast<QQuickItem *>(component.create(context));
  if (!item)
  {
    ignerr << "Failed to initialize custom panel" << std::endl;
    return 1;
  }

  // C++ ownership
  QQmlEngine::setObjectOwnership(item, QQmlEngine::CppOwnership);

  // Add to main window
  auto win = app.findChild<gz::gui::MainWindow *>()->QuickWindow();
  auto drawerItem = win->findChild<QQuickItem *>("sideDrawer");

  item->setParentItem(drawerItem);
  item->setParent(app.Engine());

  // Run window
  app.exec();

  return 0;
}

