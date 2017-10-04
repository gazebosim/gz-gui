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

#include <tinyxml2.h>
#include <string>

#include <ignition/common/Console.hh>
#include <ignition/common/Filesystem.hh>
#include "ignition/gui/Iface.hh"
#include "ignition/gui/MainWindow.hh"
#include "ignition/gui/Plugin.hh"
#include "ignition/gui/qt.h"

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

/// \brief Strip last component from a path.
/// \return Original path without its last component.
/// \ToDo: Move this function to ignition::common::Filesystem
std::string dirName(const std::string &_path)
{
  std::size_t found = _path.find_last_of("/\\");
  return _path.substr(0, found);
}

/////////////////////////////////////////////////
MainWindow::MainWindow()
  : dataPtr(new MainWindowPrivate)
{
  this->setObjectName("mainWindow");

  // Title
  std::string title = "Ignition GUI";
  this->setWindowIconText(tr(title.c_str()));
  this->setWindowTitle(tr(title.c_str()));

  // File menu
  auto fileMenu = this->menuBar()->addMenu(tr("&File"));
  fileMenu->setObjectName("fileMenu");

  auto loadConfigAct = new QAction(tr("&Load configuration"), this);
  loadConfigAct->setStatusTip(tr("Quit"));
  this->connect(loadConfigAct, SIGNAL(triggered()), this, SLOT(OnLoadConfig()));
  fileMenu->addAction(loadConfigAct);

  auto saveConfigAct = new QAction(tr("&Save configuration"), this);
  saveConfigAct->setStatusTip(tr("Save configuration"));
  this->connect(saveConfigAct, SIGNAL(triggered()), this, SLOT(OnSaveConfig()));
  fileMenu->addAction(saveConfigAct);

  auto saveConfigAsAct = new QAction(tr("Save configuration as"), this);
  saveConfigAsAct->setStatusTip(tr("Save configuration as"));
  this->connect(saveConfigAsAct, SIGNAL(triggered()), this,
    SLOT(OnSaveConfigAs()));
  fileMenu->addAction(saveConfigAsAct);

  fileMenu->addSeparator();

  auto loadStylesheetAct = new QAction(tr("&Load stylesheet"), this);
  loadStylesheetAct->setStatusTip(tr("Choose a QSS file to load"));
  this->connect(loadStylesheetAct, SIGNAL(triggered()), this,
      SLOT(OnLoadStylesheet()));
  fileMenu->addAction(loadStylesheetAct);

  fileMenu->addSeparator();

  auto quitAct = new QAction(tr("&Quit"), this);
  quitAct->setStatusTip(tr("Quit"));
  this->connect(quitAct, SIGNAL(triggered()), this, SLOT(close()));
  fileMenu->addAction(quitAct);

  // Plugins menu
  auto pluginsMenu = this->menuBar()->addMenu(tr("&Plugins"));
  pluginsMenu->setObjectName("pluginsMenu");

  auto pluginMapper = new QSignalMapper(this);
  this->connect(pluginMapper, SIGNAL(mapped(QString)),
      this, SLOT(OnAddPlugin(QString)));

  auto plugins = getPluginList();
  for (auto const &path : plugins)
  {
    for (auto const &plugin : path.second)
    {
      auto pluginName = plugin.substr(3, plugin.find(".") - 3);

      auto act = new QAction(QString::fromStdString(pluginName), this);
      this->connect(act, SIGNAL(triggered()), pluginMapper, SLOT(map()));
      pluginMapper->setMapping(act, QString::fromStdString(plugin));
      pluginsMenu->addAction(act);
    }
  }

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
bool MainWindow::CloseAllDocks()
{
  igndbg << "Closing all docks" << std::endl;

  auto docks = this->findChildren<QDockWidget *>();
  for (auto dock : docks)
  {
    dock->close();
    dock->setParent(new QWidget());
  }

  QCoreApplication::processEvents();

  return true;
}

/////////////////////////////////////////////////
void MainWindow::OnLoadConfig()
{
  QFileDialog fileDialog(this, tr("Load configuration"), QDir::homePath(),
      tr("*.config"));
  fileDialog.setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint |
      Qt::WindowStaysOnTopHint | Qt::CustomizeWindowHint);

  if (fileDialog.exec() != QDialog::Accepted)
    return;

  auto selected = fileDialog.selectedFiles();
  if (selected.empty())
    return;

  if (!loadConfig(selected[0].toStdString()))
    return;

  if (!this->CloseAllDocks())
    return;

  addPluginsToWindow();
  applyConfig();
}

