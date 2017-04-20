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

#include "ignition/gui/MainWindow.hh"

namespace ignition
{
  namespace gui
  {
    class MainWindowPrivate
    {
    };
  }
}

using namespace ignition;
using namespace gui;

/////////////////////////////////////////////////
ignition::gui::MainWindow::MainWindow()
  : dataPtr(new MainWindowPrivate)
{
  this->setObjectName("mainWindow");

  // Title
  std::string title = "Ignition GUI";
  this->setWindowIconText(tr(title.c_str()));
  this->setWindowTitle(tr(title.c_str()));

  // Main widget
  auto mainWidget = new QWidget;
  mainWidget->show();
  this->setCentralWidget(mainWidget);

  // Menu
  auto fileMenu = this->menuBar()->addMenu(tr("&File"));

  auto quitAct = new QAction(tr("&Quit"), this);
  quitAct->setStatusTip(tr("Quit"));
  this->connect(quitAct, SIGNAL(triggered()), this, SLOT(close()));
  fileMenu->addAction(quitAct);

  this->show();
}

/////////////////////////////////////////////////
ignition::gui::MainWindow::~MainWindow()
{
}

