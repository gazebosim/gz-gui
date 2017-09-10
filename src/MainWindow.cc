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

#include <ignition/common/Console.hh>
#include "ignition/gui/Iface.hh"
#include "ignition/gui/MainWindow.hh"
#include "ignition/gui/Plugin.hh"

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

  // File menu
  auto fileMenu = this->menuBar()->addMenu(tr("File"));

  // Configuration
  auto loadConfigAct = new QAction(tr("Load configuration"), this);
  loadConfigAct->setStatusTip(tr("Quit"));
  this->connect(loadConfigAct, SIGNAL(triggered()), this, SLOT(OnLoadConfig()));
  fileMenu->addAction(loadConfigAct);

  auto saveConfigAct = new QAction(tr("&Save configuration"), this);
  saveConfigAct->setStatusTip(tr("Quit"));
  this->connect(saveConfigAct, SIGNAL(triggered()), this, SLOT(OnSaveConfig()));
  fileMenu->addAction(saveConfigAct);

  fileMenu->addSeparator();

  // Stylesheet
  auto loadStylesheetAct = new QAction(tr("&Load stylesheet"), this);
  loadStylesheetAct->setStatusTip(tr("Choose a QSS file to load"));
  this->connect(loadStylesheetAct, SIGNAL(triggered()), this,
      SLOT(OnLoadStylesheet()));
  fileMenu->addAction(loadStylesheetAct);

  // Language
  auto languageMenu = fileMenu->addMenu(tr("Change language"));

  auto langGroup = new QActionGroup(this);
  langGroup->setExclusive(true);

  this->connect(langGroup, SIGNAL(triggered(QAction *)), this,
      SLOT(OnLanguage(QAction *)));

  // System locale
  auto defaultLocale = QLocale::system().name();
  defaultLocale.truncate(defaultLocale.lastIndexOf('_'));

  QDir dir("/home/louise/code/ign-gui/build/include/ignition/gui/languages");
  QStringList fileNames = dir.entryList(QStringList("translation_*.qm"));

  for (int i = 0; i < fileNames.size(); ++i)
  {
    // Locale (en) from filename (translation_en.qm)
    QString locale;
    locale = fileNames[i];
    locale.truncate(locale.lastIndexOf('.'));
    locale.remove(0, locale.indexOf('_') + 1);

    auto lang = QLocale::languageToString(QLocale(locale).language());

    auto action = new QAction(lang, this);
    action->setCheckable(true);
    action->setData(locale);

    languageMenu->addAction(action);
    langGroup->addAction(action);

    // set default translators and language checked
    if (defaultLocale == locale)
      action->setChecked(true);
  }

  fileMenu->addSeparator();

  // Quit
  auto quitAct = new QAction(tr("&Quit"), this);
  quitAct->setStatusTip(tr("Quit"));
  this->connect(quitAct, SIGNAL(triggered()), this, SLOT(close()));
  fileMenu->addAction(quitAct);

  // Plugins menu
  auto pluginsMenu = this->menuBar()->addMenu(tr("&Plugins"));

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

  if (!loadConfigFromFile(selected[0].toStdString()))
    return;

  if (!this->CloseAllDocks())
    return;

  addPluginsToWindow();
  applyConfig();
}

/////////////////////////////////////////////////
std::string MainWindow::CurrentConfig() const
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

  return config;
}

/////////////////////////////////////////////////
void MainWindow::OnSaveConfig()
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

  auto config = this->CurrentConfig();

  // Open the file
  std::ofstream out(selected[0].toStdString().c_str(), std::ios::out);

  if (!out)
  {
    QMessageBox msgBox;
    std::string str = "Unable to open file: " + selected[0].toStdString();
    str += ".\nCheck file permissions.";
    msgBox.setText(str.c_str());
    msgBox.exec();
  }
  else
    out << config;

  out.close();

  ignmsg << "Saved configuration [" << selected[0].toStdString() << "]"
         << std::endl;
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
void MainWindow::OnLanguage(QAction *_action)
{
  if (!_action)
    return;

  // Ask for confirmation
  std::string msg =
      "All plugins will be reloaded if the language is changed, \n"
      "would you like to continue?\n";

  QMessageBox msgBox(QMessageBox::Warning, QString("Change language"),
                     QString(msg.c_str()));

  auto cancelButton = msgBox.addButton("Cancel", QMessageBox::RejectRole);
  auto continueButton = msgBox.addButton("Continue", QMessageBox::AcceptRole);
  msgBox.setDefaultButton(continueButton);
  msgBox.setEscapeButton(cancelButton);
  msgBox.exec();
  if (msgBox.clickedButton() != continueButton)
    return;

  auto newLanguage = _action->data().toString();

  if (this->language == newLanguage)
    return;

  this->language = newLanguage;

  auto locale = QLocale(this->language);
  QLocale::setDefault(locale);

  auto languageName = QLocale::languageToString(locale.language());

  switchTranslator(QString("/home/louise/code/ign-gui/build/include/ignition/gui/languages/translation_%1.qm").arg(this->language).toStdString());

  loadConfigFromString(this->CurrentConfig());

  if (!this->CloseAllDocks())
    return;

  addPluginsToWindow();
  applyConfig();
}

