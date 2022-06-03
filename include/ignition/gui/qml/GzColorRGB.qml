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
import QtQuick.Controls.Material 2.1
import QtQuick.Dialogs 1.0
import QtQuick.Layouts 1.3


GridLayout {
  id: root
  property string colorName

  property alias r: r_spin.value
  property alias g: g_spin.value
  property alias b: b_spin.value
  property alias a: a_spin.value

  property bool textVisible: false

  signal colorSet()
  columns: 6
  columnSpacing: 10

  Text {
    Layout.row: 0
    Layout.column: 2
    text: "Red"
    color: "dimgrey"
    visible: textVisible
  }

  IgnSpinBox {
    id: "r_spin"
    maximumValue: 255
    minimumValue: 0
    value: 255 
    stepSize: 1
    Layout.row: 1
    Layout.column: 2
    onEditingFinished: root.colorSet()
  }

  Text {
    Layout.row: 0
    Layout.column: 3
    text: "Green"
    color: "dimgrey"
    visible: textVisible
  }

  IgnSpinBox {
    id: "g_spin"
    maximumValue: 255
    minimumValue: 0
    value: 0 
    stepSize: 1
    Layout.row: 1
    Layout.column: 3
    onEditingFinished: root.colorSet()
  }

  Text {
    Layout.row: 0
    Layout.column: 4
    text: "Blue"
    color: "dimgrey"
    visible: textVisible
  }

  IgnSpinBox {
    id: "b_spin"
    maximumValue: 255
    minimumValue: 0
    value: 0 
    stepSize: 1
    Layout.row: 1
    Layout.column: 4
    onEditingFinished: root.colorSet()
  }

  Text {
    Layout.row: 0
    Layout.column: 5
    text: "Alpha"
    color: "dimgrey"
    visible: textVisible
  }

  IgnSpinBox {
    id: "a_spin"
    maximumValue: 1.0
    minimumValue: 0
    value: 1.0
    stepSize: 0.1
    decimals: 2
    Layout.row: 1
    Layout.column: 5
    onEditingFinished: root.colorSet()
  }

  Text {
    Layout.row: 1
    Layout.column: 0
    text: parent.colorName
  }

  Button {
    id: ambientButton
    Layout.row: 1
    Layout.column: 1
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
      title: "Choose a grid color"
      visible: false
      onAccepted: {
        r_spin.value = colorDialog.color.r * 255
        g_spin.value = colorDialog.color.g * 255
        b_spin.value = colorDialog.color.b * 255
        a_spin.value = colorDialog.color.a
        root.colorSet()
        colorDialog.close()
      }
      onRejected: {
        colorDialog.close()
      }
    }

  }
}

