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

#ifndef IGNITION_GUI_PLUGINS_TOPICECHO_HH_
#define IGNITION_GUI_PLUGINS_TOPICECHO_HH_

#ifndef Q_MOC_RUN
  #include <ignition/gui/qt.h>
#endif

#ifdef _MSC_VER
#pragma warning(push, 0)
#endif
#include <google/protobuf/message.h>
#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include <memory>

#include "ignition/gui/Plugin.hh"

namespace ignition
{
namespace gui
{
namespace plugins
{
  class TopicEchoPrivate;

  /// \brief Echo messages coming through an Ignition transport topic.
  class TopicEcho : public Plugin
  {
    Q_OBJECT

    /// \brief Constructor
    public: TopicEcho();

    /// \brief Destructor
    public: virtual ~TopicEcho();

    // Documentation inherited
    public: virtual void LoadConfig(const tinyxml2::XMLElement *_pluginElem);

    /// \brief Signal to add a message to the GUI list.
    /// \param[in] _msg Text message to add.
    signals: void AddMsg(QString _msg);

    /// \brief Receives incoming text messages.
    /// \param[in] _msg New text message.
    private: void OnMessage(const google::protobuf::Message &_msg);

    /// \brief Clear list and unsubscribe.
    private: void Stop();

    /// \brief Callback when echo button is pressed
    private slots: void OnEcho();

    /// \brief QT callback when the buffer spin box has been changed.
    /// \param[in] _value New value of the spin box.
    private slots: void OnBuffer(int _value);

    /// \brief QT callback when the pause check box has been changed.
    /// \param[in] _value New value of the check box.
    private slots: void OnPause(bool _value);

    /// \brief Callback from the ::AddMsg function.
    /// \param[in] _msg Message to add to the list.
    private slots: void OnAddMsg(QString _msg);

    /// \brief ToDo.
    private slots: void UpdateFrequency();


    /// \internal
    /// \brief Pointer to private data.
    private: std::unique_ptr<TopicEchoPrivate> dataPtr;
  };
}
}
}

#endif
