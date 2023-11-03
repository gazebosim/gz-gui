/*
 * Copyright (C) 2021 Open Source Robotics Foundation
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

#ifndef GZ_GUI_PLUGINS_TRANSPORTSCENEMANAGER_HH_
#define GZ_GUI_PLUGINS_TRANSPORTSCENEMANAGER_HH_

#include <memory>

#include "gz/gui/Plugin.hh"

namespace gz::gui::plugins
{
  class TransportSceneManagerPrivate;

  /// \brief Provides a Gazebo Transport interface to
  /// `gz::gui::plugins::MinimalScene`.
  ///
  /// ## Configuration
  ///
  /// * \<service\> : Name of service where this system will request a scene
  ///                 message. Optional, defaults to "/scene".
  /// * \<pose_topic\> : Name of topic to subscribe to receive pose updates.
  ///                    Optional, defaults to "/pose".
  /// * \<deletion_topic\> : Name of topic to request entity deletions.
  ///                        Optional, defaults to "/delete".
  /// * \<scene_topic\> : Name of topic to receive scene updates. Optional,
  ///                     defaults to "/scene".
  class TransportSceneManager : public Plugin
  {
    Q_OBJECT

    /// \brief Constructor
    public: TransportSceneManager();

    /// \brief Destructor
    public: virtual ~TransportSceneManager();

    // Documentation inherited
    public: virtual void LoadConfig(const tinyxml2::XMLElement *_pluginElem)
        override;

    // Documentation inherited
    private: bool eventFilter(QObject *_obj, QEvent *_event) override;

    /// \internal
    /// \brief Pointer to private data.
    private: std::unique_ptr<TransportSceneManagerPrivate> dataPtr;
  };
}  // namespace gz::gui::plugins

#endif  // GZ_GUI_PLUGINS_TRANSPORTSCENEMANAGER_HH_
