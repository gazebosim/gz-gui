import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.1

Pane {
  id: cardPane
  Material.elevation: 6

  property int rulersThickness: 25
  property int minSize: 100

  MouseArea {
    anchors.fill: parent
    drag{
      target: parent.parent
      minimumX: 0
      minimumY: 0
      maximumX: parent.parent.parent.width - parent.parent.width
      maximumY: parent.parent.parent.height - parent.parent.height
      smoothed: true
    }
  }

  // Left ruler
  Rectangle {
    width: rulersThickness
    height: parent.parent.height
    color: "transparent"
    anchors.horizontalCenter: parent.left
    anchors.verticalCenter: parent.verticalCenter

    MouseArea {
      anchors.fill: parent
      cursorShape: Qt.SplitHCursor
      drag{ target: parent; axis: Drag.XAxis }
      onMouseXChanged: {
        if(drag.active){
          cardPane.width = cardPane.width - mouseX
          cardPane.x = cardPane.x + mouseX
          if(cardPane.width < minSize)
            cardPane.width = minSize
        }
      }
    }
  }

  // Right ruler
  Rectangle {
    width: rulersThickness
    height: parent.parent.height
    color: "transparent"
    anchors.horizontalCenter: parent.right
    anchors.verticalCenter: parent.verticalCenter

    MouseArea {
      anchors.fill: parent
      cursorShape: Qt.SplitHCursor
      drag{ target: parent; axis: Drag.XAxis }
      onMouseXChanged: {
        if(drag.active){
          cardPane.width = cardPane.width + mouseX
          if(cardPane.width < minSize)
              cardPane.width = minSize
        }
      }
    }
  }

  // Top ruler
  Rectangle {
    width: parent.parent.width
    height: rulersThickness
    x: parent.x / 2
    y: 0
    color: "transparent"
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.verticalCenter: parent.top

    MouseArea {
      anchors.fill: parent
      cursorShape: Qt.SplitVCursor
      drag{ target: parent; axis: Drag.YAxis }
      onMouseYChanged: {
        if(drag.active){
          cardPane.height = cardPane.height - mouseY
          cardPane.y = cardPane.y + mouseY
          if(cardPane.height < minSize)
            cardPane.height = minSize
        }
      }
    }
  }

  // Bottom ruler
  Rectangle {
    width: parent.parent.width
    height: rulersThickness
    x: parent.x / 2
    y: parent.y
    color: "transparent"
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.verticalCenter: parent.bottom

    MouseArea {
      anchors.fill: parent
      cursorShape: Qt.SplitVCursor
      drag{ target: parent; axis: Drag.YAxis }
      onMouseYChanged: {
        if(drag.active){
          cardPane.height = cardPane.height + mouseY
          if(cardPane.height < minSize)
            cardPane.height = minSize
        }
      }
    }
  }
}
