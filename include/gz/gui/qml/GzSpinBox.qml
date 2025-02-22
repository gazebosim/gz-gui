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
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQml 2.15


Item {
  id: root
  signal editingFinished
  property real minimumValue: 0
  property real maximumValue: 100
  property real value: 0.0
  property real stepSize: 1.0
  property int decimals: 0

  onValueChanged: {
    if (!spinBox.__modifying)
    {
      spinBox.value = decimalToInt(root.value)
    }
  }

  implicitHeight: spinBox.implicitHeight

  readonly property int kMaxInt: Math.pow(2, 31) - 1

  function decimalToInt(decimal) {
    var result = decimal * spinBox.decimalFactor
    if (result >= kMaxInt) {
      return kMaxInt
    }
    else if (result <= -kMaxInt) {
      return -kMaxInt
    }
    return result
  }

  function intToDecimal(intVal) {
    return intVal / spinBox.decimalFactor
  }

  // Used to update root.value without breaking existing bindings
  // TODO(azeey) When migrating to Qt6, we might need to set
  // the restoreMode property
  Binding on value {
    id: valueUpdater
    target: root
    property: 'value'
    value: intToDecimal(spinBox.value)
    when: false
    restoreMode: Binding.RestoreBinding
  }

  SpinBox {
      id: spinBox
      // This property is used to guard against binding loops. When
      // spinBox.value is modified through user input, (e.g. clicking on the
      // up arrow), we set the root.value. Without this guard condition, the
      // ValueChanged event of root.value would fire which would then try to
      // set spinBox.value, i.e a binding loop.
      property bool __modifying: false

      anchors.fill : parent
      bottomPadding: 0
      topPadding: 0
      leftPadding: 5
      implicitHeight: 40
      clip: true
      value: 0

      // Keep the decimal representation of value so that we
      // can decimalFactor changes properly. This is different from
      // root.value because this does not have any bindings and thus
      // cannot be modified from outside of this SpinBox element.
      property real __valueAsDecimal: 0.0
      onValueChanged: {
        __valueAsDecimal = intToDecimal(value)
      }
      // Note that this is a different event than ValueChanged. The
      // ValueModified event only fires when the value is edited by user
      // input from the GUI.
      onValueModified: {
        __modifying = true
        // Set the "value" property without breaking/changing its bindings.
        // This is done by temporarily enabling the binding (valueUpdater),
        // emitting the editingFinished signal so users GzSpinBox can be
        // notified and take the new value of the spinbox, and finally
        // disabling valueUpdater to restore the original bindings.
        valueUpdater.when = true
        root.editingFinished()
        valueUpdater.when = false
        __modifying = false
      }

      from: decimalToInt(minimumValue)
      to: decimalToInt(root.maximumValue)

      stepSize: decimalToInt(root.stepSize)
      editable: true
      anchors.centerIn: parent

      readonly property real decimalFactor: Math.pow(10, root.decimals)
      onDecimalFactorChanged: {
        value = decimalToInt(__valueAsDecimal)
      }

      contentItem: TextInput {
        font.pointSize: 10
        text: spinBox.textFromValue(spinBox.value, spinBox.locale)
        horizontalAlignment: Qt.AlignRight
        verticalAlignment: Qt.AlignVCenter
        readOnly: !spinBox.editable
        validator: spinBox.validator
        inputMethodHints: Qt.ImhFormattedNumbersOnly
        selectByMouse: true
        clip: true
      }

       up.indicator: Rectangle {
        x: spinBox.mirrored ? 0 : parent.width - width
        implicitWidth: 20
        implicitHeight: 20
        color: "transparent"

        Text {
            text: "⏶"
            opacity: spinBox.up.pressed ? 1: 0.6
            anchors.fill: parent
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignBottom
        }
      }
       down.indicator: Rectangle {
        x: spinBox.mirrored ? 0 : parent.width - width
        y: 20
        implicitWidth: 20
        implicitHeight: 20
        color: "transparent"

        Text {
            text: "⏷"
            opacity: spinBox.down.pressed ? 1: 0.6
            anchors.fill: parent
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignTop
        }
      }

      validator: DoubleValidator {
          bottom: Math.min(spinBox.from, spinBox.to)
          top:  Math.max(spinBox.from, spinBox.to)
          decimals: root.decimals
          notation: DoubleValidator.StandardNotation
      }

      textFromValue: function(value, locale) {
          return intToDecimal(value).toLocaleString(locale, 'f', parent.decimals)
      }

      valueFromText: function(text, locale) {
          return Math.round(decimalToInt(Number.fromLocaleString(locale, text)))
      }

      background: Rectangle {
        implicitWidth: 40
        implicitHeight: parent.implicitHeight
        border.color: "gray"
      }
    }
  }
