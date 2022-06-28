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
import QtQuick.Layouts 1.3

GridLayout {
  id: gzVectorRoot

  property alias xSpin: x_spin
  property alias ySpin: y_spin
  property alias zSpin: z_spin

  property string xName: ""
  property string yName: ""
  property string zName: ""

  property bool textVisible: true

  signal gzVectorSet()

  columns: 5
  columnSpacing: 10
  property double spinMax: 1000000

  IgnHelpers {
    id: gzHelper
  }

  Text {
    Layout.row: 0
    Layout.column: 1
    Layout.alignment: Qt.AlignCenter
    text: xName
    color: "dimgrey"
    visible: textVisible
  }

  IgnSpinBox {
    id: x_spin
    maximumValue: spinMax
    minimumValue: -spinMax
    decimals: gzHelper.getDecimals(x_spin.width)
    Layout.row: 1
    Layout.column: 1
    Layout.fillWidth: true
    onEditingFinished: gzVectorRoot.gzVectorSet()
  }

  Text {
    Layout.row: 0
    Layout.column: 2
    Layout.alignment: Qt.AlignCenter
    text: yName
    color: "dimgrey"
    visible: textVisible
  }

  IgnSpinBox {
    id: y_spin
    maximumValue: spinMax
    minimumValue: -spinMax
    stepSize: 1
    decimals: gzHelper.getDecimals(y_spin.width)
    Layout.row: 1
    Layout.column: 2
    Layout.fillWidth: true
    onEditingFinished: gzVectorRoot.gzVectorSet()
  }

  Text {
    Layout.row: 0
    Layout.column: 3
    Layout.alignment: Qt.AlignCenter
    text: zName
    color: "dimgrey"
    visible: textVisible
  }

  IgnSpinBox {
    id: z_spin
    maximumValue: spinMax
    minimumValue: -spinMax
    stepSize: 1
    decimals: gzHelper.getDecimals(z_spin.width)
    Layout.row: 1
    Layout.column: 3
    Layout.fillWidth: true
    onEditingFinished: gzVectorRoot.gzVectorSet()
  }
}
