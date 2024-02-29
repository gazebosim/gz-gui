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
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import RenderWindow

import Qt5Compat.GraphicalEffects

Rectangle {
  Layout.minimumWidth: 200
  Layout.minimumHeight: 200
  anchors.fill: parent

  /**
   * True to enable gamma correction
   */
  property bool gammaCorrect: false

  /**
   * Get mouse position on 3D widget
   */
  MouseArea {
    id: mouseArea
    anchors.fill: parent
    hoverEnabled: true
    acceptedButtons: Qt.NoButton
    visible: MinimalScene.loadingError.length == 0
    onEntered: {
      MinimalScene.OnFocusWindow()
    }
    onPositionChanged: {
      MinimalScene.OnHovered(mouseArea.mouseX, mouseArea.mouseY);
    }
  }

  RenderWindow {
    id: renderWindow
    objectName: "rw"
    anchors.fill: parent
    // visible: MinimalScene.loadingError.length == 0
  }

  /*
   * Gamma correction for sRGB output. Enabled when engine is set to ogre2
   */
  GammaAdjust {
      anchors.fill: renderWindow
      source: renderWindow
      gamma: 2.4
      enabled: gammaCorrect
      visible: gammaCorrect
  }

  onParentChanged: {
    if (undefined === parent)
      return;

      width = Qt.binding(function() {return parent.parent.width})
      height = Qt.binding(function() {return parent.parent.height})
  }

  DropArea {
  anchors.fill: renderWindow

  onDropped: {
    MinimalScene.OnDropped(drop.text, drag.x, drag.y)
  }

  Label {
    anchors.fill: parent
    anchors.margins: 10
    text: MinimalScene.loadingError
    visible: (MinimalScene.loadingError.length > 0);
    wrapMode: Text.WordWrap
  }
}
}
