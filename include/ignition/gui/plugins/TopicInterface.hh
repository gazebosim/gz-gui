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

  /// \brief This plugin provides a graphical interface for messages coming
  /// through an Ignition Transport topic.
  ///
  /// ## Configuration
  /// <topic>: Topic to subscribe to. This is defined the moment the plugin is
  ///          created and can't be changed afterwards. If no topic is
  ///          specified, the plugin subscribes to `/echo`.
  /// <message_type>: Fully qualified name of the message type expected on the
  ///                 topic above. Widgets will be generated according to this.
  ///                 If not specified, widget will be constructed according to
  ///                 the first message received on <topic>. Once widgets have
  ///                 been generated, the message type can't be changed.
  class TopicInterface : public Plugin
  {
    Q_OBJECT

    /// \brief Constructor
    public: TopicInterface();

    /// \brief Destructor
    public: virtual ~TopicInterface();

    // Documentation inherited
    public: virtual void LoadConfig(const tinyxml2::XMLElement *_pluginElem);

    /// \brief Callback when a message is received.
    /// \param[in] _msg New message.
    private: void OnMessage(const google::protobuf::Message &_msg);

    /// \brief Create message widget and add it to layout.
    /// \param[in] _msg Message.
    private: void CreateWidget(const google::protobuf::Message &_msg);

    /// \internal
    /// \brief Pointer to private data.
    private: std::unique_ptr<TopicInterfacePrivate> dataPtr;
  };
}
}
}

#endif
