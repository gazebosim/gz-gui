/*
 * Copyright (C) 2018 Open Source Robotics Foundation
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
#ifndef IGNITION_GUI_APPLICATION_HH_
#define IGNITION_GUI_APPLICATION_HH_

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "ignition/gui/qt.h"
#include "ignition/gui/Export.hh"

namespace tinyxml2
{
  class XMLElement;
}

namespace ignition
{
  namespace gui
  {
    class ApplicationPrivate;
    class Dialog;
    class MainWindow;
    class Plugin;

    /// \brief Type of window which the application will display
    enum class WindowType : int
    {
      /// \brief A main window, which may contain top-level menus and multiple
      /// plugins
      kMainWindow = 0,

      /// \brief One independent dialog per plugin
      kDialog = 1
    };

    /// \brief Holds all the configuration for a plugin
    struct PluginConfig
    {
      /// \brief The plugin's filename
      std::string filename = "";

      /// \brief The plugin's XML element
      tinyxml2::XMLElement *elem = nullptr;
    };

    /// \brief An Ignition GUI application loads a QML engine and
    /// provides an API to load plugins and configuration files. The application
    /// supports either running a single main window or several plugins as
    /// standalone dialogs.
    class IGNITION_GUI_VISIBLE Application : public QGuiApplication
    {
      Q_OBJECT

      // Documentation inherited
      public: Application(int &_argc, char **_argv);

      /// \brief Destructor
      public: virtual ~Application();

      /// \brief Initialize the application window(s) with the default
      /// configuration.
      /// \param[in] _type Window type, such as main window or dialog
      public: bool Initialize(const WindowType _type);

      /// \brief Initialize the application window(s) with a given
      /// configuration.
      /// \param[in] _type Window type, such as main window or dialog.
      /// \param[in] _config Path to config file.
      public: bool Initialize(const WindowType _type,
                              const std::string &_config);

      /// \brief Initialize the application window(s) with a given
      /// configuration and plugins.
      /// \param[in] _type Window type, such as main window or dialog.
      /// \param[in] _config Path to config file.
      /// \param[in] _plugins Configuration for plugins
      public: bool Initialize(const WindowType _type,
                              const std::string &_config,
                              const std::vector<PluginConfig> &_plugins);

      /// \brief Get the QML engine
      /// \return Pointer to QML engine
      public: QQmlApplicationEngine *Engine() const;

      /// \brief Load a plugin from a file name. The plugin file must be in the
      /// path.
      /// If a window has been initialized, the plugin is added to the window.
      /// Otherwise, the plugin is stored and can be later added to a window or
      /// dialog.
      /// \param[in] _filename Plugin filename.
      /// \param[in] _pluginElem Element containing plugin configuration
      /// \return True if successful
      /// \sa LoadConfig
      /// \sa AddPluginsToWindow
      public: bool LoadPlugin(const std::string &_filename,
          const tinyxml2::XMLElement *_pluginElem = nullptr);

      /// \brief Load a configuration file, which includes window configurations
      /// and plugins. This function doesn't instantiate the plugins, it just
      /// keeps them in memory and they can be applied later by either
      /// instantiating a window or several dialogs.
      /// \param[in] _config Full path to configuration file.
      /// \return True if successful
      /// \sa InitializeMainWindow
      /// \sa InitializeDialogs
      public: bool LoadConfig(const std::string &_config);

      /// \brief Apply previously loaded config to the main window.
      /// * Make sure the window is created first
      /// * Be sure to call loadConfig() for each plugin first
      /// \return True if successful
      public: bool ApplyConfig();

      /// \brief Specifies the location of the default configuration file.
      /// This is the file that stores the user settings when pressing
      /// "Save configuration".
      /// \param[in] _path The default configuration full path including
      /// filename.
      /// \sa LoadDefaultConfig
      /// \sa defaultConfigPath
      public: void SetDefaultConfigPath(const std::string &_path);

      /// \brief Get the location of the default configuration file.
      /// \return The default configuration path.
      /// \sa LoadDefaultConfig
      /// \sa SetDefaultConfigPath
      public: std::string DefaultConfigPath();

      /// \brief Set the environment variable which defines the paths to
      /// look for plugins.
      /// \param[in] _env Name of environment variable.
      public: void SetPluginPathEnv(const std::string &_env);

      /// \brief Add an path to look for plugins.
      /// \param[in] _path Full path.
      public: void AddPluginPath(const std::string &_path);

      /// \brief Get the list of available plugins, organized by path. The
      /// paths are given in the following order:
      ///
      /// 1. Paths given by the environment variable
      /// 2. Paths added by calling addPluginPath
      /// 3. Path ~/.ignition/gui/plugins
      /// 4. The path where Ignition GUI plugins are installed
      ///
      /// \return A vector of pairs, where each pair contains:
      /// * A path
      /// * A vector of plugins in that path
      public: std::vector<std::pair<std::string, std::vector<std::string>>>
          PluginList();

      /// \brief Remove plugin by name. The plugin is removed from the
      /// application and its shared library unloaded if this was its last
      /// instance.
      /// \param[in] _pluginName Plugn instance's unique name
      /// \return True if successful
      public: bool RemovePlugin(const std::string &_pluginName);

      /// \brief Load the configuration from the default config file.
      /// \return True if successful
      /// \sa SetDefaultConfigPath
      /// \sa DefaultConfigPath
      /// \sa LoadConfig
      private: bool LoadDefaultConfig();

      /// \brief Create a main window, populate with previously loaded plugins
      /// and apply previously loaded configuration.
      /// An empty window will be created if no plugins have been loaded.
      /// \return True if successful
      /// \sa LoadConfig
      /// \sa LoadPlugin
      private: bool InitializeMainWindow();

      /// \brief Create individual dialogs for all previously loaded plugins.
      /// This has no effect if no plugins have been loaded.
      /// \return True if successful
      /// \sa LoadConfig
      /// \sa LoadPlugin
      private: bool InitializeDialogs();


      /// \brief Remove plugin by pointer.
      /// \param[in] _plugin Shared pointer to plugin
      private: void RemovePlugin(std::shared_ptr<Plugin> _plugin);

      /// \brief Add previously loaded plugins to the main window.
      /// \return True if successful. Will fail if the window hasn't been
      /// created yet.
      /// \sa LoadPlugin
      private: bool AddPluginsToWindow();

      /// \internal
      /// \brief Private data pointer
      private: std::unique_ptr<ApplicationPrivate> dataPtr;
    };

    /// \brief Get current running application, this is a cast of qGuiApp.
    /// \return Pointer to running application, or nullptr if none is running.
    IGNITION_GUI_VISIBLE
    Application *App();
  }
}
#endif
