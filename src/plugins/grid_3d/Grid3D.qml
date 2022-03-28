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
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1
import QtQuick.Dialogs 1.0
import QtQuick.Layouts 1.3
import "qrc:/qml"

GridLayout {
  columns: 4
  columnSpacing: 10
  Layout.minimumWidth: 300
  Layout.minimumHeight: 625
  anchors.fill: parent
  anchors.leftMargin: 10
  anchors.rightMargin: 10

  // Get number of decimal digits based on a widget's width
  // TODO(chapulina) Move this to a common place so all widgets can use it
  function getDecimals(_width) {
    if (_width <= 80)
      return 2;
    else if (_width <= 100)
      return 4;
    return 6;
  }

  Connections {
    target: Grid3D
    onNewParams: {
      horizontalCellCount.value = _hCellCount;
      verticalCellCount.value = _vCellCount;
      cellLength.value = _cellLength;
      x.value = _pos.x;
      y.value = _pos.y;
      z.value = _pos.z;
      roll.value = _rot.x;
      pitch.value = _rot.y;
      yaw.value = _rot.z;
      r.value = _color.r;
      g.value = _color.g;
      b.value = _color.b;
      a.value = _color.a;
    }
  }

  ComboBox {
    id: combo
    Layout.columnSpan: 2
    Layout.fillWidth: true
    model: Grid3D.nameList
    onCurrentIndexChanged: {
      if (currentIndex < 0)
        return;

      Grid3D.OnName(textAt(currentIndex));
    }
    popup.width: parent.width
    ToolTip.visible: hovered
    ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
    ToolTip.text: qsTr("Grids in the scene")
  }

  RoundButton {
    text: "\u21bb"
    Layout.columnSpan: 1
    Material.background: Material.primary
    onClicked: {
      Grid3D.OnRefresh();
    }
    ToolTip.visible: hovered
    ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
    ToolTip.text: qsTr("Refresh list of grids")
  }

  CheckBox {
    Layout.alignment: Qt.AlignHCenter
    id: showgrid
    Layout.columnSpan: 1
    text: qsTr("Show")
    checked: true
    onClicked: {
      Grid3D.OnShow(checked)
    }
  }

  Text {
    Layout.columnSpan: 4
    text: "Cell Count"
    color: "dimgrey"
    font.bold: true
  }

  Text {
    Layout.columnSpan: 2
    id: vercelltext
    color: "dimgrey"
    text: "Vertical"
  }

  IgnSpinBox {
    Layout.columnSpan: 2
    Layout.fillWidth: true
    id: verticalCellCount
    maximumValue: Number.MAX_VALUE
    minimumValue: 0
    value: 0
    onEditingFinished: Grid3D.UpdateVCellCount(verticalCellCount.value)
  }

  Text {
    Layout.columnSpan: 2
    id: honcelltext
    color: "dimgrey"
    text: "Horizontal"
  }

  IgnSpinBox {
    Layout.columnSpan: 2
    Layout.fillWidth: true
    id: horizontalCellCount
    maximumValue: Number.MAX_VALUE
    minimumValue: 1
    value: 20
    onEditingFinished: Grid3D.UpdateHCellCount(horizontalCellCount.value)
  }

  Text {
    Layout.columnSpan: 4
    id: celllengthtext
    text: "Cell Length"
    color: "dimgrey"
    font.bold: true
  }

  Text {
    Layout.columnSpan: 2
    id: length
    color: "dimgrey"
    text: "Length (m)"
  }
  IgnSpinBox {
    Layout.columnSpan: 2
    Layout.fillWidth: true
    id: cellLength
    maximumValue: Number.MAX_VALUE
    minimumValue: 0.0000001
    value: 1.00
    decimals: getDecimals(cellLength.width)
    stepSize: 0.01
    onEditingFinished: Grid3D.UpdateCellLength(cellLength.value)
  }

  Text {
    Layout.columnSpan: 2
    id: cartesian
    color: "dimgrey"
    font.bold: true
    text: "Position (m)"
  }

  Text {
    Layout.columnSpan: 2
    id: principal
    text: "Rotation (rad)"
    color: "dimgrey"
    font.bold: true
  }

  Text {
    text: "X"
    color: "dimgrey"
  }

  IgnSpinBox {
    Layout.fillWidth: true
    id: x
    value: 0.00
    maximumValue: Number.MAX_VALUE
    minimumValue: -Number.MAX_VALUE
    decimals: getDecimals(x.width)
    stepSize: 0.01
    onEditingFinished: Grid3D.SetPose(x.value, y.value, z.value, roll.value, pitch.value, yaw.value)
  }

  Text {
    text: "Roll"
    color: "dimgrey"
  }

  IgnSpinBox {
    Layout.fillWidth: true
    id: roll
    maximumValue: 6.28
    minimumValue: 0.00
    value: 0.00
    decimals: getDecimals(roll.width)
    stepSize: 0.01
    onEditingFinished: Grid3D.SetPose(x.value, y.value, z.value, roll.value, pitch.value, yaw.value)
  }

  Text {
    text: "Y"
    color: "dimgrey"
  }

  IgnSpinBox {
    Layout.fillWidth: true
    id: y
    value: 0.00
    maximumValue: Number.MAX_VALUE
    minimumValue: -Number.MAX_VALUE
    decimals: getDecimals(y.width)
    stepSize: 0.01
    onEditingFinished: Grid3D.SetPose(x.value, y.value, z.value, roll.value, pitch.value, yaw.value)
  }

  Text {
    text: "Pitch"
    color: "dimgrey"
  }

  IgnSpinBox {
    Layout.fillWidth: true
    id: pitch
    maximumValue: 6.28
    minimumValue: 0.00
    value: 0.00
    decimals: getDecimals(pitch.width)
    stepSize: 0.01
    onEditingFinished: Grid3D.SetPose(x.value, y.value, z.value, roll.value, pitch.value, yaw.value)
  }

  Text {
    text: "Z"
    color: "dimgrey"
  }

  IgnSpinBox {
    Layout.fillWidth: true
    id: z
    value: 0.00
    maximumValue: Number.MAX_VALUE
    minimumValue: -Number.MAX_VALUE
    decimals: getDecimals(z.width)
    stepSize: 0.01
    onEditingFinished: Grid3D.SetPose(x.value, y.value, z.value, roll.value, pitch.value, yaw.value)
  }

  Text {
    text: "Yaw"
    color: "dimgrey"
  }

  IgnSpinBox {
    Layout.fillWidth: true
    id: yaw
    maximumValue: 6.28
    minimumValue: 0.00
    value: 0.00
    decimals: getDecimals(yaw.width)
    stepSize: 0.01
    onEditingFinished: Grid3D.SetPose(x.value, y.value, z.value, roll.value, pitch.value, yaw.value)
  }

  Text {
    Layout.columnSpan: 4
    text: "Color"
    color: "dimgrey"
    font.bold: true
  }

  Text {
    text: "R"
    color: "dimgrey"
  }

  IgnSpinBox {
    Layout.fillWidth: true
    id: r
    maximumValue: 1.00
    minimumValue: 0.00
    value: 0.7
    stepSize: 0.01
    decimals: getDecimals(r.width)
    onEditingFinished: Grid3D.SetColor(r.value, g.value, b.value, a.value)
  }

  Text {
    text: "G"
    color: "dimgrey"
  }

  IgnSpinBox {
    Layout.fillWidth: true
    id: g
    maximumValue: 1.00
    minimumValue: 0.00
    value: 0.7
    stepSize: 0.01
    decimals: getDecimals(g.width)
    onEditingFinished: Grid3D.SetColor(r.value, g.value, b.value, a.value)
  }

  Text {
    text: "B"
    color: "dimgrey"
  }

  IgnSpinBox {
    Layout.fillWidth: true
    id: b
    maximumValue: 1.00
    minimumValue: 0.00
    value: 0.7
    stepSize: 0.01
    decimals: getDecimals(b.width)
    onEditingFinished: Grid3D.SetColor(r.value, g.value, b.value, a.value)
  }

  Text {
    text: "A"
    color: "dimgrey"
  }

  IgnSpinBox {
    Layout.fillWidth: true
    id: a
    maximumValue: 1.00
    minimumValue: 0.00
    value: 1.0
    stepSize: 0.01
    decimals: getDecimals(a.width)
    onEditingFinished: Grid3D.SetColor(r.value, g.value, b.value, a.value)
  }

  Button {
    Layout.alignment: Qt.AlignHCenter
    Layout.columnSpan: 4
    id: color
    text: qsTr("Custom Color")
    onClicked: colorDialog.open()

    ColorDialog {
      id: colorDialog
      title: "Choose a grid color"
      visible: false
      onAccepted: {
        r.value = colorDialog.color.r
        g.value = colorDialog.color.g
        b.value = colorDialog.color.b
        a.value = colorDialog.color.a
        Grid3D.SetColor(colorDialog.color.r, colorDialog.color.g, colorDialog.color.b, colorDialog.color.a)
        colorDialog.close()
      }
      onRejected: {
        colorDialog.close()
      }
    }
  }

  // Bottom spacer
  Item {
    Layout.columnSpan: 4
    Layout.fillHeight: true
  }
}

