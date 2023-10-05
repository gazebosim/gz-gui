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
#ifndef GZ_GUI_APPLICATION_HH_
#define GZ_GUI_APPLICATION_HH_

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "gz/gui/qt.h"
#include "gz/gui/Export.hh"

#ifdef _WIN32
// Disable warning C4251 which is triggered by
// std::unique_ptr
#pragma warning(push)
#pragma warning(disable: 4251)
#endif

namespace tinyxml2
{
  class XMLElement;
}

namespace gz::gui
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

  /// \brief One independent dialog per plugin. Also useful to open a
  /// startup dialog before the main window.
  kDialog = 1
};

/// \brief A Gazebo GUI application loads a QML engine and
/// provides an API to load plugins and configuration files. The application
/// supports either running a single main window or several plugins as
/// standalone dialogs.
class GZ_GUI_VISIBLE Application : public QApplication
{
  Q_OBJECT

  /// \brief Constructor.
  /// \param[in] _argc Argument count.
  /// \param[in] _argv Argument values.
  /// \param[in] _type Window type, by default it's a main window.
  /// \param[in] _renderEngineGuiApiBackend --render-engine-gui-api-backend
  /// option
  public: Application(int &_argc, char **_argv,
      const WindowType _type = WindowType::kMainWindow,
      const char *_renderEngineGuiApiBackend = nullptr);

  /// \brief Destructor
  public: virtual ~Application();

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
  /// \param[in] _path Full path to configuration file.
  /// \return True if successful
  /// \sa InitializeMainWindow
  /// \sa InitializeDialogs
  public: bool LoadConfig(const std::string &_path);

  /// \brief Load the configuration from the default config file.
  /// \return True if successful
  /// \sa SetDefaultConfigPath
  /// \sa DefaultConfigPath
  /// \sa LoadConfig
  public: bool LoadDefaultConfig();

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
  /// 3. Path ~/.gz/gui/plugins
  /// 4. The path where Gazebo GUI plugins are installed
  ///
  /// \return A vector of pairs, where each pair contains:
  /// * A path
  /// * A vector of plugins in that path
  public: std::vector<std::pair<std::string, std::vector<std::string>>>
      PluginList();

  /// \brief Remove plugin by name. The plugin is removed from the
  /// application and its shared library unloaded if this was its last
  /// instance.
  /// \param[in] _pluginName Plugn instance's unique name. This is the
  /// plugin card's object name.
  /// \return True if successful
  public: bool RemovePlugin(const std::string &_pluginName);

  /// \brief Get a plugin by its unique name.
  /// \param[in] _pluginName Plugn instance's unique name. This is the
  /// plugin card's object name.
  /// \return Pointer to plugin object, null if not found.
  public: std::shared_ptr<Plugin> PluginByName(
      const std::string &_pluginName) const;

  /// \brief Notify that a plugin has been added.
  /// \param[in] _objectName Plugin's object name.
  signals: void PluginAdded(const QString &_objectName);

  /// \brief Callback when user requests to close a plugin
  public slots: void OnPluginClose();

  /// \brief Create a main window. Just calls InitializeMainWindow.
  /// \return True if successful
  /// \sa InitializeMainWindow
  public: bool CreateMainWindow();

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

  /// \brief Apply previously loaded config to the main window.
  /// \return True if successful, will fail if there's no main window
  /// initialized.
  private: bool ApplyConfig();

  /// \internal
  /// \brief Private data pointer
  private: std::unique_ptr<ApplicationPrivate> dataPtr;
};

/// \brief Get current running application, this is a cast of qGuiApp.
/// \return Pointer to running application, or nullptr if none is running.
GZ_GUI_VISIBLE
Application *App();
}  // namespace gz::gui

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif  // GZ_GUI_APPLICATION_HH_
