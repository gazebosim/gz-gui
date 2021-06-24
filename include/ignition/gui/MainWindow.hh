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

#ifdef _WIN32
// Disable warning C4251 which is triggered by
// std::unique_ptr
#pragma warning(push)
#pragma warning(disable: 4251)
#endif

namespace ignition
{
  namespace gui
  {
    class MainWindowPrivate;
    struct WindowConfig;

    /// \brief The main window class creates a QQuickWindow and acts as an
    /// interface which provides properties and functions which can be called
    /// from Main.qml
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

      /// \brief Top toolbar color for light theme (Pre-defined color name or
      /// hex value). Defaults to material primary.
      Q_PROPERTY(
        QString toolBarColorLight
        READ ToolBarColorLight
        WRITE SetToolBarColorLight
        NOTIFY ToolBarColorLightChanged
      )

      /// \brief Top toolbar text color for light theme (Pre-defined color name
      /// or hex value). Defaults to material background.
      Q_PROPERTY(
        QString toolBarTextColorLight
        READ ToolBarTextColorLight
        WRITE SetToolBarTextColorLight
        NOTIFY ToolBarTextColorLightChanged
      )

      /// \brief Top toolbar color for dark theme (Pre-defined color name or
      /// hex value). Defaults to material primary.
      Q_PROPERTY(
        QString toolBarColorDark
        READ ToolBarColorDark
        WRITE SetToolBarColorDark
        NOTIFY ToolBarColorDarkChanged
      )

      /// \brief Top toolbar text color for dark theme (Pre-defined color name
      /// or hex value). Defaults to material background.
      Q_PROPERTY(
        QString toolBarTextColorDark
        READ ToolBarTextColorDark
        WRITE SetToolBarTextColorDark
        NOTIFY ToolBarTextColorDarkChanged
      )

      /// \brief Plugin toolbar color for light theme (Pre-defined color name or
      /// hex value). Defaults to material accent.
      Q_PROPERTY(
        QString pluginToolBarColorLight
        READ PluginToolBarColorLight
        WRITE SetPluginToolBarColorLight
        NOTIFY PluginToolBarColorLightChanged
      )

      /// \brief Plugin toolbar text color for light theme (Pre-defined color
      /// name or hex value). Defaults to material foreground.
      Q_PROPERTY(
        QString pluginToolBarTextColorLight
        READ PluginToolBarTextColorLight
        WRITE SetPluginToolBarTextColorLight
        NOTIFY PluginToolBarTextColorLightChanged
      )

      /// \brief Plugin toolbar color for dark theme (Pre-defined color name or
      /// hex value). Defaults to material accent.
      Q_PROPERTY(
        QString pluginToolBarColorDark
        READ PluginToolBarColorDark
        WRITE SetPluginToolBarColorDark
        NOTIFY PluginToolBarColorDarkChanged
      )

      /// \brief Plugin toolbar text color for dark theme (Pre-defined color
      /// name or hex value). Defaults to material foreground.
      Q_PROPERTY(
        QString pluginToolBarTextColorDark
        READ PluginToolBarTextColorDark
        WRITE SetPluginToolBarTextColorDark
        NOTIFY PluginToolBarTextColorDarkChanged
      )

      /// \brief Flag to show side drawer
      Q_PROPERTY(
        bool showDrawer
        READ ShowDrawer
        WRITE SetShowDrawer
        NOTIFY ShowDrawerChanged
      )

      /// \brief Flag to show side drawer's default options
      Q_PROPERTY(
        bool showDefaultDrawerOpts
        READ ShowDefaultDrawerOpts
        WRITE SetShowDefaultDrawerOpts
        NOTIFY ShowDefaultDrawerOptsChanged
      )

      /// \brief Flag to show plugins menu
      Q_PROPERTY(
        bool showPluginMenu
        READ ShowPluginMenu
        WRITE SetShowPluginMenu
        NOTIFY ShowPluginMenuChanged
      )

