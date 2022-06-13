/*
 * Copyright (C) 2020 Open Source Robotics Foundation
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
    target: GridConfig
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
      gzcolor.r = _color.r * 255;
      gzcolor.g = _color.g * 255;
      gzcolor.b = _color.b * 255;
      gzcolor.a = _color.a;
    }
  }

  ComboBox {
    id: combo
    Layout.columnSpan: 2
    Layout.fillWidth: true
    model: GridConfig.nameList
    onCurrentIndexChanged: {
      if (currentIndex < 0)
        return;

      GridConfig.OnName(textAt(currentIndex));
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
      GridConfig.OnRefresh();
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
      GridConfig.OnShow(checked)
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
    onEditingFinished: GridConfig.UpdateVCellCount(verticalCellCount.value)
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
    onEditingFinished: GridConfig.UpdateHCellCount(horizontalCellCount.value)
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
    onEditingFinished: GridConfig.UpdateCellLength(cellLength.value)
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
    onEditingFinished: GridConfig.SetPose(x.value, y.value, z.value, roll.value, pitch.value, yaw.value)
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
    onEditingFinished: GridConfig.SetPose(x.value, y.value, z.value, roll.value, pitch.value, yaw.value)
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
    onEditingFinished: GridConfig.SetPose(x.value, y.value, z.value, roll.value, pitch.value, yaw.value)
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
    onEditingFinished: GridConfig.SetPose(x.value, y.value, z.value, roll.value, pitch.value, yaw.value)
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
    onEditingFinished: GridConfig.SetPose(x.value, y.value, z.value, roll.value, pitch.value, yaw.value)
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
    onEditingFinished: GridConfig.SetPose(x.value, y.value, z.value, roll.value, pitch.value, yaw.value)
  }

  Text {
    Layout.columnSpan: 4
    text: "Color"
    color: "dimgrey"
    font.bold: true
  }

  Text {
    Layout.columnSpan: 2
    color: "dimgrey"
    text: "Grid Color"
  }

  GzColor { 
    id: gzcolor
    Layout.columnSpan: 2
    Layout.alignment: Qt.AlignRight
    Layout.bottomMargin: 5
    Layout.rightMargin: 20
    onColorSet: GridConfig.SetColor(1.0 * r / 255.0, 1.0 * g / 255.0, 1.0 * b / 255.0, a)
  }

}

