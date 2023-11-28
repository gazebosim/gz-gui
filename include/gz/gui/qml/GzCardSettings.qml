/*
 * Copyright (C) 2018 Open Source Robotics Foundation
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
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import QtQuick.Dialogs
import "qrc:/gz/gui/qml"

Dialog {
  id: settingsDialog

  Column {
    id: settingsColumn
    anchors.horizontalCenter: settingsDialog.horizontalCenter
    width: settingsDialog.width * 0.6

    Switch {
      id: titleSwitch
      text: "Show title bar"
      checked: cardPane.showTitleBar
      onToggled: {
        cardPane.showTitleBar = checked
        // why is binding not working?
        closeSwitch.enabled = checked
        dockSwitch.enabled = checked
      }
    }

    Switch {
      id: closeSwitch
      text: "Show close button"
      visible: !cardPane.standalone
      enabled: cardPane.showTitleBar
      checked: cardPane.showCloseButton
      onToggled: {
        cardPane.showCloseButton = checked
      }
    }

    Switch {
      id: dockSwitch
      text: "Show dock button"
      visible: !cardPane.standalone
      enabled: cardPane.showTitleBar
      checked: cardPane.showDockButton
      onToggled: {
        cardPane.showDockButton = checked
      }
    }

    Switch {
      id: collapseSwitch
      text: "Show collapse button"
      visible: !cardPane.standalone
      enabled: cardPane.showTitleBar
      checked: cardPane.showCollapseButton
      onToggled: {
        cardPane.showCollapseButton = checked
      }
    }

    Switch {
      id: resizableSwitch
      text: "Resizable"
      visible: cardPane.state === "floating"
      checked: cardPane.resizable
      onToggled: {
        cardPane.resizable = checked
      }
    }

    GridLayout {
      width: parent.width
      columns: 3
      visible: !cardPane.standalone

      Label {
        text: "Background Color "
      }

      Button {
        Layout.preferredWidth: parent.width * 0.4
        onClicked: colorDialog.open()
        background: Rectangle {
          y: 8
          width: 50
          height: 30
          id: "bgColor"
          color: cardBackground
          border.color: "#000000"
          border.width: 2
        }
      }
    }


    GridLayout {
      width: parent.width
      columns: 2
      visible: cardPane.state === "floating"

      Label {
        text: "Position"
        font.weight: Font.DemiBold
      }

      Text {
        text: ""
      }

      // TODO(louise) Support setting anchors from the dialog
      Button {
        visible: cardPane.anchored
        text: "Clear anchors"
        Layout.columnSpan: 2
        onClicked: {
          cardPane.clearAnchors()
        }
      }

      GzSpinBox {
        visible: !cardPane.anchored
        to: cardPane.parent ? cardPane.parent.width - cardPane.width : from
        onVisibleChanged: value = cardPane.x
        onValueChanged: {
          cardPane.x = value;
        }
      }
      Label {
        visible: !cardPane.anchored
        text: "X"
      }
      GzSpinBox {
        visible: !cardPane.anchored
        to: cardPane.parent ? cardPane.parent.height - cardPane.height : from
        onVisibleChanged: value = cardPane.y
        onValueChanged: {
          cardPane.y = value;
        }
      }
      Label {
        visible: !cardPane.anchored
        text: "Y"
      }
      GzSpinBox {
        visible: !cardPane.anchored
        to: 10000
        onVisibleChanged: value = cardPane.z
        onValueChanged: {
          cardPane.z = value;
        }
      }
      Label {
        visible: !cardPane.anchored
        text: "Z"
      }
      Label {
        text: "Size"
        font.weight: Font.DemiBold
      }
      Text {
        text: ""
      }
      GzSpinBox {
        to: cardPane.parent ? cardPane.parent.width : from
        onVisibleChanged: {
          if (cardPane)
            value = cardPane.width
        }
        onValueChanged: {
          cardPane.width = value;
        }
      }
      Label {
        text: "Width"
      }
      GzSpinBox {
        to: cardPane.parent ? cardPane.parent.height : from

        onVisibleChanged: {
          if (cardPane)
            value = cardPane.height
        }
        onValueChanged: {
          cardPane.height = value;
        }
      }
      Label {
        text: "Height"
      }
    }
  }

  ColorDialog {
    id: colorDialog
    title: "Please choose a color"
    options: [ColorDialog.ShowAlphaChannel]
    onAccepted: {
      content.color = colorDialog.color
      bgColor.color = colorDialog.color
      cardBackground = colorDialog.color
    }
    onRejected: {
      console.log("Canceled")
    }
    Component.onCompleted: visible = false
  }
}
