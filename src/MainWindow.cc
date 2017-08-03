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
MainWindow::MainWindow()
  : dataPtr(new MainWindowPrivate)
{
  this->setObjectName("mainWindow");

  // Title
  std::string title = "Ignition GUI";
  this->setWindowIconText(tr(title.c_str()));
  this->setWindowTitle(tr(title.c_str()));

  // Main widget

  // Menu
  auto fileMenu = this->menuBar()->addMenu(tr("&File"));

  auto loadConfigAct = new QAction(tr("&Load configuration"), this);
  loadConfigAct->setStatusTip(tr("Quit"));
  this->connect(loadConfigAct, SIGNAL(triggered()), this, SLOT(OnLoadConfig()));
  fileMenu->addAction(loadConfigAct);

  auto saveConfigAct = new QAction(tr("&Save configuration"), this);
  saveConfigAct->setStatusTip(tr("Quit"));
  this->connect(saveConfigAct, SIGNAL(triggered()), this, SLOT(OnSaveConfig()));
  fileMenu->addAction(saveConfigAct);

  // Docking
  this->setDockOptions(QMainWindow::AnimatedDocks |
                       QMainWindow::AllowTabbedDocks |
                       QMainWindow::AllowNestedDocks);
}

/////////////////////////////////////////////////
MainWindow::~MainWindow()
{
}

/////////////////////////////////////////////////
void MainWindow::OnLoadConfig()
{
  QFileDialog fileDialog(this, tr("Load configuration"), QDir::homePath(),
      tr("*.ini"));
  fileDialog.setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint |
      Qt::WindowStaysOnTopHint | Qt::CustomizeWindowHint);

  if (fileDialog.exec() != QDialog::Accepted)
    return;

  auto selected = fileDialog.selectedFiles();
  if (selected.empty())
    return;

  auto settings = new QSettings(selected[0], QSettings::IniFormat);

  auto state = settings->value("state", QByteArray()).toByteArray();
  this->restoreState(state);

  auto pos = settings->value("pos", QPoint(200, 200)).toPoint();
  this->move(pos);

  auto size = settings->value("size", QSize(400, 400)).toSize();
  this->resize(size);
}

/////////////////////////////////////////////////
void MainWindow::OnSaveConfig()
{
  QFileDialog fileDialog(this, tr("Save configuration"), QDir::homePath(),
      tr("*.ini"));
  fileDialog.setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint |
      Qt::WindowStaysOnTopHint | Qt::CustomizeWindowHint);
  fileDialog.setAcceptMode(QFileDialog::AcceptSave);

  if (fileDialog.exec() != QDialog::Accepted)
    return;

  auto selected = fileDialog.selectedFiles();
  if (selected.empty())
    return;

  auto settings = new QSettings(selected[0], QSettings::IniFormat);
  settings->setValue("state", this->saveState());
  settings->setValue("pos", this->pos());
  settings->setValue("size", this->size());
  settings->sync();
}

