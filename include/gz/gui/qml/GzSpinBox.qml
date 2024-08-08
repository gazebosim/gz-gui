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
import QtQuick
import QtQuick.Controls

Item {
  id: root
  property int decimals: 2
  property real value: 0.0
  property real from: 0.0
  property real to: 100.0
  property alias minimumValue: root.from
  property alias maximumValue: root.to
  property real stepSize: 1.0
  signal editingFinished(real _value)
  implicitWidth: spinbox.implicitWidth
  implicitHeight: spinbox.implicitHeight
  // Timer {
  //     interval: 50; running: true; repeat: true
  //     onTriggered: {
  //       console.log("root. w: ", root.parent.objectName, spinbox.objectName, " = ", root.width, root.height, root.implicitWidth, root.implicitHeight)
  //     }
  // }


  function decimalToInt(decimal) {
    return Math.round(decimal * spinbox.factor)
  }

  readonly property int maxInt32: Math.pow( 2, 31 ) - 1;
  clip: true

  SpinBox{
    id: spinbox
    anchors.fill: parent
    property real factor: Math.pow(10, root.decimals)
    stepSize: decimalToInt(root.stepSize)
    value: decimalToInt(root.value)
    to : Math.min(decimalToInt(root.to), maxInt32)
    from : Math.max(decimalToInt(root.from), -maxInt32)
    editable: true
    clip: true
    topPadding: 0
    bottomPadding: 0

    validator: DoubleValidator {
      bottom: Math.min(spinbox.from, spinbox.to)
      top:  Math.max(spinbox.from, spinbox.to)
    }

    textFromValue: function(value, locale) {
      return Number(value / factor).toLocaleString(locale, 'f', root.decimals)
    }

    valueFromText: function(text, locale) {
      return decimalToInt(Number.fromLocaleString(locale, text))
    }

    onValueModified: {
      root.value = spinbox.value / spinbox.factor
      // console.log("val mod: ", root.value)
      root.editingFinished(root.value)
    }

    background: Rectangle {
      implicitWidth: 80
      implicitHeight: 80
      border.color: "red"
    }
  }
}
