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

#include <gz/msgs/int32.pb.h>

#include <string>

#include <gz/gui/Application.hh>
#include <gz/gui/MainWindow.hh>
#include <gz/plugin/Register.hh>

#include "KeyPublisher.hh"

namespace gz::gui::plugins
{
class KeyPublisherPrivate
{
  /// \brief Node for communication
  public: gz::transport::Node node;

  /// \brief Publisher
  public: gz::transport::Node::Publisher pub;

  /// \brief Topic
  public: std::string topic = "keyboard/keypress";

  /// \brief Publish keyboard strokes
  /// \param[in] key_press Pointer to the keyevent
  public: void KeyPub(QKeyEvent *_keyPress)
  {
    gz::msgs::Int32 Msg;
    Msg.set_data(_keyPress->key());
    pub.Publish(Msg);
  }
};

/////////////////////////////////////////////////
KeyPublisher::KeyPublisher(): Plugin(), dataPtr(new KeyPublisherPrivate)
{
  // Advertise publisher node
  this->dataPtr->pub = this->dataPtr->node.Advertise<msgs::Int32>
    (this->dataPtr->topic);
}

/////////////////////////////////////////////////
KeyPublisher::~KeyPublisher()
{
}

/////////////////////////////////////////////////
void KeyPublisher::LoadConfig(const tinyxml2::XMLElement *)
{
  if (this->title.empty())
    this->title = "Key publisher";

  gui::App()->findChild
    <MainWindow *>()->QuickWindow()->installEventFilter(this);
}

/////////////////////////////////////////////////
bool KeyPublisher::eventFilter(QObject *_obj, QEvent *_event)
{
  if (_event->type() == QEvent::KeyPress)
  {
    QKeyEvent *keyEvent = static_cast<QKeyEvent*>(_event);
    this->dataPtr->KeyPub(keyEvent);
  }
  return QObject::eventFilter(_obj, _event);
}
}  // namespace gz::gui::plugins

// Register this plugin
GZ_ADD_PLUGIN(gz::gui::plugins::KeyPublisher,
              gz::gui::Plugin)
