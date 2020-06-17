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

#include <iostream>

#include <ignition/gui/Application.hh>
#include <ignition/gui/MainWindow.hh>
#include <ignition/plugin/Register.hh>
#include <ignition/msgs/int32.pb.h>
#include "ignition/gui/plugins/KeyPublisher.hh"

namespace ignition
{
namespace gui
{
  class KeyPublisherPrivate
  {
    public: ignition::transport::Node node;
    public: ignition::transport::Node::Publisher pub;
    public: std::string topic = "keyboard/keypress";
    public: void KeyPub(QKeyEvent *key_press)
    {
      ignition::msgs::Int32 Msg;
      Msg.set_data(key_press->key());
      pub.Publish(Msg);
    }
  };
}
}

using namespace ignition;
using namespace gui;

KeyPublisher::KeyPublisher(): Plugin(), dataPtr(new KeyPublisherPrivate)
{
  this->dataPtr->pub = this->dataPtr->node.Advertise<ignition::msgs::Int32>(this->dataPtr->topic);
}


KeyPublisher::~KeyPublisher()
{
}


void KeyPublisher::LoadConfig(const tinyxml2::XMLElement *)
{
  //it loads the XML file which contains the UI file of Qt
  if (this->title.empty())
  {
    this->title = "Key tool";
  }
  ignition::gui::App()->findChild
    <ignition::gui::MainWindow *>()->QuickWindow()->installEventFilter(this);
}



bool KeyPublisher::eventFilter(QObject *_obj, QEvent *_event)
{
  //dataPtr(new KeyPublisherPrivate)
  if (_event->type() == QEvent::KeyPress)
  {
    QKeyEvent *keyEvent = static_cast<QKeyEvent*>(_event);
    this->dataPtr->KeyPub(keyEvent);
  }
  return QObject::eventFilter(_obj, _event);
}

/*
void KeyPublisher::KeyPub(QKeyEvent *key_press)
{
  
  https://doc.qt.io/archives/qtjambi-4.5.2_01/com/trolltech/qt/core/Qt.Key.html
  
  ignition::msgs::Int32 Msg;
  Msg.set_data(key_press->key());
  this->dataPtr->pub.Publish(Msg);
}
*/
// Register this plugin
IGNITION_ADD_PLUGIN(ignition::gui::KeyPublisher,
                    ignition::gui::Plugin)
