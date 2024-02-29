/*
 * Copyright (C) 2023 Open Source Robotics Foundation
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
import QtQuick.Layouts 1.3

Rectangle {
  id: cameraFps
  color: "transparent"
  Layout.minimumWidth: 150
  Layout.minimumHeight: 80

  RowLayout {
    id: cameraFpsLayout
    anchors.fill: parent
    anchors.margins: 10

    Label {
      ToolTip.text: qsTr("Camera FPS")
      font.weight: Font.DemiBold
      text: "FPS"
    }

    Label {
      objectName: "cameraFps"
      text: CameraFps.cameraFPSValue
      Layout.alignment: Qt.AlignRight
    }
  }
}
