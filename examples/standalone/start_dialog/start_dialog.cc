/*
 * Copyright (C) 2022 Open Source Robotics Foundation
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

#include <gz/gui/qt.h>
#include <gz/gui/Application.hh>
#include <gz/gui/Dialog.hh>
#include <gz/gui/MainWindow.hh>

//////////////////////////////////////////////////
int main(int _argc, char **_argv)
{
  // Increase verboosity so we see all messages
  gz::common::Console::SetVerbosity(4);

  // Create app
  gz::gui::Application app(_argc, _argv, gz::gui::WindowType::kDialog);

  igndbg << "Open dialog" << std::endl;

  // Add and display a dialog
  auto dialog = new gz::gui::Dialog();
  dialog->QuickWindow();

  std::string qmlFile(":start_dialog/start_dialog.qml");
  if (!QFile(QString::fromStdString(qmlFile)).exists())
  {
    ignerr << "Can't find [" << qmlFile
           << "]. Are you sure it was added to the .qrc file?" << std::endl;
    return -1;
  }

  QQmlComponent dialogComponent(gz::gui::App()->Engine(),
      QString(QString::fromStdString(qmlFile)));
  if (dialogComponent.isError())
  {
    std::stringstream errors;
    errors << "Failed to instantiate QML file [" << qmlFile << "]."
           << std::endl;
    for (auto error : dialogComponent.errors())
    {
      errors << "* " << error.toString().toStdString() << std::endl;
    }
    ignerr << errors.str();
    return -1;
  }

  auto dialogItem = qobject_cast<QQuickItem *>(dialogComponent.create());
  if (!dialogItem)
  {
    ignerr << "Failed to instantiate QML file [" << qmlFile << "]." << std::endl
           << "Are you sure the file is valid QML? "
           << "You can check with the `qmlscene` tool" << std::endl;
    return -1;
  }

  dialogItem->setParentItem(dialog->RootItem());

  // Execute start dialog
  app.exec();

  // After dialog is shut, display the main window
  igndbg << "Dialog closed, open main window" << std::endl;

  // Create main window
  app.CreateMainWindow();

  // Run main window
  app.exec();

  igndbg << "Main window closed" << std::endl;

  return 0;
}

