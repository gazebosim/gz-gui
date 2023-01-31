/*
 * Copyright (C) 2023 Open Source Robotics Foundation
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
#ifndef GZ_GUI_PLUGINS_CAMERACONFIG_HH_
#define GZ_GUI_PLUGINS_CAMERACONFIG_HH_

#include <memory>

#include "gz/gui/Plugin.hh"

namespace gz
{
namespace gui
{
namespace plugins
{
  class CameraConfigPrivate;

  class CameraConfig : public Plugin
  {
    Q_OBJECT

    Q_PROPERTY(
      QString followTargetName
      READ FollowTargetName
      WRITE SetFollowTargetName
      NOTIFY FollowTargetNameChanged
    )

    /// \brief Constructor
    public: CameraConfig();

    /// \brief Destructor
    public: virtual ~CameraConfig();

    // Documentation inherited
    public: virtual void LoadConfig(const tinyxml2::XMLElement *_pluginElem)
        override;

    /// \brief Set the new follow target, requested from the GUI.
    /// \param[in] _targetName The target name to follow
    public: Q_INVOKABLE void SetFollowTargetName(const QString &_targetName);

    /// \brief Notify that the follow target name has changed
    signals: void FollowTargetNameChanged();

    /// \brief Get the follow target's name as a string
    /// \return Follow target's name
    public: Q_INVOKABLE QString FollowTargetName() const;

    // Documentation inherited
    private: bool eventFilter(QObject *_obj, QEvent *_event) override;

    /// \internal
    /// \brief Pointer to private data.
    private: std::unique_ptr<CameraConfigPrivate> dataPtr;
  };
}
}
}
#endif
