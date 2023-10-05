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
#ifndef GZ_GUI_PLUGIN_HH_
#define GZ_GUI_PLUGIN_HH_

#include <tinyxml2.h>
#include <memory>
#include <string>

#include "gz/gui/qt.h"
#include "gz/gui/Export.hh"

#ifdef _WIN32
// Disable warning C4251 which is triggered by
// std::unique_ptr
#pragma warning(push)
#pragma warning(disable: 4251)
#endif

namespace gz::gui
{
/// \brief Namespace for all plugins
namespace plugins {}

class PluginPrivate;

/// \brief Base class for Gazebo GUI plugins.
///
/// When inheriting from this plugin, the following are assumed:
///
/// * The derived class' name is the same as the generated shared library
///   (i.e. if the Publisher class extends Plugin, the library file is
///   libPublisher.so)
///
/// * There is a QML file with the same name as the plugin's shared library
///   name.
///   (i.e. there must be a Publisher.qml)
///
/// * The QML file is prefixed by the library's name in the QRC file
///   (i.e. the file's resource is found at ':/Publisher/Publisher.qml')
class GZ_GUI_VISIBLE Plugin : public QObject
{
  Q_OBJECT

  /// \brief Constructor
  public: Plugin();

  /// \brief Destructor
  public: virtual ~Plugin();

  /// \brief Load the plugin with a configuration file.
  /// This loads the default parameters and then calls LoadConfig(), which
  /// should be overridden to load custom parameters.
  ///
  /// Called when a plugin is first created.
  /// This function should not be blocking.
  ///
  /// \sa Load
  /// \param[in] _pluginElem Element containing configuration
  public: void Load(const tinyxml2::XMLElement *_pluginElem);

  /// \brief Get the configuration XML as a string
  /// \return Config element
  public: virtual std::string ConfigStr();

  /// \brief Get the card item which contains this plugin. The item is
  /// generated the first time this function is run.
  /// \return Pointer to card item.
  public: QQuickItem *CardItem() const;

  /// \brief Get the plugin item.
  /// \return Pointer to plugin item.
  public: QQuickItem *PluginItem() const;

  /// \brief Get the QML context where the plugin was created.
  /// \return Pointer context.
  public: QQmlContext *Context() const;

  /// \brief Apply changes which should come after the plugin already
  /// has a parent.
  public: void PostParentChanges();

  /// \brief Load the plugin with a configuration file. Override this
  /// on custom plugins to handle custom configurations.
  ///
  /// Called when a plugin is first created.
  /// This function should not be blocking.
  ///
  /// \sa Load
  /// \param[in] _pluginElem Element containing configuration
  protected: virtual void LoadConfig(
      const tinyxml2::XMLElement *_pluginElem)
      {
        (void)_pluginElem;
      }

  /// \brief Get title
  /// \return Plugin title.
  public: virtual std::string Title() const {return this->title;}

  /// \brief Get the value of the the `delete_later` element from the
  /// configuration file, which defaults to false.
  /// \return The value of `delete_later`.
  public: bool DeleteLaterRequested() const;

  /// \brief Wait until the plugin has a parent, then close and delete the
  /// parent.
  protected: void DeleteLater();

  /// \brief Title to be displayed on top of plugin.
  protected: std::string title = "";

  /// \brief XML configuration
  protected: std::string configStr;

  /// \brief Load configuration which is common to all plugins and handled
  /// by Gazebo GUI.
  /// \details Called when a plugin is first created.
  /// \sa LoadConfig
  /// \param[in] _gzGuiElem <gz-gui> element within the <plugin>.
  /// Will be nullptr if not present in the SDF.
  private: virtual void LoadCommonConfig(
      const tinyxml2::XMLElement *_gzGuiElem);

  /// \brief Apply any anchors which may have been specified on the config
  /// through the <anchor> tag and any state properties.
  private: void ApplyAnchors();

  /// \internal
  /// \brief Pointer to private data
  private: std::unique_ptr<PluginPrivate> dataPtr;
};
}  // namespace gz::gui

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif  // GZ_GUI_PLUGIN_HH_
