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
#ifndef IGNITION_GUI_BACKEND_HH_
#define IGNITION_GUI_BACKEND_HH_

#include <memory>
#include <string>
#include <utility>
#include <vector>

//#include <ignition/common/SingletonT.hh>

#include "ignition/gui/qt.h"
#include "ignition/gui/Export.hh"

// Forward declarations.
namespace tinyxml2
{
  class XMLElement;
}

namespace ignition
{
  namespace gui
  {
    class BackEndPrivate;
    class Dialog;
    class MainWindow;
    class Plugin;

    /// \brief
    /// \remarks Multiple backends can be instantiated per process, but
    /// they will share the same Qt application.
    class IGNITION_GUI_VISIBLE BackEnd : public QObject
       // , public virtual common::SingletonT<BackEnd>
    {
      Q_OBJECT

      /// \brief Constructor. Initializes application, signal handlers and
      /// others.
      public: BackEnd();

      /// \brief Destructor
      public: virtual ~BackEnd();

      /// \brief Load plugins from a configuration file.
      /// \param[in] _config Path to configuration file.
      /// \return True if successful
      public: bool LoadConfig(const std::string &_config);

      /// \brief Load the configuration from the default config file.
      /// \return True if successful
      /// \sa SetDefaultConfigPath
      /// \sa defaultConfigPath
      public: bool LoadDefaultConfig();

      /// \brief Load a plugin from a file name. The plugin file must be in the
      /// path.
      /// \param[in] _filename Plugin filename.
      /// \param[in] _pluginElem Element containing plugin configuration
      /// \return True if successful
      public: bool LoadPlugin(const std::string &_filename,
          const tinyxml2::XMLElement *_pluginElem = nullptr);

      /// \brief Set the global style sheet from a file.
      /// \param[in] _qssFile Path to QSS file. Use `:/style.qss` for Ignition
      /// GUI's default style sheet.
      /// \sa SetStyleFromString
      public: bool SetStyleFromFile(const std::string &_qssFile);

      /// \brief Set the global style sheet from a string.
      /// \param[in] _styleSheet Style sheet in QSS format. Pass an empty string
      /// to use Qt's default style.
      /// \return True if successful.
      /// \sa SetStyleFromFile
      public: bool SetStyleFromString(const std::string &_styleSheet);

      /// \brief Specifies the location of the default configuration file.
      /// This is the file that stores the user settings when pressing
      /// "Save configuration".
      /// \param[in] _path The default configuration full path including filename.
      /// \sa LoadDefaultConfig
      /// \sa defaultConfigPath
      public: void SetDefaultConfigPath(const std::string &_path);

      /// \brief Get the location of the default configuration file.
      /// \return The default configuration path.
      /// \sa LoadDefaultConfig
      /// \sa SetDefaultConfigPath
      public: std::string DefaultConfigPath();

      /// \brief Add previously loaded plugins to the main window.
      /// * Make sure the window is created first
      /// * Be sure to call LoadPlugin() for each plugin first
      /// * Plugins previously added must be loaded again
      /// \return True if successful
      public: bool AddPluginsToWindow();

      /// \brief Remove plugin
      /// \return True if successful
      public: bool RemovePlugin(const std::string &_pluginName);

      /// \brief Apply previously loaded config to the main window.
      /// * Make sure the window is created first
      /// * Be sure to call loadConfig() for each plugin first
      /// \return True if successful
      public: bool ApplyConfig();

      /// \brief Run a main window using the given configuration file. This is
      /// the main entry point for the command line tool "ign gui -c".
      /// \param[in] _config Full path to configuration file.
      /// \return True if successful
      public: bool RunConfig(const std::string &_config);

      /// \brief Run a given plugin as a standalone window. This is the main
      /// entry point for the command line tool "ign gui -s".
      /// \param[in] _filename Plugin file name. The file must be in the path.
      /// \return True if successful
      public: bool RunStandalone(const std::string &_filename);

      /// \brief Run previously loaded plugins as individual dialogs.
      /// This has no effect if no plugins have been loaded.
      /// \return True if successful
      public: bool RunDialogs();

      /// \brief Run an empty window.
      /// \return True if successful
      public: bool RunEmptyWindow();

      /// \brief Run previously loaded plugins on a single main window.
      /// An empty window will be created if no plugins have been loaded.
      /// \return True if successful
      public: bool CreateMainWindow();

      /// \brief Get the main window, if one has been generated. Use this to
      /// customize the window before running it.
      /// \return Pointer to the main window
      public: MainWindow *Window();

      /// \brief Get the dialogs, if they have been generated.
      /// \return Vector of dialogs
      public: std::vector<Dialog *> Dialogs();

      /// \brief Run previously created window.
      /// \return True if successful
      public: bool RunMainWindow();

      /// \brief Set the environment variable which defines the paths to look for
      /// plugins.
      /// \param[in] _env Name of environment variable.
      public: void SetPluginPathEnv(const std::string &_env);

      /// \brief Add an path to look for plugins.
      /// \param[in] _path Full path.
      public: void AddPluginPath(const std::string &_path);

      /// \brief Set the verbosity level (from 0 to 4).
      public: void SetVerbosity(const unsigned int _verbosity);

      /// \brief Print the available plugins, organized by path.
      public: void ListPlugins();

      /// \brief Get the list of available plugins, organized by path. The paths
      /// are given in the following order:
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

      // Check whether the app has been initialized
      private: bool CheckApp();

      // Install a signal handler to allow graceful shutdown on Ctrl-C.
      private: bool InstallSignalHandler();

      /// \brief Get home directory.
      /// \return Home directory or empty string if home wasn't found.
      /// \ToDo: Move this function to ignition::common::Filesystem
      std::string HomePath();
      void RemoveAddedPlugin(std::shared_ptr<Plugin> _plugin);

      // QT message handler that pipes qt messages into our console system.
      private: static void MessageHandler(QtMsgType _type,
          const QMessageLogContext &_context, const QString &_msg);

      /// \internal
      /// \brief Private data pointer
      private: std::unique_ptr<BackEndPrivate> dataPtr;

      /// \brief required SingletonT friendship
//      private: friend class SingletonT<BackEnd>;
    };
  }
}
#endif
