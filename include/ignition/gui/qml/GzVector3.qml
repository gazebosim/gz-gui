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
import QtQuick.Controls.Material 2.1
import QtQuick.Layouts 1.3

/**
 *  Item displaying a 3D vector
 *
 *  Users should load values to xValues, yValues, and zValues.
 *  If readOnly == False,
 *  users can read from signal parameters of GzVectorSet: _x, _y, and _z
 *
 *  Usage example:
 *  GzVector3 {
 *    id: gzVector
 *    xName: "Red"
 *    yName: "Green"
 *    zName: "Blue"
 *    unit: ""
 *    readOnly: false
 *    xValue: xValueFromCPP
 *    yValue: yValueFromCPP
 *    zValue: zValueFromCPP
 *    onGzVectorSet: {
 *      myFunc(_x, _y, _z, _roll, _pitch, _yaw)
 *    }
 *  }
**/
Rectangle {
  id: gzVectorRoot

  // Readn-only / write
  property bool readOnly: true

  // User input value
  property double xValue
  property double yValue
  property double zValue

  /**
   * Useful only when readOnly == false. User should read spinbox values from
   * its parameters.
   */
  signal gzVectorSet(double _x, double _y, double _z)

  // Names for XYZ
  property string xName: "X"
  property string yName: "Y"
  property string zName: "Z"

  // Units, defaults to meters.
  // Set to "" to omit the parentheses.
  property string unit: "m"

  // Show Pose bar (used to control expand)
  property bool show: true

  height: gzVectorContent.height

  // Left indentation
  property int indentation: 10

  // Horizontal margins
  property int margin: 5

  // Maximum spinbox value
  property double spinMax: 1000000

  // local variables to store spinbox values
  property var xItem: {}
  property var yItem: {}
  property var zItem: {}

  // Dummy component to use its functions.
  IgnHelpers {
    id: gzHelper

    // Temperary getDecimals()
    // Remove after merging gz-gui/common_widget_pose
    function getDecimals(_width) {
      if (_width <= 0 || _width > 110)
        return 6

      if (_width <= 80)
        return 2

      return 4
    }
  }

  /**
   * Used to create a spin box
   */
  Component {
    id: writableNumber
    IgnSpinBox {
      id: writableSpin
      value: numberValue
      minimumValue: -spinMax
      maximumValue: spinMax
      decimals: gzHelper.getDecimals(writableSpin.width)
      onEditingFinished: {
        gzVectorRoot.gzVectorSet(xItem.value, yItem.value, zItem.value)
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
    id: gzVectorContent
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
      columns: 4

      // Left spacer
      Item {
        Layout.rowSpan: 3
        width: indentation + margin
      }

      Text {
        text: unit == "" ? xName : xName + ' (' + unit + ')'
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

      // Right spacer
      Item {
        Layout.rowSpan: 3
        width: margin
      }

      Text {
        text: unit == "" ? yName : yName + ' (' + unit + ')'
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
        text: unit == "" ? zName : zName + ' (' + unit + ')'
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
    }
  }
}
