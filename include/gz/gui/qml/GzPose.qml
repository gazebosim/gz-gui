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
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.1
import QtQuick.Layouts 1.3
import QtQuick.Controls.Styles 1.4

/**
 *  Item displaying 3D pose information.
 *
 *  Users can set values to xValue, yValue, etc.
 *  If readOnly == False,
 *  users can read from signal parameters of gzPoseSet: _x, _y, etc.
 *
 *  Usage example:
 *  GzPose {
 *    id: gzPose
 *    readOnly: false
 *    xValue: xValueFromCPP
 *    yValue: yValueFromCPP
 *    zValue: zValueFromCPP
 *    rollValue: rollValueFromCPP
 *    pitchValue: pitchValueFromCPP
 *    yawValue: yawValueFromCPP
 *    onGzPoseSet: {
 *      myFunc(_x, _y, _z, _roll, _pitch, _yaw)
 *    }
 *  }
**/

Item {
  id: gzPoseRoot

  // Read-only / write
  property bool readOnly: false

  // User input value.
  property double xValue
  property double yValue
  property double zValue
  property double rollValue
  property double pitchValue
  property double yawValue

  /**
   * Used to read spinbox values
   * @params: _x, _y, _z, _roll, _pitch, _yaw: corresponding spinBoxes values
   * @note: When readOnly == false, user should read spinbox value from its
   *        parameters.
   *        When readOnly == true, this signal is unused.
   */
  signal gzPoseSet(double _x, double _y, double _z, double _roll, double _pitch, double _yaw)

  // Maximum spinbox value
  property double spinMax: Number.MAX_VALUE

  // Expand/Collapse of this widget
  property bool expand: true


  /*** The following are private variables: ***/
  height: gzPoseContent.height

  // local variables to store spinbox values
  property var xItem: {}
  property var yItem: {}
  property var zItem: {}
  property var rollItem: {}
  property var pitchItem: {}
  property var yawItem: {}

  // Dummy component to use its functions.
  GzHelpers {
    id: gzHelper
  }
  /*** Private variables end: ***/

  /**
   * Used to create a spin box
   */
  Component {
    id: writableNumber
    GzSpinBox {
      id: writableSpin
      value:  numberValue
      minimumValue: -spinMax
      maximumValue: spinMax
      decimals: gzHelper.getDecimals(writableSpin.width)
      onEditingFinished: {
        gzPoseRoot.gzPoseSet(xItem.value, yItem.value, zItem.value, rollItem.value, pitchItem.value, yawItem.value)
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
        var decimals = gzHelper.getDecimals(numberText.width)
        return numberValue.toFixed(decimals)
      }
    }
  }

  Rectangle {
    id: gzPoseContent
    width: parent.width
    height: expand ? gzPoseGrid.height : 0
    clip: true
    color: "transparent"

    Behavior on height {
      NumberAnimation {
        duration: 200;
        easing.type: Easing.InOutQuad
      }
    }

    GridLayout {
      id: gzPoseGrid
      width: parent.width
      columns: 4

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
          property double numberValue: xValue
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
          property double numberValue: rollValue
          sourceComponent: readOnly ? readOnlyNumber : writableNumber
          onLoaded: {
            rollItem = rollLoader.item
          }
        }
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
          property double numberValue: yValue
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
          property double numberValue: pitchValue
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
          property double numberValue: zValue
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
          property double numberValue: yawValue
          sourceComponent: readOnly ? readOnlyNumber : writableNumber
          onLoaded: {
            yawItem = yawLoader.item
          }
        }
      }
    } // end of GridLayout
  } // end of Rectangle (gzPoseContent)
} // end of Rectangle (gzPoseRoot)
