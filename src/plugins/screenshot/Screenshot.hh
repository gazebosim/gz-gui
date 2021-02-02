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

#include <ignition/msgs/boolean.pb.h>
#include <ignition/msgs/stringmsg.pb.h>

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

  /// \brief Provides a button and a transport service for taking a screenshot
  /// of current 3D scene.
  ///
  /// /gui/screenshot service:
  ///     Data: Path to save to, leave empty to save to latest path.
  ///     Response: True if screenshot has been queued succesfully.
  class Screenshot : public Plugin
  {
    Q_OBJECT

    /// \brief Constructor
    public: Screenshot();

    /// \brief Destructor
    public: ~Screenshot() override;

    // Documentation inherited
    public: void LoadConfig(const tinyxml2::XMLElement *_pluginElem) override;

    /// \brief Callback when screenshot is requested from the GUI.
    public slots: void OnScreenshot();

    /// \brief Callback for changing the directory where screenshots are saved
    public slots: void OnChangeDirectory(const QString &_dirUrl);

    /// \brief Callback for all installed event filders.
    /// \param[in] _obj Object that received the event
    /// \param[in] _event Event
    private: bool eventFilter(QObject *_obj, QEvent *_event) override;

    /// \brief Callback for saving a screenshot (from the user camera) request
    /// \param[in] _msg Request message of the saved file path
    /// \param[in] _res Response data
    /// \return True if the request is received
    private: bool ScreenshotService(const msgs::StringMsg &_msg,
        msgs::Boolean &_res);

    /// \brief Encapsulates the logic to find the user camera through the
    /// render engine singleton.
    private: void FindUserCamera();

    /// \brief Save a screenshot from the user camera
    private: void SaveScreenshot();

    /// \internal
    /// \brief Pointer to private data.
    private: std::unique_ptr<ScreenshotPrivate> dataPtr;
  };
}
}
}

#endif
