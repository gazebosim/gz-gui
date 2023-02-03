/*
 * Copyright (C) 2023 Rudis Laboratories LLC
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
#ifndef GZ_GUI_PLUGINS_FOLLOWCONFIG_HH_
#define GZ_GUI_PLUGINS_FOLLOWCONFIG_HH_

#include <memory>

#include "gz/gui/Plugin.hh"

namespace gz
{
namespace gui
{
namespace plugins
{
  class FollowConfigPrivate;

  class FollowConfig : public Plugin
  {
    Q_OBJECT

    /// \brief Constructor
    public: FollowConfig();

    /// \brief Destructor
    public: virtual ~FollowConfig();

    // Documentation inherited
    public: virtual void LoadConfig(const tinyxml2::XMLElement *_pluginElem)
        override;

    /// \brief Set the follow offset, requested from the GUI.
    /// \param[in] _x The follow offset distance in x
    /// \param[in] _y The follow offset distance in y
    /// \param[in] _z The follow offset distance in z
    public slots: void SetFollowOffset(double _x,
          double _y, double _z);

    /// \brief Set the follow pgain, requested from the GUI.
    /// \param[in] _p The follow offset distance in x
    public slots: void SetFollowPGain(double _p);


    // Documentation inherited
    private: bool eventFilter(QObject *_obj, QEvent *_event) override;

    /// \internal
    /// \brief Pointer to private data.
    private: std::unique_ptr<FollowConfigPrivate> dataPtr;
  };
}
}
}
#endif
