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
import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.0
import "qrc:/qml"

Dialog {
  id: settingsDialog

  Column {
    id: settingsColumn
    anchors.horizontalCenter: settingsDialog.horizontalCenter
    width: settingsDialog.width * 0.6

    Switch {
      id: titleSwitch
      text: "Show title bar"
      checked: card.showTitleBar
      onToggled: {
        card.showTitleBar = checked
        // why is binding not working?
        closeSwitch.enabled = checked
        dockSwitch.enabled = checked
      }
    }

    Switch {
      id: closeSwitch
      text: "Show close button"
      visible: !card.standalone
      enabled: card.showTitleBar
      checked: card.showCloseButton
      onToggled: {
        card.showCloseButton = checked
      }
    }

    Switch {
      id: dockSwitch
      text: "Show dock button"
      visible: !card.standalone
      enabled: card.showTitleBar
      checked: card.showDockButton
      onToggled: {
        card.showDockButton = checked
      }
    }

    Switch {
      id: resizableSwitch
      text: "Resizable"
      visible: card.state === "floating"
      checked: card.resizable
      onToggled: {
        card.resizable = checked
      }
    }

    GridLayout {
      width: parent.width
      columns: 3
      visible: !card.standalone

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
      visible: card.state === "floating"

      Label {
        text: "Position"
        font.weight: Font.DemiBold
      }

      Text {
        text: ""
      }

      // TODO(louise) Support setting anchors from the dialog
      Button {
        visible: card.anchored
        text: "Clear anchors"
        Layout.columnSpan: 2
        onClicked: {
          card.clearAnchors()
        }
      }

      IgnSpinBox {
        visible: !card.anchored
        maximumValue: card.parent ? card.parent.width - card.width : minSize
        onVisibleChanged: value = card.x
        onValueChanged: {
          card.x = value;
        }
      }
      Label {
        visible: !card.anchored
        text: "X"
      }
      IgnSpinBox {
        visible: !card.anchored
        maximumValue: card.parent ? card.parent.height - card.height : minSize
        onVisibleChanged: value = card.y
        onValueChanged: {
          card.y = value;
        }
      }
      Label {
        visible: !card.anchored
        text: "Y"
      }
      IgnSpinBox {
        visible: !card.anchored
        maximumValue: 10000
        onVisibleChanged: value = card.z
        onValueChanged: {
          card.z = value;
        }
      }
      Label {
        visible: !card.anchored
        text: "Z"
      }
      Label {
        text: "Size"
        font.weight: Font.DemiBold
      }
      Text {
        text: ""
      }
      IgnSpinBox {
        maximumValue: card.parent ? card.parent.width : minSize
        onVisibleChanged: {
          if (card)
            value = card.width
        }
        onValueChanged: {
          card.width = value;
        }
      }
      Label {
        text: "Width"
      }
      IgnSpinBox {
        maximumValue: card.parent ? card.parent.height : minSize
        onVisibleChanged: {
          if (card)
            value = card.height
        }
        onValueChanged: {
          card.height = value;
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
    showAlphaChannel : true
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
