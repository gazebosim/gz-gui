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

#include <gz/common/Console.hh>
#include "gz/gui/Application.hh"
#include "gz/gui/Dialog.hh"

namespace gz
{
  namespace gui
  {
    class DialogPrivate
    {
      /// \brief Pointer to quick window
      public: QQuickWindow *quickWindow{nullptr};
    };
  }
}

using namespace gz;
using namespace gui;

/////////////////////////////////////////////////
Dialog::Dialog()
  : dataPtr(new DialogPrivate)
{
  // Load QML and keep pointer to generated QQuickWindow
  std::string qmlFile("qrc:qml/StandaloneDialog.qml");
  App()->Engine()->load(QUrl(QString::fromStdString(qmlFile)));

  this->dataPtr->quickWindow = qobject_cast<QQuickWindow *>(
      App()->Engine()->rootObjects().value(0));
  if (!this->dataPtr->quickWindow)
  {
    gzerr << "Internal error: Failed to instantiate QML file [" << qmlFile
           << "]" << std::endl;
    return;
  }
}

/////////////////////////////////////////////////
Dialog::~Dialog()
{
}

/////////////////////////////////////////////////
QQuickWindow *Dialog::QuickWindow() const
{
  return this->dataPtr->quickWindow;
}

/////////////////////////////////////////////////
QQuickItem *Dialog::RootItem() const
{
  auto dialogItem = this->dataPtr->quickWindow->findChild<QQuickItem *>();
  if (!dialogItem)
  {
    gzerr << "Internal error: Null dialog root item!" << std::endl;
  }

  return dialogItem;
}

