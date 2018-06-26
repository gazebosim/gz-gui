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
    enum class InitializeType : int
    {
      kMainWindow = 0,
      kDialog = 1
    };

    struct PluginConfig
    {
      std::string filename = "";
      tinyxml2::XMLElement *elem = nullptr;
    };

    /// \brief An Ignition GUI application loads a QML engine and
    /// provides an API to load plugins and configuration files. The application
    /// supports either running a single main window or several plugins as
    /// standalone dialogs.
    ///
    /// ## Usage
    ///
    /// ### Exec API
    ///
    /// All functions prefixed with `Exec` are self-contained and will run the
    /// application, blocking until it is terminated (i.e. they call exec()).
    ///
    /// For example: `ExecConfig(configFilePath)` will load a configuration file
    /// and run the application.
    ///
    /// ### Load-Initialize API
    ///
    /// To have more control over the application initialization, use the API
    /// prefixed by `Load` and `Initialize`. Load functions such as `LoadPlugin`
    /// and `LoadConfig` will store plugins and configurations in memory, and
    /// then those can be applied to either a main window
    /// (`InitializeMainWindow`) or dialogs (`InitializeDialogs`).
    ///
    class IGNITION_GUI_VISIBLE Application : public QGuiApplication
    {
      Q_OBJECT

      // Documentation inherited
      public: Application(int &_argc, char **_argv);

      /// \brief Destructor
      public: virtual ~Application();

      public: bool Initialize(const InitializeType _type);
      public: bool Initialize(const InitializeType _type,
                              const std::string &_config);
      public: bool Initialize(const InitializeType _type,
                              const std::string &_config,
                              const std::vector<PluginConfig> &plugins);

      /// \brief Get the QML engine
      /// \return Pointer to QML engine
      public: QQmlApplicationEngine *Engine() const;

      /// \brief Load a plugin from a file name. The plugin file must be in the
      /// path.
      /// \param[in] _filename Plugin filename.
      /// \param[in] _pluginElem Element containing plugin configuration
      /// \return True if successful
      /// \sa LoadConfig
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

      /// \brief Add previously loaded plugins to the main window.
      /// * Make sure the window is created first
      /// * Be sure to call LoadPlugin() for each plugin first
      /// * Plugins previously added must be loaded again
      /// \return True if successful
      public: bool AddPluginsToWindow();

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
