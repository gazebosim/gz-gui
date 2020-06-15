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

#ifndef IGNITION_GUI_KEYPUBLISHER_HH_
#define IGNITION_GUI_KEYPUBLISHER_HH_

#include <ignition/gui/qt.h>
#include <ignition/gui/Plugin.hh>
#include <ignition/transport/Node.hh>


namespace ignition
{
namespace gui
{
  class KeyPublisher : public ignition::gui::Plugin  //inherited from Qobject
  {
    Q_OBJECT

    public: KeyPublisher();    //constructor
    public: virtual ~KeyPublisher();   //destructor
    
    public: virtual void LoadConfig(const tinyxml2::XMLElement *_pluginElem) override;
    protected: bool eventFilter(QObject *_obj, QEvent *_event) override;

    //Publish message
    public: ignition::transport::Node node;
    public: std::string topic = "/gazebo/keyboard/keypress";
    public: ignition::transport::Node::Publisher pub ;
    public: void KeyPub(QKeyEvent *key_press);
  };
}
}

#endif
