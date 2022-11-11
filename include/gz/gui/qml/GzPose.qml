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
 *  To enable plotting, set gzPlotEnabled = true.
 *  If true, gzPlotMimeData* is required.
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
 *    gzPlotEnabled: true
 *    gzPlotMimeDataX: {"text/plain" : "Component," + entityFromCpp + "," + typeIdFromCpp + ",x," + typeNameFromCpp}
 *    gzPlotMimeDataY: {"text/plain" : "Component," + entityFromCpp + "," + typeIdFromCpp + ",y," + typeNameFromCpp}
 *    gzPlotMimeDataZ: {"text/plain" : "Component," + entityFromCpp + "," + typeIdFromCpp + ",z," + typeNameFromCpp}
 *    gzPlotMimeDataRoll: {"text/plain" : "Component," + entityFromCpp + "," + typeIdFromCpp + ",roll," + typeNameFromCpp}
 *    gzPlotMimeDataPitch:{"text/plain" : "Component," + entityFromCpp + "," + typeIdFromCpp + ",pitch," + typeNameFromCpp}
 *    gzPlotMimeDataYaw: {"text/plain" : "Component," + entityFromCpp + "," + typeIdFromCpp + ",yaw," + typeNameFromCpp}
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

  // Display/Hide plotting icons
  property bool gzPlotEnabled: false

  // Data for plotting, see GzPlotIcon.qml for more details
  property var gzPlotMimeDataX: {"text/plain" : ""}
  property var gzPlotMimeDataY: {"text/plain" : ""}
  property var gzPlotMimeDataZ: {"text/plain" : ""}
  property var gzPlotMimeDataRoll: {"text/plain" : ""}
  property var gzPlotMimeDataPitch: {"text/plain" : ""}
  property var gzPlotMimeDataYaw: {"text/plain" : ""}

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
        gzPoseRoot.gzPoseSet(xItem.value, yItem.value, zItem.value,
                             rollItem.value, pitchItem.value, yawItem.value)
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

  /**
   * Used to create a plotting icon
   */
  Component {
    id: gzPlotIcon
    GzPlotIcon {
      gzMimeData: gzLoaderMimeData
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

      Rectangle {
        color: "transparent"
        height: 40
        Layout.preferredWidth: xText.width + 40
        Loader {
          id: xPlot
          active: gzPlotEnabled
          sourceComponent: gzPlotIcon
          property var gzLoaderMimeData: gzPlotMimeDataX
        }

        Text {
          id: xText
          text: 'X (m)'
          leftPadding: 5
          color: Material.theme == Material.Light ? "#444444" : "#bbbbbb"
          font.pointSize: 12
          anchors.centerIn: parent
        }
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

      Rectangle {
        color: "transparent"
        height: 40
        Layout.preferredWidth: rollText.width + 40
        Loader {
          id: rollPlot
          active: gzPlotEnabled
          sourceComponent: gzPlotIcon
          property var gzLoaderMimeData: gzPlotMimeDataRoll
        }

        Text {
          id: rollText
          text: 'Roll (rad)'
          leftPadding: 5
          color: Material.theme == Material.Light ? "#444444" : "#bbbbbb"
          font.pointSize: 12
          anchors.centerIn: parent
        }
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

      Rectangle {
        color: "transparent"
        height: 40
        Layout.preferredWidth: yText.width + 40
        Loader {
          id: yPlot
          active: gzPlotEnabled
          sourceComponent: gzPlotIcon
          property var gzLoaderMimeData: gzPlotMimeDataY
        }

        Text {
          id: yText
          text: 'Y (m)'
          leftPadding: 5
          color: Material.theme == Material.Light ? "#444444" : "#bbbbbb"
          font.pointSize: 12
          anchors.centerIn: parent
        }
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

       Rectangle {
        color: "transparent"
        height: 40
        Layout.preferredWidth: pitchText.width + 40
        Loader {
          id: pitchPlot
          active: gzPlotEnabled
          sourceComponent: gzPlotIcon
          property var gzLoaderMimeData: gzPlotMimeDataPitch
        }

        Text {
          id: pitchText
          text: 'Pitch (rad)'
          leftPadding: 5
          color: Material.theme == Material.Light ? "#444444" : "#bbbbbb"
          font.pointSize: 12
          anchors.centerIn: parent
        }
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

      Rectangle {
        color: "transparent"
        height: 40
        Layout.preferredWidth: zText.width + 40
        Loader {
          id: zPlot
          active: gzPlotEnabled
          sourceComponent: gzPlotIcon
          property var gzLoaderMimeData: gzPlotMimeDataZ
        }

        Text {
          id: zText
          text: 'Z (m)'
          leftPadding: 5
          color: Material.theme == Material.Light ? "#444444" : "#bbbbbb"
          font.pointSize: 12
          anchors.centerIn: parent
        }
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

      Rectangle {
        color: "transparent"
        height: 40
        Layout.preferredWidth: yawText.width + 40
        Loader {
          id: yawPlot
          active: gzPlotEnabled
          sourceComponent: gzPlotIcon
          property var gzLoaderMimeData: gzPlotMimeDataYaw
        }

        Text {
          id: yawText
          text: 'Yaw (rad)'
          leftPadding: 5
          color: Material.theme == Material.Light ? "#444444" : "#bbbbbb"
          font.pointSize: 12
          anchors.centerIn: parent
        }
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
