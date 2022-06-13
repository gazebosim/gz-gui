/*
 * Copyright (C) 2022 Open Source Robotics Foundation
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
import QtQuick.Controls 2.1
import QtQuick.Dialogs 1.0
import QtQuick.Layouts 1.3


Item {
  id: gzColorRoot

  property double r: 255
  property double g: 0
  property double b: 0
  property double a: 1.0

  signal colorSet()

  Button {
    id: colorButton
    Layout.row: 1
    Layout.column: 0
    Layout.leftMargin: 5
    ToolTip.text: "Open color dialog"
    ToolTip.visible: hovered
    ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
    background: Rectangle {
      implicitWidth: 40
      implicitHeight: 40
      radius: 5
      border.color: Qt.rgba(r / 255, g / 255, b / 255, a)
      border.width: 2
      color: Qt.rgba(r / 255, g / 255, b / 255, a)
    }
    onClicked: colorDialog.open()

    ColorDialog {
      id: colorDialog
      title: "Choose a color"
      visible: false
      onAccepted: {
        r = colorDialog.color.r * 255
        g = colorDialog.color.g * 255
        b = colorDialog.color.b * 255
        a = colorDialog.color.a
        gzColorRoot.colorSet()
        colorDialog.close()
        console.log(r,g,b,a)
      }
      onRejected: {
        colorDialog.close()
      }
    }

  }
}

