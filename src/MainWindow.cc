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

  auto loadConfigAct = new QAction(tr("Load configuration"), this);
  loadConfigAct->setStatusTip(tr("Quit"));
  this->connect(loadConfigAct, SIGNAL(triggered()), this, SLOT(OnLoadConfig()));
  fileMenu->addAction(loadConfigAct);

  auto saveConfigAct = new QAction(tr("&Save configuration"), this);
  saveConfigAct->setStatusTip(tr("Quit"));
  this->connect(saveConfigAct, SIGNAL(triggered()), this, SLOT(OnSaveConfig()));
  fileMenu->addAction(saveConfigAct);

  fileMenu->addSeparator();

  auto quitAct = new QAction(tr("&Quit"), this);
  quitAct->setStatusTip(tr("Quit"));
  this->connect(quitAct, SIGNAL(triggered()), this, SLOT(close()));
  fileMenu->addAction(quitAct);









  auto langGroup = new QActionGroup(this);
  langGroup->setExclusive(true);

  this->connect(langGroup, SIGNAL(triggered(QAction *)), this, SLOT(OnLanguage(QAction *)));

  // System locale
  auto defaultLocale = QLocale::system().name();
  defaultLocale.truncate(defaultLocale.lastIndexOf('_'));

  QDir dir("/home/louise/code/ign-gui/include/ignition/gui/languages");
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

    fileMenu->addAction(action);
    langGroup->addAction(action);

    // set default translators and language checked
    if (defaultLocale == locale)
      action->setChecked(true);
  }









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

  this->setCentralWidget(new QPushButton(QMainWindow::tr("Publish")));
}

/////////////////////////////////////////////////
MainWindow::~MainWindow()
{
}

/////////////////////////////////////////////////
bool MainWindow::CloseAllDocks()
{
  ignmsg << "Closing all docks" << std::endl;

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
  config += "</window>\n";

  // Plugins
  auto plugins = this->findChildren<Plugin *>();
  for (const auto plugin : plugins)
    config += plugin->ConfigStr();

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

  auto newLanguage = _action->data().toString();

  if (this->language == newLanguage)
    return;

  this->language = newLanguage;

  auto locale = QLocale(this->language);
  QLocale::setDefault(locale);

  auto languageName = QLocale::languageToString(locale.language());

  switchTranslator(QString("/home/louise/code/ign-gui/include/ignition/gui/languages/translation_%1.qm").arg(this->language).toStdString());
 // switchTranslator(this->translatorQt, QString("qt_%1.qm").arg(this->language));
}

/////////////////////////////////////////////////
void MainWindow::changeEvent(QEvent *_event)
{
  if (!_event)
    return;

  switch(_event->type())
  {
    // this event is sent if a translator is loaded
    case QEvent::LanguageChange:
igndbg << "A" << std::endl;
    break;

    // this event is sent if the system language changes
    case QEvent::LocaleChange:
    {
igndbg << "A" << std::endl;
      auto locale = QLocale::system().name();
      locale.truncate(locale.lastIndexOf('_'));
    }
    break;
    default:
    break;
  }

  QMainWindow::changeEvent(_event);
}