/////////////////////////////////////////////////
void MainWindow::OnSaveConfig()
{
  this->SaveImpl(defaultConfigPath());
}

/////////////////////////////////////////////////
void MainWindow::OnSaveConfigAs()
{
  QFileDialog fileDialog(this, tr("Save configuration"), QDir::homePath(),
      tr("*.config"));
  fileDialog.setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint |
      Qt::WindowStaysOnTopHint | Qt::CustomizeWindowHint);
  fileDialog.setAcceptMode(QFileDialog::AcceptSave);

  if (fileDialog.exec() != QDialog::Accepted)
    return;

  auto selected = fileDialog.selectedFiles();
  if (selected.empty())
    return;

  this->SaveImpl(selected[0].toStdString());
}

/////////////////////////////////////////////////
void MainWindow::SaveImpl(const std::string &_path)
{
  std::string config = "<?xml version=\"1.0\"?>\n\n";

  // Window settings
  config += "<window>\n";
  config += "  <state>";
  config += this->saveState().toBase64().toStdString();
  config += "</state>\n";
  config += "  <position_x>";
  config += std::to_string(this->pos().x());
  config += "</position_x>\n";
  config += "  <position_y>";
  config += std::to_string(this->pos().y());
  config += "</position_y>\n";
  config += "  <width>";
  config += std::to_string(this->width());
  config += "</width>\n";
  config += "  <height>";
  config += std::to_string(this->height());
  config += "</height>\n";
  config += "  <stylesheet>\n";
  config += static_cast<QApplication *>(
      QApplication::instance())->styleSheet().toStdString();
  config += "  </stylesheet>\n";
  config += "</window>\n";

  // Plugins
  auto plugins = this->findChildren<Plugin *>();
  for (const auto plugin : plugins)
    config += plugin->ConfigStr();

  // Create the intermediate directories if needed.
  // We check for errors when we try to open the file.
  auto dirname = dirName(_path);
  ignition::common::createDirectories(dirname);

  // Open the file
  std::ofstream out(_path.c_str(), std::ios::out);
  if (!out)
  {
    QMessageBox msgBox;
    std::string str = "Unable to open file: " + _path;
    str += ".\nCheck file permissions.";
    msgBox.setText(str.c_str());
    msgBox.exec();
  }
  else
    out << config;

  ignmsg << "Saved configuration [" << _path << "]" << std::endl;
}

/////////////////////////////////////////////////
void MainWindow::OnLoadStylesheet()
{
  QFileDialog fileDialog(this, tr("Load stylesheet"), QDir::homePath(),
      tr("*.qss"));
  fileDialog.setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint |
      Qt::WindowStaysOnTopHint | Qt::CustomizeWindowHint);

  if (fileDialog.exec() != QDialog::Accepted)
    return;

  auto selected = fileDialog.selectedFiles();
  if (selected.empty())
    return;

  setStyleFromFile(selected[0].toStdString());
}

/////////////////////////////////////////////////
void MainWindow::OnAddPlugin(QString _plugin)
{
  auto plugin = _plugin.toStdString();
  ignlog << "Add [" << plugin << "] via menu" << std::endl;

  loadPlugin(plugin);
  addPluginsToWindow();
}

