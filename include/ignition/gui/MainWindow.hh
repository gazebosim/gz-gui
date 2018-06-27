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
#ifndef IGNITION_GUI_MAINWINDOW_HH_
#define IGNITION_GUI_MAINWINDOW_HH_

#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include <ignition/common/Console.hh>

#include "ignition/gui/qt.h"
#include "ignition/gui/Export.hh"

namespace ignition
{
  namespace gui
  {
    class MainWindowPrivate;
    struct WindowConfig;

    /// \brief The main window class creates a QQuickWindow and acts as an
    /// interface which provides properties and functions which can be called
    /// from MainWindow.qml
    class IGNITION_GUI_VISIBLE MainWindow : public QObject
    {
      Q_OBJECT

      /// \brief Number of plugins currently instantiated inside the window.
      Q_PROPERTY(
        int pluginCount
        READ PluginCount
        WRITE SetPluginCount
        NOTIFY PluginCountChanged
      )

      /// \brief Material theme (Light / Dark)
      Q_PROPERTY(
        QString materialTheme
        READ MaterialTheme
        WRITE SetMaterialTheme
        NOTIFY MaterialThemeChanged
      )

      /// \brief Material primary color (Pre-defined color name or hex value)
      Q_PROPERTY(
        QString materialPrimary
        READ MaterialPrimary
        WRITE SetMaterialPrimary
        NOTIFY MaterialPrimaryChanged
      )

      /// \brief Material accent color (Pre-defined color name or hex value)
      Q_PROPERTY(
        QString materialAccent
        READ MaterialAccent
        WRITE SetMaterialAccent
        NOTIFY MaterialAccentChanged
      )

      /// \brief
      Q_PROPERTY(
        bool showPanel
        READ ShowPanel
        WRITE SetShowPanel
        NOTIFY ShowPanelChanged
      )

      /// \brief
      Q_PROPERTY(
        bool showDefaultPanelOpts
        READ ShowDefaultPanelOpts
        WRITE SetShowDefaultPanelOpts
        NOTIFY ShowDefaultPanelOptsChanged
      )

      /// \brief
      Q_PROPERTY(
        bool showPluginMenu
        READ ShowPluginMenu
        WRITE SetShowPluginMenu
        NOTIFY ShowPluginMenuChanged
      )

      /// \brief Constructor
      public: MainWindow();

      /// \brief Destructor
      public: virtual ~MainWindow();

      /// \brief Get the QtQuick window created by this object
      /// \return Pointer to the QtQuick window
      public: QQuickWindow *QuickWindow() const;

      /// \brief Save current window and plugin configuration to a file on disk.
      /// Will open an error dialog in case it's not possible to write to the
      /// path.
      /// \param[in] _path The full destination path including filename.
      public: void SaveConfig(const std::string &_path);

      /// \brief Apply a WindowConfig to this window and keep a copy of it.
      /// \param[in] _config The configuration to apply.
      /// \return True if successful.
      public: bool ApplyConfig(const WindowConfig &_config);

      /// \brief Get the current window configuration.
      /// \return Updated window config
      public: WindowConfig CurrentWindowConfig() const;

      /// \brief Callback when user requests to close a plugin
      public slots: void OnPluginClose();

      /// \brief Add a plugin to the window.
      /// \param [in] _plugin Plugin filename
      public slots: void OnAddPlugin(QString _plugin);

      /// \brief Return a list of all plugin names found
      /// \return List with plugin names
      public: Q_INVOKABLE QStringList PluginListModel() const;

      /// \brief Returns the number of plugins current instantiated in the
      /// window.
      /// \return Number of plugins
      public: Q_INVOKABLE int PluginCount() const;

      /// \brief Sets the number of plugins current instantiated in the
      /// window.
      /// \param[in] _pluginCount Number of plugins
      public: Q_INVOKABLE void SetPluginCount(const int _pluginCount);

      /// \brief Returns the material theme.
      /// \return Theme (Light / Dark)
      public: Q_INVOKABLE QString MaterialTheme() const;

      /// \brief Sets the material theme
      /// \param[in] _materialTheme Theme (Light / Dark)
      public: Q_INVOKABLE void SetMaterialTheme(
          const QString &_materialTheme);

      /// \brief Returns the material primary color.
      /// \return Primary color
      public: Q_INVOKABLE QString MaterialPrimary() const;

