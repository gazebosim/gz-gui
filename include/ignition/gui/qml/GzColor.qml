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
import QtQuick.Controls.Material 2.1


// RGBA using range 0 - 1.0
Item {
  id: gzColorRoot

  implicitWidth: 40
  implicitHeight: 40

  property double r: 1.0
  property double g: 0.0
  property double b: 0.0
  property double a: 1.0

  signal gzColorSet()

  Button {
    id: gzColorButton
    Layout.leftMargin: 5
    ToolTip.text: "Open color dialog"
    ToolTip.visible: hovered
    ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
    background: Rectangle {
      implicitWidth: 40
      implicitHeight: 40
      radius: 5
      border.color: (Material.theme === Material.Light) ? Qt.rgba(0,0,0,1) : Qt.rgba(1,1,1,1)
      border.width: 2
      color: Qt.rgba(r,g,b,a)
    }
    onClicked: gzColorDialog.open()
  }

  ColorDialog {
    id: gzColorDialog
    title: "Choose a color"
    visible: false
    showAlphaChannel: true
    onAccepted: {
      r = gzColorDialog.color.r
      g = gzColorDialog.color.g
      b = gzColorDialog.color.b
      a = gzColorDialog.color.a
      gzColorRoot.gzColorSet()
      gzColorDialog.close()
    }
    onRejected: {
      gzColorDialog.close()
    }
  }
}

