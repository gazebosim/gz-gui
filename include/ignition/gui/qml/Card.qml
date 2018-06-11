import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.1
import QtQuick.Window 2.2

// TODO: don't use "parent"
Pane {

  /**
   * Thickness of rulers used to resize card
   */
  property int rulersThickness: 25

  /**
   * Minimum length of each dimension
   */
  property int minSize: 100

  /**
   * True to have a dock button
   */
  property bool hasDockButton: true

  /**
   * True to have a close button
   */
  property bool hasCloseButton: true

  /**
   * True to have a title bar
   */
  property bool hasTitlebar: true

  /**
   * The plugin name, which goes on the toolbar
   */
  property alias pluginName: titleLabel.text

  /**
   * ▁
   */
  property string dockIcon: "\u2581"

  /**
   * □
   */
  property string undockIcon: "\u25A1"

  /**
   * ✕
   */
  property string closeIcon: "\u2715"

  /**
   * Close signal
   */
  signal close()

  /**
   * ID within QML
   */
  id: card

  /**
   * Object name accessible from C++
   */
  objectName: "plugin_" + Math.floor(Math.random() * 100000);

  Material.elevation: 6
  padding: 0
  state: "docked"

  states: [
    State {
      name: "undocked"
      ParentChange {
        target: card;
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
        target: card;
        parent: parent;
      }
    }
  ]

  /**
   * Window for undocking
   */
  Window {
    // TODO: resize
    width: card.width;
    height: card.height;
    visible: false;
    id: undockedWindow

    Rectangle {
      id: undockedWindowContent
      anchors.fill: parent
    }

    onClosing: {
      card.state = "docked"
    }
  }

  /**
   * Top toolbar
   */
  ToolBar {
    id: cardToolbar
    objectName: "cardToolbar"
    visible: card.hasTitlebar
    Material.foreground: "white"
    Material.background: Material.accent
    width: card.width
    height: card.hasTitlebar ? 50 : 0
    x: 0
    y: 0
    z: 100

    RowLayout {
      spacing: 10
      anchors.fill: parent
      anchors.leftMargin: 10

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
            target: card
            minimumX: 0
            minimumY: 0
            maximumX: card.parent.width - card.width
            maximumY: card.parent.height - card.height
            smoothed: true
          }
        }
      }

      // Dock / undock button
      ToolButton {
        id: dockButton
        text: card.state === "docked" ? undockIcon : dockIcon
        visible: card.hasDockButton
        onClicked: {
          const docked = card.state === "docked"
          card.state = docked ? "undocked" : "docked"
          undockedWindow.visible = docked
        }
      }

      // Close button
      ToolButton {
        id: closeButton
        visible: card.hasCloseButton
        text: closeIcon
        onClicked: {
          card.close();
        }
      }
    }
  }

  // For drag
  MouseArea {
    enabled: !hasTitlebar
    anchors.fill: content
    drag{
      target: card
      minimumX: 0
      minimumY: 0
      maximumX: card.parent.width - card.width
      maximumY: card.parent.height - card.height
      smoothed: true
    }
  }

  Rectangle {
    objectName: "content"
    id: content
    y: cardToolbar.height
    width: card.width
    height: card.height - cardToolbar.height
    clip: true
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
          var newCardX = Math.max(card.x + mouseX, 0)
          var newCardWidth = Math.max(card.width + (card.x - newCardX), minSize)
          if (newCardWidth === card.width)
            return;
          card.x = newCardX
          card.width = newCardWidth
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

          card.width = Math.max(card.width + mouseX, minSize)

          if (card.width + card.x > card.parent.width)
            card.width = card.parent.width - card.x
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
          var newCardY = Math.max(card.y + mouseY, 0)
          var newCardHeight = Math.max(card.height + (card.y - newCardY), minSize)
          if (newCardHeight === card.height)
            return;
          card.y = newCardY
          card.height = newCardHeight
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

          card.height = Math.max(card.height + mouseY, minSize)

          if (card.height + card.y > card.parent.height)
            card.height = card.parent.height - card.y
        }
      }
    }
  }
}