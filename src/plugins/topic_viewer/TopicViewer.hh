/*
 * Copyright (C) 2020 Open Source Robotics Foundation
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

#ifndef GZ_GUI_PLUGINS_TOPICVIEWER_HH_
#define GZ_GUI_PLUGINS_TOPICVIEWER_HH_

#include <memory>

#include <gz/gui/Plugin.hh>

#ifndef _WIN32
#  define TopicViewer_EXPORTS_API
#else
#  if (defined(TopicViewer_EXPORTS))
#    define TopicViewer_EXPORTS_API __declspec(dllexport)
#  else
#    define TopicViewer_EXPORTS_API __declspec(dllimport)
#  endif
#endif

namespace gz::gui::plugins
{
class TopicsModel;
class TopicViewerPrivate;

/// \brief a Plugin to view the topics and their msgs & fields
/// Field's informations can be passed by dragging them via the UI
class TopicViewer_EXPORTS_API TopicViewer : public Plugin
{
  Q_OBJECT

  /// \brief Constructor
  public: TopicViewer();

  /// \brief Destructor
  public: ~TopicViewer();

  /// \brief Documentaation inherited
  public: void LoadConfig(const tinyxml2::XMLElement *) override;

  /// \brief Get the model of msgs & fields
  /// \return Pointer to the model of msgs & fields
  public: QStandardItemModel *Model();

  /// \brief update the model according to the changes of the topics
  public slots: void UpdateModel();

  /// \brief Pointer to private data.
  private: std:: unique_ptr<TopicViewerPrivate> dataPtr;
};
}  // namespace gz::gui::plugins
#endif  // GZ_GUI_PLUGINS_TOPICVIEWER_HH_
