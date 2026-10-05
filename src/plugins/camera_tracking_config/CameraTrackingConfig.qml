/*
 * Copyright (C) 2024 CogniPilot Foundation
 * Copyright (C) 2024 Rudis Laboratories LLC
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

  // X track camera pose
  property double xTrackPose: 0.0
  // Y track camera pose
  property double yTrackPose: 0.0
  // Z track camera pose
  property double zTrackPose: 0.0
  // P Gain track camera pose
  property double pGainTrack: 0.01
  // X camera follow distance
  property double xFollowOffset: -3.0
  // Y camera follow distance
  property double yFollowOffset: 0.0
  // Z camera follow distance
  property double zFollowOffset: 2.0
  // P Gain camera follow distance
  property double pGainFollow: 0.01

  GridLayout {
    Layout.fillWidth: true
    Layout.margins: 2
    columns: 2

    // X Track Offset
    Label {
      id: xTrackPoseLabel
      text: "Track Offset X (m)"
      color: "dimgrey"
    }
    GzSpinBox {
      id: xTrackPoseField
      Layout.fillWidth: true
      value: xTrackPose
      maximumValue: 1000.0
      minimumValue: -1000.0
      decimals: 2
      stepSize: 0.5
      onEditingFinished:{
        xTrackPose = value
        CameraTrackingConfig.SetTracking(xTrackPose, yTrackPose, zTrackPose, pGainTrack, xFollowOffset, yFollowOffset, zFollowOffset, pGainFollow)
      }
    }
    // Y Track Offset
    Label {
      id: yTrackPoseLabel
      text: "Track Offset Y (m)"
      color: "dimgrey"
    }
    GzSpinBox {
      id: yTrackPoseField
      Layout.fillWidth: true
      value: yTrackPose
      maximumValue: 1000.0
      minimumValue: -1000.0
      decimals: 2
      stepSize: 0.5
      onEditingFinished:{
        yTrackPose = value
        CameraTrackingConfig.SetTracking(xTrackPose, yTrackPose, zTrackPose, pGainTrack, xFollowOffset, yFollowOffset, zFollowOffset, pGainFollow)
      }
    }
    // Z Track Offset
    Label {
      id: zTrackPoseLabel
      text: "Track Offset Z (m)"
      color: "dimgrey"
    }
    GzSpinBox {
      id: zTrackPoseField
      Layout.fillWidth: true
      value: zTrackPose
      maximumValue: 1000.0
      minimumValue: -1000.0
      decimals: 2
      stepSize: 0.5
      onEditingFinished:{
        zTrackPose = value
        CameraTrackingConfig.SetTracking(xTrackPose, yTrackPose, zTrackPose, pGainTrack, xFollowOffset, yFollowOffset, zFollowOffset, pGainFollow)
      }
    }
    // P Gain track
    Label {
      id: pGainTrackLabel
      text: "Track P Gain"
      color: "dimgrey"
    }
    GzSpinBox {
      id: pGainTrackField
      Layout.fillWidth: true
      value: pGainTrack
      maximumValue: 1.0
      minimumValue: 0.001
      decimals: 3
      stepSize: 0.01
      onEditingFinished:{
        pGainTrack = value
        CameraTrackingConfig.SetTracking(xTrackPose, yTrackPose, zTrackPose, pGainTrack, xFollowOffset, yFollowOffset, zFollowOffset, pGainFollow)
      }
    }
    // X Follow Offset
    Label {
      id: xFollowOffsetLabel
      text: "Follow Offset X (m)"
      color: "dimgrey"
    }
    GzSpinBox {
      id: xFollowOffsetField
      Layout.fillWidth: true
      value: xFollowOffset
      maximumValue: 100.0
      minimumValue: -100.0
      decimals: 2
      stepSize: 0.5
      onEditingFinished:{
        xFollowOffset = value
        CameraTrackingConfig.SetTracking(xTrackPose, yTrackPose, zTrackPose, pGainTrack, xFollowOffset, yFollowOffset, zFollowOffset,pGainFollow)
      }
    }
    // Y Follow Offset
    Label {
      id: yFollowOffsetLabel
      text: "Follow Offset Y (m)"
      color: "dimgrey"
    }
    GzSpinBox {
      id: yFollowOffsetField
      Layout.fillWidth: true
      value: yFollowOffset
      maximumValue: 100.0
      minimumValue: -100.0
      decimals: 2
      stepSize: 0.5
      onEditingFinished:{
        yFollowOffset = value
        CameraTrackingConfig.SetTracking(xTrackPose, yTrackPose, zTrackPose, pGainTrack, xFollowOffset, yFollowOffset, zFollowOffset,pGainFollow)
      }
    }
    // Z Follow Offset
    Label {
      id: zFollowOffsetLabel
      text: "Follow Offset Z (m)"
      color: "dimgrey"
    }
    GzSpinBox {
      id: zFollowOffsetField
      Layout.fillWidth: true
      value: zFollowOffset
      maximumValue: 100.0
      minimumValue: -100.0
      decimals: 2
      stepSize: 0.5
      onEditingFinished:{
        zFollowOffset = value
        CameraTrackingConfig.SetTracking(xTrackPose, yTrackPose, zTrackPose, pGainTrack, xFollowOffset, yFollowOffset, zFollowOffset,pGainFollow)
      }
    }
    // P Gain follow
    Label {
      id: pGainFollowLabel
      text: "Follow Offset P Gain"
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
        CameraTrackingConfig.SetTracking(xTrackPose, yTrackPose, zTrackPose, pGainTrack, xFollowOffset, yFollowOffset, zFollowOffset,pGainFollow)
      }
    }
  }
}