      /// \brief Flag to enable confirmation dialog on exit
      Q_PROPERTY(
        bool showDialogOnExit
        READ ShowDialogOnExit
        WRITE SetShowDialogOnExit
        NOTIFY ShowDialogOnExitChanged
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

      /// \brief Returns the top toolbar color for light theme.
      /// \return Toolbar color
      public: Q_INVOKABLE QString ToolBarColorLight() const;

      /// \brief Sets the top toolbar color for light theme.
      /// \param[in] _toolBarColorLight Toolbar color
      public: Q_INVOKABLE void SetToolBarColorLight(
          const QString &_toolBarColorLight);

      /// \brief Returns the top toolbar text color for light theme.
      /// \return Toolbar text color
      public: Q_INVOKABLE QString ToolBarTextColorLight() const;

      /// \brief Sets the top toolbar text color for light theme.
      /// \param[in] _toolBarTextColorLight Toolbar text color
      public: Q_INVOKABLE void SetToolBarTextColorLight(
          const QString &_toolBarTextColorLight);

      /// \brief Returns the top toolbar color for dark theme.
      /// \return Toolbar color
      public: Q_INVOKABLE QString ToolBarColorDark() const;

      /// \brief Sets the top toolbar color for dark theme.
      /// \param[in] _toolBarColorDark Toolbar color
      public: Q_INVOKABLE void SetToolBarColorDark(
          const QString &_toolBarColorDark);

      /// \brief Returns the top toolbar text color for dark theme.
      /// \return Toolbar text color
      public: Q_INVOKABLE QString ToolBarTextColorDark() const;

      /// \brief Sets the top toolbar text color for dark theme.
      /// \param[in] _toolBarTextColorDark Toolbar text color
      public: Q_INVOKABLE void SetToolBarTextColorDark(
          const QString &_toolBarTextColorDark);

      /// \brief Returns the plugin toolbar color for light theme.
      /// \return Toolbar color
      public: Q_INVOKABLE QString PluginToolBarColorLight() const;

      /// \brief Sets the plugin toolbar color for light theme.
      /// \param[in] _pluginPluginToolBarColorLight Toolbar color
      public: Q_INVOKABLE void SetPluginToolBarColorLight(
          const QString &_pluginPluginToolBarColorLight);

      /// \brief Returns the plugin toolbar text color for light theme.
      /// \return Toolbar text color
      public: Q_INVOKABLE QString PluginToolBarTextColorLight() const;

      /// \brief Sets the plugin toolbar text color for light theme.
      /// \param[in] _pluginPluginToolBarTextColorLight Toolbar text color
      public: Q_INVOKABLE void SetPluginToolBarTextColorLight(
          const QString &_pluginPluginToolBarTextColorLight);

      /// \brief Returns the plugin toolbar color for dark theme.
      /// \return Toolbar color
      public: Q_INVOKABLE QString PluginToolBarColorDark() const;

      /// \brief Sets the plugin toolbar color for dark theme.
      /// \param[in] _pluginPluginToolBarColorDark Toolbar color
      public: Q_INVOKABLE void SetPluginToolBarColorDark(
          const QString &_pluginPluginToolBarColorDark);

      /// \brief Returns the plugin toolbar text color for dark theme.
      /// \return Toolbar text color
      public: Q_INVOKABLE QString PluginToolBarTextColorDark() const;

      /// \brief Sets the plugin toolbar text color for dark theme.
      /// \param[in] _pluginPluginToolBarTextColorDark Toolbar text color
      public: Q_INVOKABLE void SetPluginToolBarTextColorDark(
          const QString &_pluginPluginToolBarTextColorDark);

      /// \brief Get the flag to show the side drawer.
      /// \return True to show.
      public: Q_INVOKABLE bool ShowDrawer() const;

      /// \brief Set the flag to show the side drawer.
      /// \param[in] _showDrawer True to show.
      public: Q_INVOKABLE void SetShowDrawer(const bool _showDrawer);

      /// \brief Get the flag to show the side drawer's default options.
      /// \return True to show.
      public: Q_INVOKABLE bool ShowDefaultDrawerOpts() const;

      /// \brief Set the flag to show the side drawer's default options.
      /// \param[in] _showDefaultDrawerOpts True to show.
      public: Q_INVOKABLE void SetShowDefaultDrawerOpts(
          const bool _showDefaultDrawerOpts);

      /// \brief Get the flag to show the plugin menu.
      /// \return True to show.
      public: Q_INVOKABLE bool ShowPluginMenu() const;

      /// \brief Set the flag to show the plugin menu.
      /// \param[in] _showPluginMenu True to show.
      public: Q_INVOKABLE void SetShowPluginMenu(const bool _showPluginMenu);

      /// \brief Get the flag to show the plugin menu.
      /// \return True to show.
      public: Q_INVOKABLE bool ShowDialogOnExit() const;

      /// \brief Set the flag to show the confirmation dialog when exiting.
      /// \param[in] _showDialogOnExit True to show.
      public: Q_INVOKABLE void SetShowDialogOnExit(bool _showDialogOnExit);

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

      /// \brief Notifies when the toolbar color light has changed.
      signals: void ToolBarColorLightChanged();

      /// \brief Notifies when the toolbar text color light has changed.
      signals: void ToolBarTextColorLightChanged();

      /// \brief Notifies when the toolbar color dark has changed.
      signals: void ToolBarColorDarkChanged();

      /// \brief Notifies when the toolbar text color dark has changed.
      signals: void ToolBarTextColorDarkChanged();

      /// \brief Notifies when the toolbar color light has changed.
      signals: void PluginToolBarColorLightChanged();

      /// \brief Notifies when the toolbar text color light has changed.
      signals: void PluginToolBarTextColorLightChanged();

      /// \brief Notifies when the toolbar color dark has changed.
      signals: void PluginToolBarColorDarkChanged();

      /// \brief Notifies when the toolbar text color dark has changed.
      signals: void PluginToolBarTextColorDarkChanged();

      /// \brief Notifies when the show drawer flag has changed.
      signals: void ShowDrawerChanged();

      /// \brief Notifies when the show drawer default options flag has changed.
      signals: void ShowDefaultDrawerOptsChanged();

      /// \brief Notifies when the show menu flag has changed.
      signals: void ShowPluginMenuChanged();

      /// \brief Notifies when the showDialogOnExit flag has changed.
      signals: void ShowDialogOnExitChanged();

      /// \brief Notifies when the window config has changed.
      signals: void configChanged();

      /// \brief Displays a message to the user
      signals: void notify(const QString &_message);

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

      /// \brief Top toolbar color light
      std::string toolBarColorLight{""};

      /// \brief Top toolbar text color light
      std::string toolBarTextColorLight{""};

      /// \brief Top toolbar color dark
      std::string toolBarColorDark{""};

      /// \brief Top toolbar text color dark
      std::string toolBarTextColorDark{""};

      /// \brief Plugin toolbar color light
      std::string pluginToolBarColorLight{""};

      /// \brief Plugin toolbar text color light
      std::string pluginToolBarTextColorLight{""};

      /// \brief Plugin toolbar color dark
      std::string pluginToolBarColorDark{""};

      /// \brief Plugin toolbar text color dark
      std::string pluginToolBarTextColorDark{""};

      /// \brief Show the side drawer
      bool showDrawer{true};

      /// \brief Show the default options of the drawer
      bool showDefaultDrawerOpts{true};

      /// \brief Show the plugins menu
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

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif
