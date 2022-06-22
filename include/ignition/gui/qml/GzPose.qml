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
import QtQuick.Controls 1.4
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.1
import QtQuick.Layouts 1.3
import QtQuick.Controls.Styles 1.4

// Item displaying 3D pose information.
Item {
  id: gzPoseRoot
  height: gzPoseContent.height

  // Left indentation
  property int indentation: 10

  // Horizontal margins
  property int margin: 5

  // Maximum spinbox value
  property double spinMax: 1000000

  // Read-only / write
  property bool readOnly: false

  // Show Pose bar
  property bool show: false

  // Loaded item for Pose
  property var xItem: {}
  property var yItem: {}
  property var zItem: {}
  property var rollItem: {}
  property var pitchItem: {}
  property var yawItem: {}

  property double xModelValue
  property double yModelValue
  property double zModelValue
  property double rollModelValue
  property double pitchModelValue
  property double yawModelValue

  signal gzPoseSet()

  /**
   * Used to create a spin box
   */
  Component {
    id: writableNumber
    IgnSpinBox {
      id: writableSpin
      value:  numberValue
      minimumValue: -spinMax
      maximumValue: spinMax
      decimals: getDecimals(writableSpin.width)
      onEditingFinished: {
        gzPoseRoot.gzPoseSet()
      }
    }
  }

  /**
   * Used to create a read-only number
   */
  Component {
    id: readOnlyNumber
    Text {
      id: numberText
      anchors.fill: parent
      horizontalAlignment: Text.AlignRight
      verticalAlignment: Text.AlignVCenter
      text: {
        var decimals = getDecimals(numberText.width)
        return numberValue.toFixed(decimals)
      }
    }
  }

  Rectangle {
    id: gzPoseContent
    width: parent.width
    height: show ? grid.height : 0
    clip: true
    color: "transparent"

    Behavior on height {
      NumberAnimation {
        duration: 200;
        easing.type: Easing.InOutQuad
      }
    }

    GridLayout {
      id: grid
      width: parent.width
      columns: 6

      // Left spacer
      Item {
        Layout.rowSpan: 3
        width: margin + indentation
      }

      Text {
        text: 'X (m)'
        leftPadding: 5
        color: Material.theme == Material.Light ? "#444444" : "#bbbbbb"
        font.pointSize: 12
      }

      Item {
        Layout.fillWidth: true
        height: 40
        Loader {
          id: xLoader
          anchors.fill: parent
          property double numberValue: xModelValue
          sourceComponent: readOnly ? readOnlyNumber : writableNumber
          onLoaded: {
            xItem = xLoader.item
          }
        }
      }

      Text {
        text: 'Roll (rad)'
        leftPadding: 5
        color: Material.theme == Material.Light ? "#444444" : "#bbbbbb"
        font.pointSize: 12
      }

      Item {
        Layout.fillWidth: true
        height: 40
        Loader {
          id: rollLoader
          anchors.fill: parent
          property double numberValue: rollModelValue
          sourceComponent: readOnly ? readOnlyNumber : writableNumber
          onLoaded: {
            rollItem = rollLoader.item
          }
        }
      }

      // Right spacer
      Item {
        Layout.rowSpan: 3
        width: margin
      }

      Text {
        text: 'Y (m)'
        leftPadding: 5
        color: Material.theme == Material.Light ? "#444444" : "#bbbbbb"
        font.pointSize: 12
      }

      Item {
        Layout.fillWidth: true
        height: 40
        Loader {
          id: yLoader
          anchors.fill: parent
          property double numberValue: yModelValue
          sourceComponent: readOnly ? readOnlyNumber : writableNumber
          onLoaded: {
             yItem = yLoader.item
          }
        }
      }

      Text {
        text: 'Pitch (rad)'
        leftPadding: 5
        color: Material.theme == Material.Light ? "#444444" : "#bbbbbb"
        font.pointSize: 12
      }

      Item {
        Layout.fillWidth: true
        height: 40
        Loader {
          id: pitchLoader
          anchors.fill: parent
          property double numberValue: pitchModelValue
          sourceComponent: readOnly ? readOnlyNumber : writableNumber
          onLoaded: {
            pitchItem = pitchLoader.item
          }
        }
      }

      Text {
        text: 'Z (m)'
        leftPadding: 5
        color: Material.theme == Material.Light ? "#444444" : "#bbbbbb"
        font.pointSize: 12
      }

      Item {
        Layout.fillWidth: true
        height: 40
        Loader {
          id: zLoader
          anchors.fill: parent
          property double numberValue: zModelValue
          sourceComponent: readOnly ? readOnlyNumber : writableNumber
          onLoaded: {
            zItem = zLoader.item
          }
        }
      }

      Text {
        text: 'Yaw (rad)'
        leftPadding: 5
        color: Material.theme == Material.Light ? "#444444" : "#bbbbbb"
        font.pointSize: 12
      }

      Item {
        Layout.fillWidth: true
        height: 40
        Loader {
          id: yawLoader
          anchors.fill: parent
          property double numberValue: yawModelValue
          sourceComponent: readOnly ? readOnlyNumber : writableNumber
          onLoaded: {
            yawItem = yawLoader.item
          }
        }
      }
    } // end of GridLayout
  } // end of Rectangle (gzPoseContent)
} // end of Rectangle (gzPoseRoot)
