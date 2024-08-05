/*
 * Copyright (C) 2021 Open Source Robotics Foundation
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
import QtQuick.Controls.Material
import QtQuick.Layouts
import gz.gui
import "qrc:/gz/gui/qml"

ColumnLayout {
  Layout.minimumWidth: 400
  Layout.minimumHeight: 650
  Layout.margins: 5
  anchors.fill: parent
  focus: true

  // Maximum forward velocity
  property double maxForwardVel: Teleop.maxForwardVel

  // Maximum vertical velocity
  property double maxVerticalVel: Teleop.maxVerticalVel

  // Maximum yaw velocity
  property double maxYawVel: Teleop.maxYawVel

  // Send command according to given scale
  function sendCommand(_forwardScale, _verticalScale, _yawScale) {
    var forwardVel = _forwardScale * maxForwardVel;
    var verticalVel = _verticalScale * maxVerticalVel;
    var yawVel = _yawScale * maxYawVel;
    Teleop.OnTeleopTwist(forwardVel, verticalVel, yawVel)
  }

  // Forward scale based on button state
  function forwardScale() {
    if (forwardButton.checked)
      return 1;
    else if (backwardButton.checked)
      return -1;

    return 0;
  }

  // Vertical scale based on button state
  function verticalScale() {
    if (upButton.checked)
      return 1;
    else if (downButton.checked)
      return -1;

    return 0;
  }

  // Yaw scale based on button state
  function yawScale() {
    if (leftButton.checked)
      return 1;
    else if (rightButton.checked)
      return -1;

    return 0;
  }

  // Topic input
  Label {
    id: topicLabel
    text: "Topic"
    Layout.fillWidth: true
    Layout.margins: 10
  }
  TextField {
    id: topicField
    Layout.fillWidth: true
    Layout.margins: 10
    text: Teleop.topic
    placeholderText: qsTr("Topic to publish...")
    onEditingFinished: {
      Teleop.SetTopic(text)
    }
  }

  // Velocity input
  Label {
    id: velocityLabel
    text: "Maximum velocity"
    Layout.margins: 10
    ToolTip.text: "Value that's set by buttons and keys, and scaled by sliders."
    ToolTip.visible: velocityLabelMA.containsMouse
    MouseArea {
        id: velocityLabelMA
        anchors.fill: parent
        hoverEnabled: true
    }
  }

  GridLayout {
    Layout.fillWidth: true
    Layout.margins: 10
    columns: 2

    // Forward velocity input
    Label {
      id: maxForwardVelLabel
      text: "Forward (m/s)"
      color: "dimgrey"
    }
    GzSpinBox {
      id: maxForwardVelField
      Layout.fillWidth: true
      value: maxForwardVel
      maximumValue: 10000.0
      minimumValue: 0.0
      decimals: 2
      stepSize: 0.10
      onEditingFinished: {
        Teleop.SetMaxForwardVel(value)
      }
    }

    // Vertical velocity input
    Label {
      id: maxVerticalVelLabel
      text: "Vertical (m/s)"
      color: "dimgrey"
    }
    GzSpinBox {
      id: maxVerticalVelField
      Layout.fillWidth: true
      value: maxVerticalVel
      maximumValue: 10000.0
      minimumValue: 0.0
      decimals: 2
      stepSize: 0.10
      onEditingFinished:{
        Teleop.SetMaxVerticalVel(value)
      }
    }

    // Yaw velocity input
    Label {
      id: maxYawVelLabel
      text: "Yaw (rad/s)"
      color: "dimgrey"
    }
    GzSpinBox {
      id: maxYawVelField
      Layout.fillWidth: true
      value: maxYawVel
      maximumValue: 10000.0
      minimumValue: 0.0
      decimals: 2
      stepSize: 0.10
      onEditingFinished:{
        Teleop.SetMaxYawVel(1.0)
      }
    }
  }

  TabBar {
    id: tabs
    Layout.fillWidth: true
    Layout.margins: 10

    onCurrentIndexChanged: {
      Teleop.OnKeySwitch(currentIndex == 1);
    }

    TabButton {
      text: qsTr("Buttons")
    }
    TabButton {
      text: qsTr("Keyboard")
    }
    TabButton {
      text: qsTr("Sliders")
    }
  }

  StackLayout {
    Layout.fillHeight: true
    Layout.fillWidth: true
    Layout.margins: 10
    currentIndex: tabs.currentIndex

    // Buttons
    Item {
      GridLayout {
        id: buttonsGrid
        width: parent.width
        columns: 4

        Button {
          id: forwardButton
          text: "\u25B2"
          checkable: true
          Layout.row: 0
          Layout.column: 1
          onClicked: {
            if (backwardButton.checked)
              backwardButton.checked = false
            sendCommand(forwardScale(), verticalScale(), yawScale());
          }
          ToolTip.visible: hovered
          ToolTip.text: "Forward"
          Material.background: Material.primary
          contentItem: Label {
            renderType: Text.NativeRendering
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font.family: "Helvetica"
            font.pointSize: 10
            color: "black"
            text: forwardButton.text
          }
        }
        Button {
          id: leftButton
          text: "\u25C0"
          checkable: true
          Layout.row: 1
          Layout.column: 0
          onClicked: {
            if (rightButton.checked)
              rightButton.checked = false
            sendCommand(forwardScale(), verticalScale(), yawScale());
          }
          ToolTip.visible: hovered
          ToolTip.text: "Left"
          Material.background: Material.primary
          contentItem: Label {
            renderType: Text.NativeRendering
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font.family: "Helvetica"
            font.pointSize: 10
            color: "black"
            text: leftButton.text
          }
        }
        Button {
          id: rightButton
          text: "\u25B6"
          checkable: true
          Layout.row: 1
          Layout.column: 2
          onClicked: {
            if (leftButton.checked)
              leftButton.checked = false
            sendCommand(forwardScale(), verticalScale(), yawScale());
          }
          ToolTip.visible: hovered
          ToolTip.text: "Right"
          Material.background: Material.primary
          contentItem: Label {
            renderType: Text.NativeRendering
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font.family: "Helvetica"
            font.pointSize: 10
            color: "black"
            text: rightButton.text
          }
        }
        Button {
          id: backwardButton
          text: "\u25BC"
          checkable: true
          Layout.row: 2
          Layout.column: 1
          onClicked: {
            if (forwardButton.checked)
              forwardButton.checked = false
            sendCommand(forwardScale(), verticalScale(), yawScale());
          }
          ToolTip.visible: hovered
          ToolTip.text: "Back"
          Material.background: Material.primary
          contentItem: Label {
            renderType: Text.NativeRendering
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font.family: "Helvetica"
            font.pointSize: 10
            color: "black"
            text: backwardButton.text
          }
        }
        Button {
          id: stopButton
          text: "Stop"
          checkable: false
          Layout.row: 1
          Layout.column: 1
          onClicked: {
            forwardButton.checked = false
            leftButton.checked = false
            rightButton.checked = false
            backwardButton.checked = false
            upButton.checked = false
            downButton.checked = false
            sendCommand(forwardScale(), verticalScale(), yawScale());
          }
          ToolTip.visible: hovered
          ToolTip.text: "Stop"
          Material.background: Material.primary
          contentItem: Label {
            renderType: Text.NativeRendering
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font.family: "Helvetica"
            font.pointSize: 10
            color: "black"
            text: stopButton.text
          }
        }

        Button {
          id: upButton
          text: "\u2191"
          checkable: true
          Layout.row: 0
          Layout.column: 3
          onClicked: {
            if (downButton.checked)
              downButton.checked = false
            sendCommand(forwardScale(), verticalScale(), yawScale());
          }
          ToolTip.visible: hovered
          ToolTip.text: "Up"
          Material.background: Material.primary
          contentItem: Label {
            renderType: Text.NativeRendering
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font.family: "Helvetica"
            font.pointSize: 10
            color: "black"
            text: upButton.text
          }
        }

        Button {
          id: downButton
          text: "\u2193"
          checkable: true
          Layout.row: 2
          Layout.column: 3
          onClicked: {
            if (upButton.checked)
              upButton.checked = false
            sendCommand(forwardScale(), verticalScale(), yawScale());
          }
          ToolTip.visible: hovered
          ToolTip.text: "Down"
          Material.background: Material.primary
          contentItem: Label {
            renderType: Text.NativeRendering
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font.family: "Helvetica"
            font.pointSize: 10
            color: "black"
            text: downButton.text
          }
        }
        // Bottom spacer
        Item {
          Layout.row: 3
          Layout.column: 0
          Layout.fillHeight: true
        }
      }
    }

    // Keyboard
    Item {
      width: parent.width
      Text {
        textFormat: Text.RichText
        text: "Hold keys:</br><ul>" +
              "<li><b>W</b>: Forward</li>" +
              "<li><b>A</b>: Left</li>" +
              "<li><b>S</b>: Back</li>" +
              "<li><b>D</b>: Right</li>" +
              "<li><b>Q</b>: Up</li>" +
              "<li><b>E</b>: Down</li></ul>"
      }
    }

    // Sliders
    Item {
      width: parent.width

      GridLayout {
        columns: 4
        columnSpacing: 10
        width: parent.width

        // Forward
        Label {
          text: "Forward (m/s)"
        }

        Label {
          text: (-maxForwardVel).toFixed(2)
        }

        Slider {
          id: forwardVelSlider
          Layout.fillWidth: true
          from: -1.0
          to: 1.0
          stepSize: 0.01

          onMoved: {
            sendCommand(forwardVelSlider.value, verticalVelSlider.value, yawVelSlider.value);
          }
        }

        Label {
          text: maxForwardVel.toFixed(2)
        }

        // Vertical
        Label {
          text: "Vertical (m/s)"
        }

        Label {
          text: (-maxVerticalVel).toFixed(2)
        }

        Slider {
          id: verticalVelSlider
          Layout.fillWidth: true
          from: -1.0
          to: 1.0
          stepSize: 0.01

          onMoved: {
            sendCommand(forwardVelSlider.value, verticalVelSlider.value, yawVelSlider.value);
          }
        }

        Label {
          text: maxVerticalVel.toFixed(2)
        }

        // Yaw
        Label {
          text: "Yaw (rad/s)"
        }

        Label {
          text: (-maxYawVel).toFixed(2)
        }

        Slider {
          id: yawVelSlider
          Layout.fillWidth: true
          from: -1.0
          to: 1.0
          stepSize: 0.01

          onMoved: {
            sendCommand(forwardVelSlider.value, verticalVelSlider.value, yawVelSlider.value);
          }
        }

        Label {
          text: maxYawVel.toFixed(2)
        }

        Button {
          text: "Stop"
          Layout.columnSpan: 4
          onClicked: {
            forwardVelSlider.value = 0.0;
            verticalVelSlider.value = 0.0;
            yawVelSlider.value = 0.0;
            sendCommand(forwardVelSlider.value, verticalVelSlider.value, yawVelSlider.value);
          }
          ToolTip.visible: hovered
          ToolTip.text: "Stop"
          Material.background: Material.primary
        }

        // Bottom spacer
        Item {
          Layout.fillHeight: true
        }
      }
    }
  }
}
