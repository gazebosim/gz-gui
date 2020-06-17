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
import QtQuick.Controls.Material 2.1
import QtQuick.Layouts 1.3

Rectangle {
  id: worldStats
  width: panel.implicitWidth + hideToolButton.width + 20
  height: panel.implicitHeight + 10
  color: "transparent"
  Layout.minimumWidth: 290
  Layout.minimumHeight: 110

  /**
   * True to show sim time
   */
  property bool showSimTime: false

  /**
   * True to show real time
   */
  property bool showRealTime: false

  /**
   * True to show real time factor
   */
  property bool showRealTimeFactor: false

  /**
   * True to show iterations
   */
  property bool showIterations: false

  property int tooltipDelay: 500
  property int tooltipTimeout: 1000

  RowLayout {
    id: hideButton
    height: worldStats.height
    ToolButton {
      id: hideToolButton
      text: panel.state === "hide" ? "\u2039" : "\u203A"
      Layout.alignment: Qt.AlignBottom
      font.pixelSize: 20
      onClicked: {
        panel.state = panel.state === "hide" ? "show" : "hide"
      }
    }
    Label {
      id: compactLabel
      visible: showRealTimeFactor
      width: 60
      verticalAlignment: Text.AlignVCenter
      Layout.alignment: Qt.AlignBottom
      Layout.bottomMargin: hideToolButton.height * 0.3
      text: WorldStats.realTimeFactor
    }
  }

  Rectangle {
    id: panel
    state: "hide"
    default property alias data: grid.data
    anchors.bottom: worldStats.bottom
    implicitWidth: grid.implicitWidth + 10
    implicitHeight: grid.implicitHeight + 10
    // color: "#22000000"
    color: "transparent"

    states:[
      State {
        name:"hide"
        PropertyChanges {
          target: panel
          x: worldStats.width
          opacity: 0
        }
        PropertyChanges {
          target: hideButton
          x: worldStats.width - hideToolButton.width - compactLabel.width - 10
        }
        PropertyChanges {
          target: compactLabel
          Layout.maximumWidth: 60
          opacity: 1
        }
      },
      State {
        name:"show"
        PropertyChanges {
          target: panel
          x: hideButton.width + 5
          opacity: 1
        }
        PropertyChanges {
          target: hideButton
          x: 0
        }
        PropertyChanges {
          target: compactLabel
          Layout.maximumWidth: 0
          opacity: 0
        }
      }
    ]

    transitions: [
      Transition {
        to:"show"
        NumberAnimation
        {
          duration: 500
          properties:"x,width,opacity"
          easing.type: Easing.OutCubic
        }
      },
      Transition {
        to:"hide"
        NumberAnimation
        {
          duration: 500
          properties:"x,width,opacity"
          easing.type: Easing.InCubic
        }
      }
    ]

    GridLayout {
      id: grid
      columns: 2
      anchors.fill: parent
      anchors.margins: 5

      /**
       * Real time factor
       */
      Label {
        text: "RTF"
        visible: showRealTimeFactor
        font.weight: Font.DemiBold
        ToolTip.visible: realTimeFactorMa.containsMouse
        ToolTip.delay: tooltipDelay
        ToolTip.timeout: tooltipTimeout
        ToolTip.text: qsTr("Real time factor")

        MouseArea {
          id: realTimeFactorMa
          anchors.fill: parent
          hoverEnabled: true
        }
      }
      Label {
        text: WorldStats.realTimeFactor
        visible: showRealTimeFactor
        Layout.alignment: Qt.AlignRight
      }

      /**
       * Sim time
       */
      Label {
        text: "Sim time"
        visible: showSimTime
        font.weight: Font.DemiBold
        ToolTip.visible: simTimeMa.containsMouse
        ToolTip.delay: tooltipDelay
        ToolTip.timeout: tooltipTimeout
        ToolTip.text: qsTr("Simulation time")

        MouseArea {
          id: simTimeMa
          anchors.fill: parent
          hoverEnabled: true
        }
      }
      Label {
        text: WorldStats.simTime
        visible: showSimTime
        Layout.alignment: Qt.AlignRight
      }

      /**
       * Real time
       */
      Label {
        text: "Real time"
        visible: showRealTime
        font.weight: Font.DemiBold
        ToolTip.visible: realTimeMa.containsMouse
        ToolTip.delay: tooltipDelay
        ToolTip.timeout: tooltipTimeout
        ToolTip.text: qsTr("Wall-clock time")

        MouseArea {
          id: realTimeMa
          anchors.fill: parent
          hoverEnabled: true
        }
      }
      Label {
        text: WorldStats.realTime
        visible: showRealTime
        Layout.alignment: Qt.AlignRight
      }

      /**
       * Iterations
       */
      Label {
        text: "Iterations"
        visible: showIterations
        font.weight: Font.DemiBold
        ToolTip.visible: iterationsMa.containsMouse
        ToolTip.delay: tooltipDelay
        ToolTip.timeout: tooltipTimeout
        ToolTip.text: qsTr("Simulation iterations")

        MouseArea {
          id: iterationsMa
          anchors.fill: parent
          hoverEnabled: true
        }
      }
      Label {
        text: WorldStats.iterations
        visible: showIterations
        Layout.alignment: Qt.AlignRight
      }
    }
  }
}
