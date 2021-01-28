/*
 * Copyright (C) 2021 Open Source Robotics Foundation
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
import QtQuick.Controls.Material 2.2
import QtQuick.Controls.Material.impl 2.2
import QtQuick.Layouts 1.3
import QtQuick.Controls.Styles 1.4
import QtQuick.Dialogs 1.0

ToolBar {
  Layout.minimumWidth: 200
  Layout.minimumHeight: 150

  background: Rectangle {
    color: "transparent"
  }

  RowLayout {
    spacing: 2

    ToolButton {
      id: screenshot
      ToolTip.text: "Take a screenshot"
      ToolTip.visible: hovered
      ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
      contentItem: Image {
        fillMode: Image.Pad
        horizontalAlignment: Image.AlignHCenter
        verticalAlignment: Image.AlignVCenter
        source: "screenshot.png"
        sourceSize.width: 30
        sourceSize.height: 30
      }
      onClicked: {
        Screenshot.OnScreenshot()
        savedPathPopup.open()
      }
    }

    ToolButton {
      id: directory
      ToolTip.text: "Change directory\nCurrent: " + ScreenshotDirectory
      ToolTip.visible: hovered
      ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
      contentItem: Image {
        fillMode: Image.Pad
        horizontalAlignment: Image.AlignHCenter
        verticalAlignment: Image.AlignVCenter
        source: "folder.png"
        sourceSize.width: 40
        sourceSize.height: 40
      }
      onClicked: {
        fileDialog.open()
      }
    }

    FileDialog {
      id: fileDialog
      title: "Save screenshots"
      folder: ScreenshotDirectory
      selectExisting: false
      selectFolder: true
      onAccepted: {
        Screenshot.OnChangeDirectory(fileDialog.folder)
        close()
      }
    }

    Popup {
      id: savedPathPopup
      x: 0
      y: 50
      Text {
        text: "Screenshot saved in: " + ScreenshotDirectory
      }
    }

  }
}
