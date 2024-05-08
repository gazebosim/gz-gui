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
#ifndef GZ_GUI_PLUGINS_TRACKCONFIG_HH_
#define GZ_GUI_PLUGINS_TRACKCONFIG_HH_

#include <memory>

#include "gz/gui/Plugin.hh"

namespace gz
{
namespace gui
{
namespace plugins
{
  class TrackConfigPrivate;

  class TrackConfig : public Plugin
  {
    Q_OBJECT

    /// \brief Constructor
    public: TrackConfig();

    /// \brief Destructor
    public: virtual ~TrackConfig();

    // Documentation inherited
    public: virtual void LoadConfig(const tinyxml2::XMLElement *)
        override;

    /// \brief Set the track camera pose location, requested from the GUI.
    /// \param[in] _x The track camera pose location in x
    /// \param[in] _y The track camera pose location in y
    /// \param[in] _z The track camera pose location in z
    /// \param[in] _p The track camera P gain
    public slots: void SetTrack(double _x,
          double _y, double _z, double _p);

    // Documentation inherited
    private: bool eventFilter(QObject *_obj, QEvent *_event) override;

    /// \internal
    /// \brief Pointer to private data.
    private: std::unique_ptr<TrackConfigPrivate> dataPtr;
  };
}
}
}
#endif