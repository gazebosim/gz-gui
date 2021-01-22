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
#ifndef IGNITION_GUI_PLUGINS_SCREENSHOT_HH_
#define IGNITION_GUI_PLUGINS_SCREENSHOT_HH_

#include <memory>

#include "ignition/gui/qt.h"
#include "ignition/gui/Plugin.hh"

namespace ignition
{
namespace gui
{
namespace plugins
{
  class ScreenshotPrivate;

  /// \brief Provides a button for taking a screenshot of current 3D scene
  class Screenshot : public Plugin
  {
    Q_OBJECT

    /// \brief Constructor
    public: Screenshot();

    /// \brief Destructor
    public: ~Screenshot() override;

    // Documentation inherited
    public: void LoadConfig(const tinyxml2::XMLElement *_pluginElem) override;

    /// \brief Callback when screenshot is requested
    public slots: void OnScreenshot();

    /// \brief Callback for changing the directory where screenshots are saved
    public slots: void OnChangeDirectory(const QString &_dirUrl);

    /// \internal
    /// \brief Pointer to private data.
    private: std::unique_ptr<ScreenshotPrivate> dataPtr;
  };
}
}
}

#endif
