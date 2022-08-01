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

/**
 * Item for a draggable plotting icon
 */

Image {
  id: gzPlotIcon
  property string gzComponentInfo
  property var gzMimeData
  width: 20
  height: 20
  y: 10
  source: "images/plottable_icon.svg"
  anchors.top: parent.top
  anchors.left: parent.left

  Drag.mimeData: gzMimeData
  Drag.dragType: Drag.Automatic
  Drag.supportedActions : Qt.CopyAction
  Drag.active: gzDragMouse.drag.active
  // a point to drag from
  Drag.hotSpot.x: 0
  Drag.hotSpot.y: y
  MouseArea {
    id: gzDragMouse
    anchors.fill: parent
    drag.target: parent
    onPressed: {
      parent.grabToImage(function(result) {parent.Drag.imageSource = result.url })
    }
    onReleased: parent.Drag.drop();
    cursorShape: Qt.DragCopyCursor
  }
}
