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
#include "ignition/gui/Application.hh"
#include "ignition/gui/MainWindow.hh"
#include "ignition/gui/Plugin.hh"
#include "ignition/gui/qt.h"

namespace ignition
{
  namespace gui
  {
    class MainWindowPrivate
    {
      /// \brief Number of plugins on the window
      public: int pluginCount;

      /// \brief Pointer to quick window
      public: QQuickWindow *quickWindow{nullptr};

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
  // Make MainWindow functions available from all QML files (using root)
  App()->Engine()->rootContext()->setContextProperty("MainWindow", this);

  // Load QML and keep pointer to generated QQuickWindow
  std::string qmlFile("qrc:qml/Main.qml");
  App()->Engine()->load(QUrl(QString::fromStdString(qmlFile)));

  this->dataPtr->quickWindow = qobject_cast<QQuickWindow *>(
      App()->Engine()->rootObjects().value(0));
  if (!this->dataPtr->quickWindow)
  {
    ignerr << "Internal error: Failed to instantiate QML file [" << qmlFile
           << "]" << std::endl;
    return;
  }
}

/////////////////////////////////////////////////
MainWindow::~MainWindow()
{
}

/////////////////////////////////////////////////
void MainWindow::OnPluginClose()
{
  auto pluginName = this->sender()->objectName();
  App()->RemovePlugin(pluginName.toStdString());
}

/////////////////////////////////////////////////
QStringList MainWindow::PluginListModel() const
{
  QStringList pluginNames;
  auto plugins = App()->PluginList();
  for (auto const &path : plugins)
  {
    for (auto const &plugin : path.second)
    {
      // Remove lib and .so
      auto pluginName = plugin.substr(3, plugin.find(".") - 3);
      pluginNames.append(QString::fromStdString(pluginName));
    }
  }
  return pluginNames;
}

//////////////////////////////////////////////////
void MainWindow::OnLoadConfig(const QString &_path)
{
  auto localPath = QUrl(_path).toLocalFile();
  if (localPath.isEmpty())
    localPath = _path;

  App()->LoadConfig(localPath.toStdString());
}

/////////////////////////////////////////////////
void MainWindow::OnSaveConfig()
{
  this->SaveConfig(App()->DefaultConfigPath());
}

/////////////////////////////////////////////////
void MainWindow::OnSaveConfigAs(const QString &_path)
{
  auto localPath = QUrl(_path).toLocalFile();
  if (localPath.isEmpty())
    localPath = _path;
  this->SaveConfig(localPath.toStdString());
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
    std::string str = "Unable to open file: " + _path;
    str += ".\nCheck file permissions.";
    this->notify(QString::fromStdString(str));
  }
  else
    out << this->dataPtr->windowConfig.XMLString();

  std::string msg("Saved configuration to <b>" + _path + "</b>");

