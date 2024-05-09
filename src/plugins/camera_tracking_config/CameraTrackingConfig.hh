/*
 * Copyright (C) 2024 Rudis Laboratories LLC
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
#ifndef GZ_GUI_PLUGINS_CAMERATRACKINGCONFIG_HH_
#define GZ_GUI_PLUGINS_CAMERATRACKINGCONFIG_HH_

#include <memory>

#include "gz/gui/Plugin.hh"

namespace gz
{
namespace gui
{
namespace plugins
{
  class CameraTrackingConfigPrivate;

  class CameraTrackingConfig : public Plugin
  {
    Q_OBJECT

    /// \brief Constructor
    public: CameraTrackingConfig();

    /// \brief Destructor
    public: virtual ~CameraTrackingConfig();

    // Documentation inherited
    public: virtual void LoadConfig(const tinyxml2::XMLElement *)
        override;

    /// \brief Set the tracking camera, requested from the GUI.
    /// \param[in] _tx The track offset in x
    /// \param[in] _ty The track offset in y
    /// \param[in] _tz The track offset in z
    /// \param[in] _tp The track camera P gain
    /// \param[in] _fx The follow offset in x
    /// \param[in] _fy The follow offset in y
    /// \param[in] _fz The follow offset in z
    /// \param[in] _fp The follow camera P gain
    public slots: void SetTracking(
          double _tx, double _ty, double _tz, double _tp,
          double _fx, double _fy, double _fz, double _fp);

    // Documentation inherited
    private: bool eventFilter(QObject *_obj, QEvent *_event) override;

    /// \internal
    /// \brief Pointer to private data.
    private: std::unique_ptr<CameraTrackingConfigPrivate> dataPtr;
  };
}
}
}
#endif
