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

#ifndef IGNITION_GUI_PLUGINS_TOPICINTERFACE_HH_
#define IGNITION_GUI_PLUGINS_TOPICINTERFACE_HH_

#ifdef _MSC_VER
#pragma warning(push, 0)
#endif
#include <google/protobuf/message.h>
#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include <memory>

#include "ignition/gui/qt.h"
#include "ignition/gui/Plugin.hh"

namespace ignition
{
namespace gui
{
namespace plugins
{
  class TopicInterfacePrivate;

  /// \brief Interface messages coming through an Ignition transport topic.
  ///
  /// ## Configuration
  /// This plugin doesn't accept any custom configuration.
  class TopicInterface : public Plugin
  {
    Q_OBJECT

    /// \brief Constructor
    public: TopicInterface();

    /// \brief Destructor
    public: virtual ~TopicInterface();

    // Documentation inherited
    public: virtual void LoadConfig(const tinyxml2::XMLElement *_pluginElem);

    /// \brief Receives incoming text messages.
    /// \param[in] _msg New text message.
    private: void OnMessage(const google::protobuf::Message &_msg);

    /// \internal
    /// \brief Pointer to private data.
    private: std::unique_ptr<TopicInterfacePrivate> dataPtr;
  };
}
}
}

#endif
