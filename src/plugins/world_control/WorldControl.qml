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
import QtQuick.Controls 2.5
import QtQuick.Controls.Material 2.1
import QtQuick.Layouts 1.3
import "qrc:/qml"

RowLayout {
  id: worldControl
  spacing: 10
  Layout.leftMargin: 10
  Layout.minimumWidth: 150
  Layout.minimumHeight: 100

  Connections {
    target: _WorldControl
    function onPlaying() {
      paused = false;
    }
    function onPaused() {
      paused = true;
    }
  }

  Shortcut {
    sequence: "Space"
    onActivated: {
      if (paused)
        _WorldControl.OnPlay()
      else
        _WorldControl.OnPause()
    }
  }

  /**
   * True to show play/pause button
   */
  property bool showPlay: false

  /**
   * True to show reset button
   */
  property bool showReset: true

  /**
   * True to show step buttons
   */
  property bool showStep: false

  /**
   * True to show multistep popup
   */
  property bool showMultiStep: false

  /**
   * Play icon
   */
  property string playIcon: "\u25B6"

  /**
   * Reset button
   */
  property string resetIcon: "\u21bb"

  /**
   * Pause icon
   */
  property string pauseIcon: "\u275A\u275A"

  /**
   * Step icon
   */
  property string stepIcon: "\u25B8\u25B8"

  /**
   * True if paused, false is playing
   */
  property bool paused: false

  /**
   * Play / pause
   */
  RoundButton {
    id: playButton
    objectName: "playButton"
    visible: showPlay
    text: paused ? playIcon : pauseIcon
    checkable: true
    Layout.alignment : Qt.AlignVCenter
    onClicked: {
      if (paused)
        _WorldControl.OnPlay()
      else
        _WorldControl.OnPause()
    }
    ToolTip.visible: hovered
    ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
    ToolTip.text: paused ? qsTr("Run the simulation") : qsTr("Pause the simulation")
    Material.background: Material.primary
  }

  /**
   * Step button
   */
  Rectangle {
     width: stepButton.width
     height: stepButton.height
     color: "transparent"

    MouseArea {
      id: buttonHoverArea
      hoverEnabled: true
      anchors.fill: parent

      onEntered: {
        var minX = 0;
        var maxX = worldControl.parent.card().parent.width -
            stepPopup.width * 0.5;
        var popX = stepButton.windowPos().x - stepPopup.width * 0.5;

        stepPopup.x = Math.min(Math.max(popX, minX), maxX);
        stepPopup.y = stepButton.windowPos().y - stepPopup.height + 4;
      }

      RoundButton {
        id: stepButton
        objectName: "stepButton"
        text: stepIcon
        visible: showStep
        height: playButton.height * 0.8
        width: playButton.width * 0.8
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
        onClicked: {
          _WorldControl.OnStep()
        }
        Material.background: Material.primary

        function windowPos() {
          return mapToItem(worldControl.parent.card().parent, 0, 0);
        }
      }
    }
  }

  Popup {
    id: stepPopup
    visible: buttonHoverArea.containsMouse || popupHoverArea.containsMouse
    parent: worldControl.parent ? worldControl.parent.card().parent : null

    contentItem: MouseArea {
      id: popupHoverArea
      anchors.fill: parent
      hoverEnabled: true

      RowLayout {
        id: row
        anchors.fill: parent

        Label {
          text: "Steps"
          Layout.alignment: Qt.AlignVCenter
          Layout.leftMargin: 15
        }

        GzSpinBox {
          maximumValue: 10000
          Layout.alignment: Qt.AlignVCenter
          value: 1
          onValueChanged: {
            _WorldControl.OnStepCount(value)
          }
        }
      }
    }

    exit: Transition {
      NumberAnimation {
        property: "opacity"
        from: 1.0
        to: 0.0
        duration: 500
        easing.type: Easing.OutExpo
      }
    }
  }

  /**
   * Reset
   */
  RoundButton {
    id: resetButton
    objectName: "resetButton"
    visible: showReset
    text: resetIcon
    checkable: true
    implicitHeight: stepButton.height
    implicitWidth: stepButton.width
    onClicked: {
      confirmationDialogOnReset.open()
    }
    Material.background: Material.primary
    ToolTip.visible: hovered
    ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
    ToolTip.text: qsTr("Reset the simulation")
  }

  Shortcut {
    sequence: "Ctrl+R"
    onActivated: confirmationDialogOnReset.open()
  }

  /**
   *  Confirmation dialog on close button
   */
  Dialog {
    id: confirmationDialogOnReset
    title: "Do you really want to reset the simulation?"
    objectName: "confirmationDialogOnReset"

    modal: true
    focus: true
    parent: ApplicationWindow.overlay
    width: 500
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
    closePolicy: Popup.CloseOnEscape
    standardButtons: Dialog.Ok  | Dialog.Discard

    onAboutToShow: function () {
      footer.standardButton(Dialog.Discard).text = "Cancel"
      footer.standardButton(Dialog.Ok).text = "Reset"
    }

    footer:
      DialogButtonBox
      {
        onClicked: function (btn)
        {
          confirmationDialogOnReset.close()
          if (btn == this.standardButton(Dialog.Ok))
          {
            _WorldControl.OnReset()
          }
        }
      }
  }
}
