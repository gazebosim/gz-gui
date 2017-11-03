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

#ifndef IGNITION_GUI_PLUGINS_RENDERSCENE_HH_
#define IGNITION_GUI_PLUGINS_RENDERSCENE_HH_

#include <memory>

#include "ignition/gui/qt.h"
#include "ignition/gui/Plugin.hh"

namespace ignition
{
namespace gui
{
namespace plugins
{
  class Scene3DPrivate;

  /// \brief
  ///
  /// ## Configuration
  class Scene3D : public Plugin
  {
    Q_OBJECT

    /// \brief Constructor
    public: Scene3D();

    /// \brief Destructor
    public: virtual ~Scene3D();

    // Documentation inherited
    public: virtual void LoadConfig(const tinyxml2::XMLElement *_pluginElem)
        override;

    // Documentation inherited
    protected: virtual void paintEvent(QPaintEvent *_e) override;

    // Documentation inherited
    protected: virtual void resizeEvent(QResizeEvent *_e) override;

    /// \internal
    /// \brief Pointer to private data.
    private: std::unique_ptr<Scene3DPrivate> dataPtr;
  };
}
}
}

#endif
