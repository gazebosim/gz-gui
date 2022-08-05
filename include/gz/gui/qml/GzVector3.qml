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
 *  Users can set values to xValue, yValue, and zValue.
 *  If readOnly == False,
 *  users can read from signal parameters of GzVectorSet: _x, _y, and _z
 *
 *  Usage example:
 *  GzVector3 {
 *    id: gzVector
 *    xName: "Red"
 *    yName: "Green"
 *    zName: "Blue"
 *    gzUnit: ""
 *    readOnly: false
 *    xValue: xValueFromCPP
 *    yValue: yValueFromCPP
 *    zValue: zValueFromCPP
 *    onGzVectorSet: {
 *      myFunc(_x, _y, _z, _roll, _pitch, _yaw)
 *    }
 *  }
**/
Item {
  id: gzVectorRoot

  // Read-only / write
  property bool readOnly: true

  // User input value
  property double xValue
  property double yValue
  property double zValue

  /**
   * Used to read spinbox values
   * @params: _x, _y, _z: corresponding spinBoxes values
   * @note: When readOnly == false, user should read spinbox value from its
   *        parameters.
   *        When readOnly == true, this signal is unused.
   */
  signal gzVectorSet(double _x, double _y, double _z)

  // Names for XYZ
  property string xName: "X"
  property string yName: "Y"
  property string zName: "Z"

  // Units, defaults to meters.
  // Set to "" to omit units & the parentheses.
  property string gzUnit: "m"

  // Expand/Collapse of this widget
  property bool expand: true

  // Maximum spinbox value
  property double spinMax: Number.MAX_VALUE

  /*** The following are private variables: ***/
  height: gzVectorContent.height

  // local variables to store spinbox values
  property var xItem: {}
  property var yItem: {}
  property var zItem: {}

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
    height: expand ? gzVectorGrid.height : 0
    clip: true
    color: "transparent"

    Behavior on height {
      NumberAnimation {
        duration: 200;
        easing.type: Easing.InOutQuad
      }
    }

    GridLayout {
      id: gzVectorGrid
      width: parent.width
      columns: 2

      Text {
        text: gzUnit == "" ? xName : xName + ' (' + gzUnit + ')'
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
        text: gzUnit == "" ? yName : yName + ' (' + gzUnit + ')'
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
        text: gzUnit == "" ? zName : zName + ' (' + gzUnit + ')'
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
