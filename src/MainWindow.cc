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
      /// \brief Configuration for this window.
      public: WindowConfig windowConfig;

      /// \brief Counts the times the window has been painted
      public: unsigned int paintCount{0};

      /// \brief Minimum number of paint events to consider the window to be
      /// fully initialized.
      public: const unsigned int paintCountMin{20};
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

  // Ubuntu Xenial + Unity: the native menubar is not registering shortcuts,
  // so we register the shortcuts independently of actions
  std::vector<QShortcut *> shortcuts;

  // Title
  std::string title = "Ignition GUI";
  this->setWindowIconText(tr(title.c_str()));
  this->setWindowTitle(tr(title.c_str()));

  // File menu
  auto fileMenu = this->menuBar()->addMenu(tr("&File"));
  fileMenu->setObjectName("fileMenu");

  auto loadConfigAct = new QAction(tr("&Load configuration"), this);
  loadConfigAct->setStatusTip(tr("Load configuration"));
  this->connect(loadConfigAct, SIGNAL(triggered()), this, SLOT(OnLoadConfig()));
  fileMenu->addAction(loadConfigAct);
  shortcuts.push_back(new QShortcut(Qt::CTRL + Qt::Key_O, this,
      SLOT(OnLoadConfig())));

  auto saveConfigAct = new QAction(tr("&Save configuration"), this);
  saveConfigAct->setStatusTip(tr("Save configuration"));
  this->connect(saveConfigAct, SIGNAL(triggered()), this, SLOT(OnSaveConfig()));
  fileMenu->addAction(saveConfigAct);
  shortcuts.push_back(new QShortcut(Qt::CTRL + Qt::Key_S, this,
      SLOT(OnSaveConfig())));

  auto saveConfigAsAct = new QAction(tr("Save configuration as"), this);
  saveConfigAsAct->setStatusTip(tr("Save configuration as"));
  this->connect(saveConfigAsAct, SIGNAL(triggered()), this,
    SLOT(OnSaveConfigAs()));
  fileMenu->addAction(saveConfigAsAct);
  shortcuts.push_back(new QShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_S, this,
      SLOT(OnSaveConfigAs())));

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
  shortcuts.push_back(new QShortcut(Qt::CTRL + Qt::Key_Q, this, SLOT(close())));

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
void MainWindow::paintEvent(QPaintEvent *_event)
{
  this->dataPtr->paintCount++;
  if (this->dataPtr->paintCount == this->dataPtr->paintCountMin)
  {
    this->dataPtr->windowConfig = this->CurrentWindowConfig();
  }
  _event->accept();
}

/////////////////////////////////////////////////
void MainWindow::closeEvent(QCloseEvent *_event)
{
  if (this->dataPtr->paintCount < this->dataPtr->paintCountMin ||
      this->dataPtr->windowConfig.XMLString() ==
      this->CurrentWindowConfig().XMLString())
  {
    _event->accept();
    return;
  }

  // Ask for confirmation
  std::string msg = "There are unsaved changes. \n\n";

  QMessageBox msgBox(QMessageBox::Warning, QString("Save configuration?"),
      QString(msg.c_str()), QMessageBox::NoButton, this);
  msgBox.setWindowFlags(Qt::Window | Qt::WindowTitleHint |
      Qt::WindowStaysOnTopHint | Qt::CustomizeWindowHint);

  auto saveButton = msgBox.addButton("Save as default",
      QMessageBox::AcceptRole);
  saveButton->setObjectName("closeConfirmationDialogSaveButton");
  saveButton->setToolTip(QString::fromStdString(
      "Save to default config file \"" + defaultConfigPath() + "\""));
  msgBox.setDefaultButton(saveButton);
  saveButton->setMinimumWidth(160);

  auto saveAsButton = msgBox.addButton("Save as...", QMessageBox::AcceptRole);
  saveAsButton->setObjectName("closeConfirmationDialogSaveAsButton");
  saveAsButton->setToolTip("Choose a file on your computer");

  auto cancelButton = msgBox.addButton("Cancel", QMessageBox::AcceptRole);
  cancelButton->setObjectName("closeConfirmationDialogCancelButton");
  msgBox.setEscapeButton(cancelButton);
  cancelButton->setToolTip("Don't close window");

  auto closeButton = msgBox.addButton("Close without saving",
      QMessageBox::AcceptRole);
  closeButton->setObjectName("closeConfirmationDialogCloseButton");
  closeButton->setToolTip("Close without saving");
  closeButton->setMinimumWidth(180);

  msgBox.show();
  msgBox.exec();

  // User doesn't want to close window anymore
  if (msgBox.clickedButton() == cancelButton)
  {
    _event->ignore();
    return;
  }

  // Save to default config
  if (msgBox.clickedButton() == saveButton)
  {
    this->OnSaveConfig();
  }

  // Save to custom file
  if (msgBox.clickedButton() == saveAsButton)
  {
    this->OnSaveConfigAs();
  }
  _event->accept();
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
  this->SaveConfig(defaultConfigPath());
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

  this->SaveConfig(selected[0].toStdString());
}