      /// \brief Sets the material primary color
      /// \param[in] _materialPrimary Primary color
      public: Q_INVOKABLE void SetMaterialPrimary(
          const QString &_materialPrimary);

      /// \brief Returns the material accent color.
      /// \return Accent color
      public: Q_INVOKABLE QString MaterialAccent() const;

      /// \brief Sets the material accent color
      /// \param[in] _materialAccent Accent color
      public: Q_INVOKABLE void SetMaterialAccent(
          const QString &_materialAccent);

      /// \brief
      /// \return
      public: Q_INVOKABLE bool ShowPanel() const;

      /// \brief
      /// \param[in] _showPanel
      public: Q_INVOKABLE void SetShowPanel(const bool _showPanel);

      /// \brief
      /// \return
      public: Q_INVOKABLE bool ShowDefaultPanelOpts() const;

      /// \brief
      /// \param[in] _showDefaultPanelOpts
      public: Q_INVOKABLE void SetShowDefaultPanelOpts(
          const bool _showDefaultPanelOpts);

      /// \brief
      /// \return
      public: Q_INVOKABLE bool ShowPluginMenu() const;

      /// \brief
      /// \param[in] _showPluginMenu
      public: Q_INVOKABLE void SetShowPluginMenu(const bool _showPluginMenu);

      /// \brief Callback when load configuration is selected
      public slots: void OnLoadConfig(const QString &_path);

      /// \brief Callback when "save configuration" is selected
      public slots: void OnSaveConfig();

      /// \brief Callback when "save configuration as" is selected
      public slots: void OnSaveConfigAs(const QString &_path);

      /// \brief Notifies when the number of plugins has changed.
      signals: void PluginCountChanged();

      /// \brief Notifies when the theme has changed.
      signals: void MaterialThemeChanged();

      /// \brief Notifies when the primary color has changed.
      signals: void MaterialPrimaryChanged();

      /// \brief Notifies when the accent color has changed.
      signals: void MaterialAccentChanged();

      /// \brief
      signals: void ShowPanelChanged();

      /// \brief
      signals: void ShowDefaultPanelOptsChanged();

      /// \brief
      signals: void ShowPluginMenuChanged();

      /// \brief
      signals: void configChanged();

      /// \brief Displays a message to the user
      signals: void notify(const QString &_message);

      /// \brief
      signals: void cardResized();
      public slots: void OnCardResized() {this->cardResized();}

      // Documentation inherited
//      protected: void paintEvent(QPaintEvent *_event) override;

      // Documentation inherited
//      protected: void closeEvent(QCloseEvent *_event) override;

      /// \internal
      /// \brief Private data pointer
      private: std::unique_ptr<MainWindowPrivate> dataPtr;
    };

    /// \brief Holds configurations related to a MainWindow.
    struct IGNITION_GUI_VISIBLE WindowConfig
    {
      /// \brief Update this config from an XML string. Only fields present on
      /// the XML will be overriden / appended / created.
      /// \param[in] _xml A config XML file in string format
      /// \return True if successful. It may fail for example if the string
      /// can't be parsed into XML.
      bool MergeFromXML(const std::string &_xml);

      /// \brief Return this configuration in XML format as a string.
      /// \return String containing a complete config file.
      std::string XMLString() const;

      /// \brief Get whether a property should be ignored
      /// \return True if it's being ignored
      bool IsIgnoring(const std::string &_prop) const;

      /// \brief Window X position in px
      int posX{-1};

      /// \brief Window Y position in px
      int posY{-1};

      /// \brief Window width in px
      int width{-1};

      /// \brief Window height in px
      int height{-1};

      /// \brief Window state (dock configuration)
      QByteArray state;

      /// \brief Material theme (light / dark)
      std::string materialTheme{""};

      /// \brief Material primary color
      std::string materialPrimary{""};

      /// \brief Material accent color
      std::string materialAccent{""};

      /// \brief
      bool showPanel{true};

      /// \brief
      bool showDefaultPanelOpts{true};

      /// \brief
      bool showPluginMenu{true};

      /// \brief True if plugins found in plugin paths should be listed under
      /// the Plugins menu. True by default.
      bool pluginsFromPaths{true};

      /// \brief List of plugins which should be shown on the list
      std::vector<std::string> showPlugins;

      /// \brief List of window properties which should be ignored on load
      std::set<std::string> ignoredProps;

      /// \brief Concatenation of all plugin configurations.
      std::string plugins{""};
    };
  }
}
#endif
