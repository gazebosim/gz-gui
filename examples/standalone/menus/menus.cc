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

#ifndef Q_MOC_RUN
  #include <ignition/common/Console.hh>
  #include <ignition/gui/Application.hh>
  #include <ignition/gui/MainWindow.hh>
  #include <ignition/gui/qt.h>
  #include "menus.hh"
#endif

//////////////////////////////////////////////////
int main(int _argc, char **_argv)
{
  // Increase verboosity so we see all messages
  ignition::common::Console::SetVerbosity(4);

  // Initialize app
  ignition::gui::Application app(_argc, _argv);

  // Hide original panel
  app.LoadConfig("../menus.config");

  // Create main window
  app.Initialize(ignition::gui::WindowType::kMainWindow);

  // Let QML files use CustomActions' functions and properties
  ignition::gui::CustomActions actions;
  auto context = new QQmlContext(app.Engine()->rootContext());
  context->setContextProperty("CustomActions", &actions);

  // Instantiate CustomPanel.qml file into a component
  QQmlComponent component(app.Engine(), ":/Menus/CustomPanel.qml");
  auto item = qobject_cast<QQuickItem *>(component.create(context));
  if (!item)
  {
    ignerr << "Failed to initialize custom panel" << std::endl;
    return 1;
  }

  // C++ ownership
  QQmlEngine::setObjectOwnership(item, QQmlEngine::CppOwnership);

  // Add to main window
  auto win = app.allWindows()[0];
  auto drawerItem = win->findChild<QQuickItem *>("sideDrawer");

  item->setParentItem(drawerItem);
  item->setParent(app.Engine());

  // Run window
  app.exec();

  return 0;
}