/////////////////////////////////////////////////
void MainWindow::SaveConfig(const std::string &_path)
{
  this->dataPtr->windowConfig = this->CurrentWindowConfig();

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
    out << this->dataPtr->windowConfig.XMLString();

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

/////////////////////////////////////////////////
bool MainWindow::ApplyConfig(const WindowConfig &_config)
{
  // Window position
  if (!_config.IsIgnoring("position_x") &&
      !_config.IsIgnoring("position_y") &&
      !_config.IsIgnoring("position") &&
      _config.posX >= 0 && _config.posY >= 0)
  {
    this->move(_config.posX, _config.posY);
  }

  // Window size
  if (!_config.IsIgnoring("width") &&
      !_config.IsIgnoring("height") &&
      !_config.IsIgnoring("size") &&
      _config.width >= 0 && _config.height >= 0)
  {
    this->resize(_config.width, _config.height);
  }

  // Docks state
  if (!_config.IsIgnoring("state") && !_config.state.isEmpty())
  {
    if (!this->restoreState(_config.state))
      ignwarn << "Failed to restore state" << std::endl;
  }

  // Stylesheet
  if (!_config.IsIgnoring("stylesheet"))
    setStyleFromString(_config.styleSheet);

  // Hide menus
  for (auto visible : _config.menuVisibilityMap)
  {
    if (auto menu = this->findChild<QMenu *>(
        QString::fromStdString(visible.first + "Menu")))
    {
      menu->menuAction()->setVisible(visible.second);
    }
  }

  // Plugins menu
  if (auto menu = this->findChild<QMenu *>("pluginsMenu"))
  {
    for (auto action : menu->actions())
    {
      action->setVisible(_config.pluginsFromPaths ||
          std::find(_config.showPlugins.begin(),
                    _config.showPlugins.end(),
                    action->text().toStdString()) !=
                    _config.showPlugins.end());
    }

    for (auto plugin : _config.showPlugins)
    {
      bool exists = false;
      for (auto action : menu->actions())
      {
        if (action->text().toStdString() == plugin)
        {
          exists = true;
          break;
        }
      }

      if (!exists)
      {
        ignwarn << "Requested to show plugin [" << plugin <<
            "] but it doesn't exist." << std::endl;
      }
    }
  }

  // Keep a copy
  this->dataPtr->windowConfig = _config;

  QCoreApplication::processEvents();

  return true;
}

/////////////////////////////////////////////////
WindowConfig MainWindow::CurrentWindowConfig() const
{
  WindowConfig config;

  // Position
  config.posX = this->pos().x();
  config.posY = this->pos().y();

  // Size
  config.width = this->width();
  config.height = this->height();

  // Docks state
  config.state = this->saveState();

  // Stylesheet
  config.styleSheet = static_cast<QApplication *>(
      QApplication::instance())->styleSheet().toStdString();

  // Menus configuration and ignored properties are kept the same as the
  // initial ones. They might have been changed programatically but we
  // don't guarantee that will be saved.
  config.menuVisibilityMap = this->dataPtr->windowConfig.menuVisibilityMap;
  config.pluginsFromPaths = this->dataPtr->windowConfig.pluginsFromPaths;
  config.showPlugins = this->dataPtr->windowConfig.showPlugins;
  config.ignoredProps = this->dataPtr->windowConfig.ignoredProps;

  // Plugins
  auto plugins = this->findChildren<Plugin *>();
  for (const auto plugin : plugins)
    config.plugins += plugin->ConfigStr();

  return config;
}

/////////////////////////////////////////////////
bool WindowConfig::MergeFromXML(const std::string &_windowXml)
{
  auto screenSize = QApplication::desktop()->screenGeometry().size();

  // TinyXml element from string
  tinyxml2::XMLDocument doc;
  doc.Parse(_windowXml.c_str());
  auto winElem = doc.FirstChildElement("window");

  if (!winElem)
    return false;

  // Position
  if (auto xElem = winElem->FirstChildElement("position_x"))
    xElem->QueryIntText(&this->posX);

  if (auto yElem = winElem->FirstChildElement("position_y"))
    yElem->QueryIntText(&this->posY);

  // Size
  if (auto widthElem = winElem->FirstChildElement("width"))
  {
    int newWidth;
    widthElem->QueryIntText(&newWidth);

    if (newWidth > screenSize.width())
    {
      ignwarn << "Asked window width [" << newWidth << "] clamped to ["
              << screenSize.width() << "] to fit screen." << std::endl;
      newWidth = screenSize.width();
    }
    this->width = newWidth;
  }

  if (auto heightElem = winElem->FirstChildElement("height"))
  {
    int newHeight;
    heightElem->QueryIntText(&newHeight);

    if (newHeight > screenSize.height())
    {
      ignwarn << "Asked window height [" << newHeight << "] clamped to ["
              << screenSize.height() << "] to fit screen." << std::endl;
      newHeight = screenSize.height();
    }
    this->height = newHeight;
  }

  // Docks state
  if (auto stateElem = winElem->FirstChildElement("state"))
  {
    auto text = stateElem->GetText();
    this->state = QByteArray::fromBase64(text);
  }

  // Stylesheet
  if (auto styleElem = winElem->FirstChildElement("stylesheet"))
  {
    if (auto txt = styleElem->GetText())
    {
      setStyleFromString(txt);
    }
    // empty string
    else
    {
      setStyleFromString("");
    }
  }

  // Menus
  if (auto menusElem = winElem->FirstChildElement("menus"))
  {
    // File
    if (auto fileElem = menusElem->FirstChildElement("file"))
    {
      // Visible
      if (fileElem->Attribute("visible"))
      {
        bool visible = true;
        fileElem->QueryBoolAttribute("visible", &visible);
        this->menuVisibilityMap["file"] = visible;
      }
    }

    // Plugins
    if (auto pluginsElem = menusElem->FirstChildElement("plugins"))
    {
      // Visible
      if (pluginsElem->Attribute("visible"))
      {
        bool visible = true;
        pluginsElem->QueryBoolAttribute("visible", &visible);
        this->menuVisibilityMap["plugins"] = visible;
      }

      // From paths
      if (pluginsElem->Attribute("from_paths"))
      {
        bool fromPaths = false;
        pluginsElem->QueryBoolAttribute("from_paths", &fromPaths);
        this->pluginsFromPaths = fromPaths;
      }

      // Show individual plugins
      for (auto showElem = pluginsElem->FirstChildElement("show");
          showElem != nullptr;
          showElem = showElem->NextSiblingElement("show"))
      {
        if (auto pluginName = showElem->GetText())
          this->showPlugins.push_back(pluginName);
      }
    }
  }

  // Ignore
  for (auto ignoreElem = winElem->FirstChildElement("ignore");
      ignoreElem != nullptr;
      ignoreElem = ignoreElem->NextSiblingElement("ignore"))
  {
    if (auto prop = ignoreElem->GetText())
      this->ignoredProps.insert(prop);
  }

  return true;
}

/////////////////////////////////////////////////
std::string WindowConfig::XMLString() const
{
  tinyxml2::XMLDocument doc;

  // Window
  auto windowElem = doc.NewElement("window");
  doc.InsertEndChild(windowElem);

  // Position
  if (!this->IsIgnoring("position") && !this->IsIgnoring("position_x"))
  {
    auto elem = doc.NewElement("position_x");
    elem->SetText(std::to_string(this->posX).c_str());
    windowElem->InsertEndChild(elem);
  }

  if (!this->IsIgnoring("position") && !this->IsIgnoring("position_y"))
  {
    auto elem = doc.NewElement("position_y");
    elem->SetText(std::to_string(this->posY).c_str());
    windowElem->InsertEndChild(elem);
  }

  // Docks state
  if (!this->IsIgnoring("state"))
  {
    auto elem = doc.NewElement("state");
    elem->SetText(this->state.toBase64().toStdString().c_str());
    windowElem->InsertEndChild(elem);
  }

  // Size
  if (!this->IsIgnoring("size") && !this->IsIgnoring("width"))
  {
    auto elem = doc.NewElement("width");
    elem->SetText(std::to_string(this->width).c_str());
    windowElem->InsertEndChild(elem);
  }

  if (!this->IsIgnoring("size") && !this->IsIgnoring("height"))
  {
    auto elem = doc.NewElement("height");
    elem->SetText(std::to_string(this->height).c_str());
    windowElem->InsertEndChild(elem);
  }

  // Stylesheet
  if (!this->IsIgnoring("stylesheet"))
  {
    auto elem = doc.NewElement("stylesheet");
    elem->SetText(this->styleSheet.c_str());
    windowElem->InsertEndChild(elem);
  }

  // Menus
  {
    auto menusElem = doc.NewElement("menus");
    windowElem->InsertEndChild(menusElem);

    // File
    {
      auto elem = doc.NewElement("file");

      // Visible
      auto m = this->menuVisibilityMap.find("file");
      if (m != this->menuVisibilityMap.end())
      {
        elem->SetAttribute("visible", m->second);
      }
      menusElem->InsertEndChild(elem);
    }

    // Plugins
    {
      auto elem = doc.NewElement("plugins");

      // Visible
      auto m = this->menuVisibilityMap.find("plugins");
      if (m != this->menuVisibilityMap.end())
      {
        elem->SetAttribute("visible", m->second);
      }

      // From paths
      elem->SetAttribute("from_paths", this->pluginsFromPaths);

      // Show
      for (const auto &show : this->showPlugins)
      {
        auto showElem = doc.NewElement("show");
        showElem->SetText(show.c_str());
        elem->InsertEndChild(showElem);
      }

      menusElem->InsertEndChild(elem);
    }

    windowElem->InsertEndChild(menusElem);
  }

  // Ignored properties
  {
    for (const auto &ignore : this->ignoredProps)
    {
      auto ignoreElem = doc.NewElement("ignore");
      ignoreElem->SetText(ignore.c_str());
      windowElem->InsertEndChild(ignoreElem);
    }
  }

  tinyxml2::XMLPrinter printer;
  doc.Print(&printer);

  std::string config = "<?xml version=\"1.0\"?>\n\n";
  config += printer.CStr();
  config += this->plugins;

  return config;
}

/////////////////////////////////////////////////
bool WindowConfig::IsIgnoring(const std::string &_prop) const
{
  return this->ignoredProps.find(_prop) != this->ignoredProps.end();
}

