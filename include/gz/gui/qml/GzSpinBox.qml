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
import QtQuick.Controls 2.15

Control {
  id: root
  signal editingFinished(real _value)
  property alias minimumValue: spinBox.minimumValue
  property alias maximumValue: spinBox.maximumValue
  property alias value: spinBox.value
  property real stepSize: 1.0
  property alias decimals: spinBox.decimals
  anchors.fill : parent


  function decimalToInt(decimal) {
    return decimal * spinBox.decimalFactor
  }
  SpinBox {
      id: spinBox
      value: decimalToInt(1.1)
  anchors.fill : parent

      property real minimumValue: 0
      property real maximumValue: 100

      from: decimalToInt(minimumValue)
      to: decimalToInt(maximumValue)

      stepSize: decimalToInt(root.stepSize)
      editable: true
      anchors.centerIn: parent

      signal editingFinished(real _value)

      property int decimals: 2
      property real realValue: value / decimalFactor
      readonly property int decimalFactor: Math.pow(10, decimals)

      validator: DoubleValidator {
          bottom: Math.min(spinBox.from, spinBox.to)
          top:  Math.max(spinBox.from, spinBox.to)
          decimals: spinBox.decimals
          notation: DoubleValidator.StandardNotation
      }

      textFromValue: function(value, locale) {
          return Number(value / decimalFactor).toLocaleString(locale, 'f', spinBox.decimals)
      }

      valueFromText: function(text, locale) {
          return Math.round(Number.fromLocaleString(locale, text) * decimalFactor)
      }
      // onValueChanged: function(_value) {
      //   editingFinished(_value)
      // }
  }
}
