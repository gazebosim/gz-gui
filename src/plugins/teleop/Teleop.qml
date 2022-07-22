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

import QtQuick 2.9
import QtQuick.Controls 1.4
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.1
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.3
import ignition.gui 1.0

ColumnLayout {
  Layout.minimumWidth: 400
  Layout.minimumHeight: 900
  Layout.margins: 5
  spacing: 5
  anchors.fill: parent
  focus: true

  property double maxLinearVel: Teleop.maxLinearVel
  property double maxAngularVel: Teleop.maxAngularVel

  function sendCommand(_linScale, _angScale) {
    // Adjust sign, sliders don't send max
    var linVel = _linScale * maxLinearVel;
    var angVel = _angScale * maxAngularVel;

console.log(linVel, angVel);

    Teleop.OnTeleopTwist(linVel, angVel)
  }

  // Topic input
  Label {
    id: topicLabel
    text: "Topic"
    Layout.fillWidth: true
    Layout.margins: 5
  }
  TextField {
    id: topicField
    Layout.fillWidth: true
    text: Teleop.topic
    placeholderText: qsTr("Topic to publish...")
    onEditingFinished: {
      Teleop.SetTopic(text)
    }
  }

  // Velocity input
  Label {
    id: velocityLabel
    text: "Velocity"
  }

  RowLayout {
    Layout.fillWidth: true
    // Linear velocity input
    Label {
      id: maxLinearVelLabel
      text: "Linear (m/s)"
      color: "dimgrey"
    }
    IgnSpinBox {
      id: maxLinearVelField
      Layout.fillWidth: true
      value: maxLinearVel
      maximumValue: 10000.0
      minimumValue: 0.0
      decimals: 2
      stepSize: 0.10
      onEditingFinished:{
        Teleop.SetMaxLinearVel(value)
      }
    }

    // Angular velocity input
    Label {
      id: maxAngularVelLabel
      text: "Angular (rad/s)"
      color: "dimgrey"
    }
    IgnSpinBox {
      id: maxAngularVelField
      Layout.fillWidth: true
      value: maxAngularVel
      maximumValue: 10000.0
      minimumValue: 0.0
      decimals: 2
      stepSize: 0.10
      onEditingFinished:{
        Teleop.SetMaxAngularVel(value)
      }
    }
  }

  TabView {
    frameVisible: false
    Layout.fillHeight: true
    Layout.fillWidth: true
    Tab {
      id: buttonsTab
      title: "Buttons"

      // Button grid
      GridLayout {
        id: buttonsGrid
        Layout.fillWidth: true
        columns: 3

        function linScale() {
          if (forwardButton.checked)
            return 1;
          else if (backwardButton.checked)
            return -1;

          return 0;
        }
        function angScale() {
          if (leftButton.checked)
            return 1;
          else if (rightButton.checked)
            return -1;

          return 0;
        }

        Button {
          id: forwardButton
          text: "\u25B2"
          checkable: true
          Layout.row: 0
          Layout.column: 1
          onClicked: {
            if(backwardButton.checked)
              backwardButton.checked = false
            sendCommand(linScale(), angScale());
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
            sendCommand(linScale(), angScale());
          }
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
            if(leftButton.checked)
              leftButton.checked = false
            sendCommand(linScale(), angScale());
          }
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
            if(forwardButton.checked)
              forwardButton.checked = false
            sendCommand(linScale(), angScale());
          }
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
            sendCommand(linScale(), angScale());
          }
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
        // Bottom spacer
        Item {
          Layout.row: 3
          Layout.column: 0
          Layout.fillHeight: true
        }
      }
    }
    Tab {
      title: "Keyboard"

      onActiveChanged: {
        Teleop.OnKeySwitch(active);
      }

      Text {
        textFormat: Text.RichText
        text: "Input from keyboard:</br><ul>" +
              "<li><b>W</b>: Forward</li>" +
              "<li><b>A</b>: Left</li>" +
              "<li><b>S</b>: Back</li>" +
              "<li><b>D</b>:Right</li></ul>"
      }
    }
    Tab {
      title: "Slider"

      GridLayout {
        columns: 4

        Label {
          text: "Linear (m/s)"
        }

        Label {
          width: 40
          text: -maxLinearVel
        }

        Slider {
          id: linearVelSlider
          height: 150
          width: 50
          from: -1.0
          to: 1.0
          stepSize: 0.01

          onMoved: {
            sendCommand(linearVelSlider.value, angularVelSlider.value);
          }
        }

        Label {
          width: 40
          text: maxLinearVel
        }

        Label {
          text: "Angular (rad/s)"
        }

        Label {
          width: 40
          text: -maxAngularVel
        }

        Slider {
          id: angularVelSlider
          height: 50
          width: 175
          from: -1.0
          to: 1.0
          stepSize: 0.01

          onMoved: {
            sendCommand(linearVelSlider.value, angularVelSlider.value);
          }
        }

        Label {
          width: 40
          text: maxAngularVel
        }

        // Bottom spacer
        Item {
          Layout.row: 2
          Layout.column: 0
          Layout.fillHeight: true
        }
      }
    }
  }
}
