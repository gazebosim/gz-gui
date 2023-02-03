/*
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
  Layout.minimumWidth: 200
  Layout.minimumHeight: 200
  Layout.margins: 2
  anchors.fill: parent
  focus: true

  // X camera follow distance
  property double xFollowOffset: -5.0
  // Y camera follow distance
  property double yFollowOffset: 0.0
  // Z camera follow distance
  property double zFollowOffset: 3.0
  // P Gain camera follow distance
  property double pGainFollow: 0.01

  GridLayout {
    Layout.fillWidth: true
    Layout.margins: 2
    columns: 2
  
    // X follow offset distance
    Label {
      id: xFollowOffsetLabel
      text: "Camera follow X (m)"
      color: "dimgrey"
    }
    GzSpinBox {
      id: xFollowOffsetField
      Layout.fillWidth: true
      value: xFollowOffset
      maximumValue: 10.0
      minimumValue: -10.0
      decimals: 2
      stepSize: 0.5
      onEditingFinished:{
        xFollowOffset = value
        FollowConfig.SetFollowOffset(xFollowOffset, yFollowOffset, zFollowOffset)
      }
    }
    // Y follow offset distance
    Label {
      id: yFollowOffsetLabel
      text: "Camera follow Y (m)"
      color: "dimgrey"
    }
    GzSpinBox {
      id: yFollowOffsetField
      Layout.fillWidth: true
      value: yFollowOffset
      maximumValue: 10.0
      minimumValue: -10.0
      decimals: 2
      stepSize: 0.5
      onEditingFinished:{
        yFollowOffset = value
        FollowConfig.SetFollowOffset(xFollowOffset, yFollowOffset, zFollowOffset)
      }
    }
    // Z follow offset distance
    Label {
      id: zFollowOffsetLabel
      text: "Camera follow Z (m)"
      color: "dimgrey"
    }
    GzSpinBox {
      id: zFollowOffsetField
      Layout.fillWidth: true
      value: zFollowOffset
      maximumValue: 10.0
      minimumValue: -10.0
      decimals: 2
      stepSize: 0.5
      onEditingFinished:{
        zFollowOffset = value
        FollowConfig.SetFollowOffset(xFollowOffset, yFollowOffset, zFollowOffset)
      }
    }
    // P Gain follow
    Label {
      id: pGainFollowLabel
      text: "Camera follow P Gain"
      color: "dimgrey"
    }
    GzSpinBox {
      id: pGainFollowField
      Layout.fillWidth: true
      value: pGainFollow
      maximumValue: 1.0
      minimumValue: 0.001
      decimals: 3
      stepSize: 0.01
      onEditingFinished:{
        pGainFollow = value
        FollowConfig.SetFollowPGain(pGainFollow)
      }
    }
  }
}
