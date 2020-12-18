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

#ifndef IGNITION_GUI_PLUGINS_KEYPUBLISHER_HH_
#define IGNITION_GUI_PLUGINS_KEYPUBLISHER_HH_

#include <ignition/gui/qt.h>

#include <memory>

#include <ignition/gui/Plugin.hh>
#include <ignition/transport/Node.hh>

namespace ignition
{
namespace gui
{
  class KeyPublisherPrivate;

  /// \brief Publish keyboard stokes to "keyboard/keypress" topic.
  ///
  /// ## Configuration
  /// This plugin doesn't accept any custom configuration.
  class KeyPublisher : public ignition::gui::Plugin
  {
    Q_OBJECT

    /// \brief Constructor
    public: KeyPublisher();

    /// \brief Destructor
    public: virtual ~KeyPublisher();

    // Documentation inherited
    public: virtual void LoadConfig(const tinyxml2::XMLElement *) override;

    /// \brief Filter events in Qt
    /// \param[in] _obj The watched object
    /// \param[in] _event Event that happen in Qt
    protected: bool eventFilter(QObject *_obj, QEvent *_event) override;

    /// \internal
    /// \brief Pointer to private data.
    private: std::unique_ptr<KeyPublisherPrivate> dataPtr;
  };
}
}

#endif
