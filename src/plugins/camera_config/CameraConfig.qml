/*
 * Copyright (C) 2023 Open Source Robotics Foundation
 * Copyright (C) 2023 Rudis Laboratories LLC
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
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.3
import gz.gui 1.0

ColumnLayout {
  Layout.minimumWidth: 400
  Layout.minimumHeight: 650
  Layout.margins: 5
  anchors.fill: parent
  focus: true

  // Target to follow
  property string target: ""

  // // X camera follow distance
  // property double xFollowOffset: -5.0
  //
  // // Y camera follow distance
  // property double yFollowOffset: 0.0
  //
  // // Z camera follow distance
  // property double zFollowOffset: 3.0
  //
  // // P Gain camera follow distance
  // property double pGainFollowOffset: 0.01

  // Target
  Label {
    id: targetLabel
    text: "Target to follow"
    Layout.fillWidth: true
    Layout.margins: 10
  }
  TextField {
    id: targetField
    Layout.fillWidth: true
    Layout.margins: 10
    text: CameraConfig.followTargetName
    placeholderText: qsTr("Target to Follow...")
    onEditingFinished: {
      target = text
      CameraConfig.SetFollowTargetName(target)
    }
  }

  // GridLayout {
  //   Layout.fillWidth: true
  //   Layout.margins: 10
  //   columns: 2
  //
  //   // X follow offset distance
  //   Label {
  //     id: xFollowOffsetLabel
  //     text: "Camera follow X (m)"
  //     color: "dimgrey"
  //   }
  //   GzSpinBox {
  //     id: xFollowOffsetField
  //     Layout.fillWidth: true
  //     value: xFollowOffset
  //     maximumValue: 10.0
  //     minimumValue: -10.0
  //     decimals: 2
  //     stepSize: 0.5
  //     onEditingFinished:{
  //       xFollowOffset = value
  //       CameraConfig.CameraFollowSetting(xFollowOffset, yFollowOffset, zFollowOffset, pGainFollowOffset, target)
  //     }
  //   }
  //   // Y follow offset distance
  //   Label {
  //     id: yFollowOffsetLabel
  //     text: "Camera follow Y (m)"
  //     color: "dimgrey"
  //   }
  //   GzSpinBox {
  //     id: yFollowOffsetField
  //     Layout.fillWidth: true
  //     value: yFollowOffset
  //     maximumValue: 10.0
  //     minimumValue: -10.0
  //     decimals: 2
  //     stepSize: 0.5
  //     onEditingFinished:{
  //       yFollowOffset = value
  //       CameraConfig.CameraFollowSetting(xFollowOffset, yFollowOffset, zFollowOffset, pGainFollowOffset, target)
  //     }
  //   }
  //   // Z follow offset distance
  //   Label {
  //     id: zFollowOffsetLabel
  //     text: "Camera follow Z (m)"
  //     color: "dimgrey"
  //   }
  //   GzSpinBox {
  //     id: zFollowOffsetField
  //     Layout.fillWidth: true
  //     value: zFollowOffset
  //     maximumValue: 10.0
  //     minimumValue: -10.0
  //     decimals: 2
  //     stepSize: 0.5
  //     onEditingFinished:{
  //       zFollowOffset = value
  //       CameraConfig.CameraFollowSetting(xFollowOffset, yFollowOffset, zFollowOffset, pGainFollowOffset, target)
  //     }
  //   }
  //   // P Gain follow
  //   Label {
  //     id: pGainFollowOffsetLabel
  //     text: "Camera follow P Gain"
  //     color: "dimgrey"
  //   }
  //   GzSpinBox {
  //     id: pGainFollowOffsetField
  //     Layout.fillWidth: true
  //     value: pGainFollowOffset
  //     maximumValue: 1.0
  //     minimumValue: 0.001
  //     decimals: 3
  //     stepSize: 0.01
  //     onEditingFinished:{
  //       pGainFollowOffset = value
  //       CameraConfig.CameraFollowSetting(xFollowOffset, yFollowOffset, zFollowOffset, pGainFollowOffset, target)
  //     }
  //   }
  // }
}
