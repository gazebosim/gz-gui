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
import QtQuick.Controls 2.2

/**
 * Rulers which can be dragged to resize a target.
 */
Rectangle {
  id: rulersRect
  color: "transparent"

  /**
   * Thickness of rulers
   */
  property int rulersThickness: 25

  /**
   * Set to false so rulers disappear.
   */
  property bool enabled: true

  /**
   * Minimum length of each dimension
   */
  property int minSize: 50

  /**
   * Target item to be resized by the rulers.
   */
  property var target: null

  // Left ruler
  Rectangle {
    width: rulersThickness
    height: parent.height - 20
    visible: rulersRect.enabled
    color: "transparent"
    anchors.horizontalCenter: parent.left
    anchors.verticalCenter: parent.verticalCenter

    MouseArea {
      anchors.fill: parent
      cursorShape: Qt.SplitHCursor
      drag { target: parent; axis: Drag.XAxis }
      onMouseXChanged: {
        if (drag.active)
        {
          resizeLeft(target, mouseX);
        }
      }
    }
  }

  // Right ruler
  Rectangle {
    width: rulersThickness
    height: parent.height - 20
    visible: rulersRect.enabled
    color: "transparent"
    anchors.horizontalCenter: parent.right
    anchors.verticalCenter: parent.verticalCenter

    MouseArea {
      anchors.fill: parent
      cursorShape: Qt.SplitHCursor
      drag { target: parent; axis: Drag.XAxis }
      onMouseXChanged: {
        if (drag.active)
        {
          resizeRight(target, mouseX);
        }
      }
    }
  }

  // Top ruler
  Rectangle {
    width: parent.width - 20
    height: rulersThickness
    visible: rulersRect.enabled
    color: "transparent"
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.verticalCenter: parent.top

    MouseArea {
      anchors.fill: parent
      cursorShape: Qt.SplitVCursor
      drag { target: parent; axis: Drag.YAxis }
      onMouseYChanged: {
        if (drag.active)
        {
          resizeTop(target, mouseY);
        }
      }
    }
  }

  // Bottom ruler
  Rectangle {
    width: parent.width - 20
    height: rulersThickness
    visible: rulersRect.enabled
    color: "transparent"
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.verticalCenter: parent.bottom

    MouseArea {
      anchors.fill: parent
      cursorShape: Qt.SplitVCursor
      drag { target: parent; axis: Drag.YAxis }
      onMouseYChanged: {
        if (drag.active)
        {
          resizeBottom(target, mouseY);
        }
      }
    }
  }

  // Top-Left Ruler
  Rectangle {
    width: 25
    height: 25
    visible: rulersRect.enabled
    color: "transparent"
    anchors.horizontalCenter: parent.left
    anchors.verticalCenter: parent.top

    MouseArea {
      anchors.fill: parent
      cursorShape: Qt.SizeFDiagCursor
      drag { target: parent; axis: Drag.XAndYAxis }
      onMouseYChanged: {
        if (drag.active)
        {
          resizeTop(target, mouseY);
          resizeLeft(target, mouseX);
        }
      }
    }
  }

  // Top-Right Ruler
  Rectangle {
    width: 25
    height: 25
    visible: rulersRect.enabled
    color: "transparent"
    anchors.horizontalCenter: parent.right
    anchors.verticalCenter: parent.top

    MouseArea {
      anchors.fill: parent
      cursorShape: Qt.SizeBDiagCursor
      drag { target: parent; axis: Drag.XAndYAxis }
      onMouseYChanged: {
        if (drag.active)
        {
          resizeTop(target, mouseY);
          resizeRight(target, mouseX);
        }
      }
    }
  }

  // Bottom-Left Ruler
  Rectangle {
    width: 25
    height: 25
    visible: rulersRect.enabled
    color: "transparent"
    anchors.horizontalCenter: parent.left
    anchors.verticalCenter: parent.bottom

    MouseArea {
      anchors.fill: parent
      cursorShape: Qt.SizeBDiagCursor
      drag { target: parent; axis: Drag.XAndYAxis }
      onMouseYChanged: {
        if (drag.active)
        {
          resizeBottom(target, mouseY);
          resizeLeft(target, mouseX);
        }
      }
    }
  }

  // Bottom-Right Ruler
  Rectangle {
    width: 25
    height: 25
    visible: rulersRect.enabled
    color: "transparent"
    anchors.horizontalCenter: parent.right
    anchors.verticalCenter: parent.bottom

    MouseArea {
      anchors.fill: parent
      cursorShape: Qt.SizeFDiagCursor
      drag { target: parent; axis: Drag.XAndYAxis }
      onMouseYChanged: {
        if (drag.active)
        {
          resizeBottom(target, mouseY);
          resizeRight(target, mouseX);
        }
      }
    }
  }

  function resizeLeft(target, mouseX)
  {
    var newCardX = Math.max(target.x + mouseX, 0)
    var newCardWidth = Math.max(target.width + (target.x - newCardX),
                                rulersRect.minSize, target.cardMinimumWidth)

    if (newCardWidth === target.width)
      return;

    target.x = newCardX
    target.width = newCardWidth
  }

  function resizeRight(target, mouseX)
  {
    target.width = Math.max(target.width + mouseX, rulersRect.minSize,
                            target.cardMinimumWidth)

    if (target.width + target.x > target.parent.width)
      target.width = target.parent.width - target.x
  }

  function resizeTop(target, mouseY)
  {
    var newCardY = Math.max(target.y + mouseY, 0)
    var newCardHeight = Math.max(target.height + (target.y - newCardY),
                                 rulersRect.minSize, target.cardMinimumHeight)

    if (newCardHeight === target.height)
      return;

    target.y = newCardY
    target.height = newCardHeight
  }

  function resizeBottom(target, mouseY)
  {
    target.height = Math.max(target.height + mouseY, rulersRect.minSize,
                             target.cardMinimumHeight)

    if (target.height + target.y > target.parent.height)
      target.height = target.parent.height - target.y
  }
}
