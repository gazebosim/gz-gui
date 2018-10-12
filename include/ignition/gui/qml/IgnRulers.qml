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
    height: parent.height
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
          var newCardX = Math.max(target.x + mouseX, 0)
          var newCardWidth = Math.max(target.width + (target.x - newCardX),
                                      rulersRect.minSize)
          if (newCardWidth === target.width)
            return;
          target.x = newCardX
          target.width = newCardWidth
        }
      }
    }
  }

  // Right ruler
  Rectangle {
    width: rulersThickness
    height: parent.height
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
          target.width = Math.max(target.width + mouseX, rulersRect.minSize)

          if (target.width + target.x > target.parent.width)
            target.width = target.parent.width - target.x
        }
      }
    }
  }

  // Top ruler
  Rectangle {
    width: parent.width
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
          var newCardY = Math.max(target.y + mouseY, 0)
          var newCardHeight = Math.max(target.height + (target.y - newCardY),
                                       rulersRect.minSize)

          if (newCardHeight === target.height)
            return;

          target.y = newCardY
          target.height = newCardHeight
        }
      }
    }
  }

  // Bottom ruler
  Rectangle {
    width: parent.width
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
          target.height = Math.max(target.height + mouseY, rulersRect.minSize)

          if (target.height + target.y > target.parent.height)
            target.height = target.parent.height - target.y
        }
      }
    }
  }
}
