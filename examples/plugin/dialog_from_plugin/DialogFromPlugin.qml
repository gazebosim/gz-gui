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
import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Dialogs 1.0
import QtQuick.Window 2.2

Rectangle {
  id: customPlugin
  objectName: 'customPlugin'
  color: 'transparent'
  height: 300
  width: 300

  function windowWidth() {
    return customPlugin.Window.window ? (customPlugin.Window.window.width) : 0
  }
  function windowHeight() {
    return customPlugin.Window.window ? (customPlugin.Window.window.height) : 0
  }

  Dialog {
    id: customDialog
    parent: customPlugin.Window.window ? customPlugin.Window.window.contentItem : customPlugin
    x: (windowWidth() - width) / 2
    y: (windowHeight() - height) / 2
    width: windowWidth() * 0.3
    height: windowHeight() * 0.3
    modal: true
    focus: true
    title: "Custom dialog"

    Label {
      text: "<br> Window width: " + windowWidth() +
            "<br> Window height: " + windowHeight()
      font.weight: Font.Bold
    }
  }

  Button {
    text: "Open dialog"
    font.pixelSize: 20
    onClicked: {
      customDialog.open()
    }
  }
}
