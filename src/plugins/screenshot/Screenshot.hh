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
#ifndef GZ_GUI_PLUGINS_SCREENSHOT_HH_
#define GZ_GUI_PLUGINS_SCREENSHOT_HH_

#include <gz/msgs/boolean.pb.h>
#include <gz/msgs/stringmsg.pb.h>

#include <memory>

#include "gz/gui/qt.h"
#include "gz/gui/Plugin.hh"

namespace gz::gui::plugins
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

/// \brief Directory to save screenshots
Q_PROPERTY(
  QString directory
  READ Directory
  WRITE SetDirectory
  NOTIFY DirectoryChanged
)

/// \brief Saved screenshot filepath
Q_PROPERTY(
  QString savedScreenshotPath
  READ SavedScreenshotPath
  WRITE SetSavedScreenshotPath
  NOTIFY SavedScreenshotPathChanged
)

/// \brief Constructor
public: Screenshot();

/// \brief Destructor
public: ~Screenshot() override;

// Documentation inherited
public: void LoadConfig(const tinyxml2::XMLElement *_pluginElem) override;

/// \brief Callback when screenshot is requested from the GUI.
public slots: void OnScreenshot();

/// \brief Callback for all installed event filders.
/// \param[in] _obj Object that received the event
/// \param[in] _event Event
private: bool eventFilter(QObject *_obj, QEvent *_event) override;

/// \brief Callback for saving a screenshot (from the user camera) request
/// \param[in] _msg Request message of the directory path to save
/// screenshots
/// \param[in] _res Response data
/// \return True if the request is received
private: bool ScreenshotService(const msgs::StringMsg &_msg,
    msgs::Boolean &_res);

/// \brief Encapsulates the logic to find the user camera through the
/// render engine singleton.
private: void FindUserCamera();

/// \brief Save a screenshot from the user camera
private: void SaveScreenshot();

/// \brief Get the directory path as a string, for example '/home/Pictures'
/// \return Directory
public: Q_INVOKABLE QString Directory() const;

/// \brief Set the directory path from a string, for example
/// '/home/Pictures'
/// \param[in] _dirUrl The new directory path
public: Q_INVOKABLE void SetDirectory(const QString &_dirUrl);

/// \brief Notify that the directory path has changed
signals: void DirectoryChanged();

/// \brief Get the filepath of the saved screenshot as a string, for example
/// '/home/Pictures/[timestamp].png'
/// \return Saved screenshot filename
public: Q_INVOKABLE QString SavedScreenshotPath() const;

/// \brief Set the filepath of the saved screenshot from a string,
/// for example '/home/Pictures/[timestamp].png'
/// \param[in] _filename The filename (including path) of the screenshot
public: Q_INVOKABLE void SetSavedScreenshotPath(const QString &_filename);

/// \brief Notify that the screenshot filename has changed
signals: void SavedScreenshotPathChanged();

/// \brief Notify that the screenshot has been saved (opens popup)
signals: void savedScreenshot();

/// \internal
/// \brief Pointer to private data.
private: std::unique_ptr<ScreenshotPrivate> dataPtr;
};
}  // namespace gz::gui::plugins
#endif  // GZ_GUI_PLUGINS_SCREENSHOT_HH_
