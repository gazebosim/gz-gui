import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.1
import QtQuick.Window 2.2

// TODO: don't use "parent"
Pane {
  id: cardPane
  Material.elevation: 6
  padding: 0
  state: "docked"

  property int rulersThickness: 25
  property int minSize: 100

  /**
   * The plugin name, which goes on the toolbar
   */
  property alias pluginName: titleLabel.text

  /**
   * ✕
   */
  property string dockIcon: "\u2715"

  /**
   * □
   */
  property string undockIcon: "\u25A1"

  states: [
    State {
      name: "undocked"
      ParentChange {
        target: cardPane;
        parent: undockedWindowContent;
        x: 0
        y: 0
        width: undockedWindowContent.width
        height: undockedWindowContent.height
      }
    },
    State {
      name: "docked"
      ParentChange {
        target: cardPane;
        parent: parent;
      }
    }
  ]

  Window {
    // TODO: resize
    width: 400;
    height: 600;
    visible: false;
    id: undockedWindow

    Rectangle {
      id: undockedWindowContent
      anchors.fill: parent
    }

    onClosing: {
      cardPane.state = "docked"
    }
  }

  ToolBar {
    id: cardToolbar
    Material.foreground: "white"
    Material.background: Material.LightBlue
    width: cardPane.width
    x: 0
    y: 0
    z: 100

    RowLayout {
      spacing: 20
      anchors.fill: parent

      Label {
        id: titleLabel
        font.pixelSize: 20
        elide: Label.ElideRight
        horizontalAlignment: Qt.AlignHLeft
        verticalAlignment: Qt.AlignVCenter
        Layout.fillWidth: true

        // For drag
        MouseArea {
          anchors.fill: parent
          drag{
            target: cardPane
            minimumX: 0
            minimumY: 0
            maximumX: cardPane.parent.width - cardPane.width
            maximumY: cardPane.parent.height - cardPane.height
            smoothed: true
          }
        }
      }

      ToolButton {
        id: dockButton
        text: cardPane.state === "docked" ? undockIcon : dockIcon
        onClicked: {
          const docked = cardPane.state === "docked"
          cardPane.state = docked ? "undocked" : "docked"
          undockedWindow.visible = docked
        }
      }
    }
  }

  Rectangle {
    objectName: "content"
    id: content
    y: cardToolbar.height
    width: cardPane.width
    height: cardPane.height - cardToolbar.height
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