  this->notify(QString::fromStdString(msg));
  ignmsg << msg << std::endl;
}

/////////////////////////////////////////////////
void MainWindow::OnAddPlugin(QString _plugin)
{
  auto plugin = _plugin.toStdString();
  ignlog << "Add [" << plugin << "] via menu" << std::endl;

  App()->LoadPlugin(plugin);
}

///////////////////////////////////////////////////
bool MainWindow::ApplyConfig(const WindowConfig &_config)
{
  if (!this->dataPtr->quickWindow)
    return false;

  // Window position
  if (!_config.IsIgnoring("position_x") &&
      !_config.IsIgnoring("position_y") &&
      !_config.IsIgnoring("position") &&
      _config.posX >= 0 && _config.posY >= 0)
  {
//    this->move(_config.posX, _config.posY);
  }

  // Window size
  if (!_config.IsIgnoring("width") &&
      !_config.IsIgnoring("height") &&
      !_config.IsIgnoring("size") &&
      _config.width >= 0 && _config.height >= 0)
  {
    this->QuickWindow()->resize(_config.width, _config.height);
  }

  // Docks state
  if (!_config.IsIgnoring("state") && !_config.state.isEmpty())
  {
//    if (!this->restoreState(_config.state))
//      ignwarn << "Failed to restore state" << std::endl;
  }

  // Style
  if (!_config.IsIgnoring("style"))
  {
    this->SetMaterialTheme(QString::fromStdString(_config.materialTheme));
    this->SetMaterialPrimary(QString::fromStdString(_config.materialPrimary));
    this->SetMaterialAccent(QString::fromStdString(_config.materialAccent));
  }

  // Menus
  this->SetShowPanel(_config.showPanel);
  this->SetShowDefaultPanelOpts(_config.showDefaultPanelOpts);

  // Plugins menu
//  if (auto menu = this->findChild<QMenu *>("pluginsMenu"))
//  {
//    for (auto action : menu->actions())
//    {
//      action->setVisible(_config.pluginsFromPaths ||
//          std::find(_config.showPlugins.begin(),
//                    _config.showPlugins.end(),
//                    action->text().toStdString()) !=
//                    _config.showPlugins.end());
//    }
//
//    for (auto plugin : _config.showPlugins)
//    {
//      bool exists = false;
//      for (auto action : menu->actions())
//      {
//        if (action->text().toStdString() == plugin)
//        {
//          exists = true;
//          break;
//        }
//      }
//
//      if (!exists)
//      {
//        ignwarn << "Requested to show plugin [" << plugin <<
//            "] but it doesn't exist." << std::endl;
//      }
//    }
//  }

  // Keep a copy
  this->dataPtr->windowConfig = _config;

  return true;
}

/////////////////////////////////////////////////
WindowConfig MainWindow::CurrentWindowConfig() const
{
  if (!this->dataPtr->quickWindow)
    return this->dataPtr->windowConfig;

  WindowConfig config;

  // Position
//  config.posX = this->pos().x();
//  config.posY = this->pos().y();

  // Size
  config.width = this->QuickWindow()->width();
  config.height = this->QuickWindow()->height();

  // Docks state
//  config.state = this->saveState();

  // Style
  config.materialTheme = this->QuickWindow()->property("materialTheme")
      .toString().toStdString() == "0" ? "Light" : "Dark";
  config.materialPrimary = this->QuickWindow()->property("materialPrimary")
      .toString().toStdString();
  config.materialAccent =
      this->QuickWindow()->property("materialAccent").toString().toStdString();

  // Menus configuration and ignored properties are kept the same as the
  // initial ones. They might have been changed programatically but we
  // don't guarantee that will be saved.
  config.showPanel = this->dataPtr->windowConfig.showPanel;
  config.showDefaultPanelOpts =
      this->dataPtr->windowConfig.showDefaultPanelOpts;
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
    widthElem->QueryIntText(&this->width);

  if (auto heightElem = winElem->FirstChildElement("height"))
    heightElem->QueryIntText(&this->height);

  // Docks state
  if (auto stateElem = winElem->FirstChildElement("state"))
  {
    auto text = stateElem->GetText();
    this->state = QByteArray::fromBase64(text);
  }

  // Style
  if (auto styleElem = winElem->FirstChildElement("style"))
  {
    auto mTheme = styleElem->Attribute("material_theme");
    if (mTheme)
    {
      this->materialTheme = mTheme;
    }
    auto mPrimary = styleElem->Attribute("material_primary");
    if (mPrimary)
    {
      this->materialPrimary = mPrimary;
    }
    auto mAccent = styleElem->Attribute("material_accent");
    if (mAccent)
    {
      this->materialAccent = mAccent;
    }
  }

  // Menus
  if (auto menusElem = winElem->FirstChildElement("menus"))
  {
    // Panel
    if (auto panelElem = menusElem->FirstChildElement("panel"))
    {
      // Visible
      if (panelElem->Attribute("visible"))
      {
        bool visible = true;
        panelElem->QueryBoolAttribute("visible", &visible);
        this->showPanel = visible;
      }
      // Default
      if (panelElem->Attribute("default"))
      {
        bool def = true;
        panelElem->QueryBoolAttribute("default", &def);
        this->showDefaultPanelOpts = def;
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
        // this->menuVisibilityMap["plugins"] = visible;
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

  // Style
  if (!this->IsIgnoring("style"))
  {
    auto elem = doc.NewElement("style");
    elem->SetAttribute("material_theme", this->materialTheme.c_str());
    elem->SetAttribute("material_primary", this->materialPrimary.c_str());
    elem->SetAttribute("material_accent", this->materialAccent.c_str());
    windowElem->InsertEndChild(elem);
  }

  // Menus
  {
    auto menusElem = doc.NewElement("menus");
    windowElem->InsertEndChild(menusElem);

    // Panel
    {
      auto elem = doc.NewElement("panel");

      // Visible
      elem->SetAttribute("visible", this->showPanel);

      // Default
      elem->SetAttribute("default", this->showDefaultPanelOpts);

      menusElem->InsertEndChild(elem);
    }

    // Plugins
    {
      auto elem = doc.NewElement("plugins");

      // Visible
//      auto m = this->menuVisibilityMap.find("plugins");
//      if (m != this->menuVisibilityMap.end())
//      {
//        elem->SetAttribute("visible", m->second);
//      }

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

/////////////////////////////////////////////////
int MainWindow::PluginCount() const
{
  return this->dataPtr->pluginCount;
}

/////////////////////////////////////////////////
void MainWindow::SetPluginCount(const int _pluginCount)
{
  this->dataPtr->pluginCount = _pluginCount;
  this->PluginCountChanged();
}

/////////////////////////////////////////////////
QString MainWindow::MaterialTheme() const
{
  return QString::fromStdString(this->dataPtr->windowConfig.materialTheme);
}

/////////////////////////////////////////////////
void MainWindow::SetMaterialTheme(const QString &_materialTheme)
{
  this->dataPtr->windowConfig.materialTheme = _materialTheme.toStdString();
  this->MaterialThemeChanged();
}

/////////////////////////////////////////////////
QString MainWindow::MaterialPrimary() const
{
  return QString::fromStdString(this->dataPtr->windowConfig.materialPrimary);
}

/////////////////////////////////////////////////
void MainWindow::SetMaterialPrimary(const QString &_materialPrimary)
{
  this->dataPtr->windowConfig.materialPrimary = _materialPrimary.toStdString();
  this->MaterialPrimaryChanged();
}

/////////////////////////////////////////////////
QString MainWindow::MaterialAccent() const
{
  return QString::fromStdString(this->dataPtr->windowConfig.materialAccent);
}

/////////////////////////////////////////////////
void MainWindow::SetMaterialAccent(const QString &_materialAccent)
{
  this->dataPtr->windowConfig.materialAccent = _materialAccent.toStdString();
  this->MaterialAccentChanged();
}

/////////////////////////////////////////////////
QQuickWindow *MainWindow::QuickWindow() const
{
  return this->dataPtr->quickWindow;
}

/////////////////////////////////////////////////
bool MainWindow::ShowPanel() const
{
  return this->dataPtr->windowConfig.showPanel;
}

/////////////////////////////////////////////////
void MainWindow::SetShowPanel(const bool _showPanel)
{
  this->dataPtr->windowConfig.showPanel = _showPanel;
  this->ShowPanelChanged();
}

/////////////////////////////////////////////////
bool MainWindow::ShowDefaultPanelOpts() const
{
  return this->dataPtr->windowConfig.showDefaultPanelOpts;
}

/////////////////////////////////////////////////
void MainWindow::SetShowDefaultPanelOpts(const bool _showDefaultPanelOpts)
{
  this->dataPtr->windowConfig.showDefaultPanelOpts =
      _showDefaultPanelOpts;
  this->ShowDefaultPanelOptsChanged();
}
