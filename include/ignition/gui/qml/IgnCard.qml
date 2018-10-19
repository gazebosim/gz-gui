import QtQuick 2.9
import QtQuick.Controls 1.4 as QQC1
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.1
import QtQuick.Layouts 1.3
import QtQuick.Window 2.2
import "qrc:/qml"

// TODO: don't use "parent"
Pane {
  /**
   * Minimum length of each dimension
   */
  property int minSize: 50

  /**
   * True to have a dock button
   */
  property bool showDockButton: true

  /**
   * True to have a close button
   */
  property bool showCloseButton: true

  /**
   * True to have a title bar
   */
  property bool showTitleBar: true

  /**
   * True to have draggable rulers for resizing
   */
  property bool resizable: true

  /**
   * True if plugin is in a standalone dialog
   */
  property bool standalone: false

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

  Material.onBackgroundChanged: {
    titleLabel.color = Material.background
  }

  /**
   * ID within QML
   */
  id: card

  /**
   * Object name accessible from C++
   */
  objectName: "plugin_" + Math.floor(Math.random() * 100000);

  // TODO(louise) Support choosing between:
  // * a transparent background
  // * a custom color, in which case there will be elevation
  // Elevation only works if background is not transparent.
  Material.elevation: 6
  background: Rectangle {
    color: "transparent"
  }
  padding: 0
  y: 50
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
      }
    }
  ]

  /**
   * Show settings dialog
   */
  function showSettingsDialog() {
    settingsDialog.open()
  }

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
    visible: card.showTitleBar
    Material.foreground: Material.foreground
    Material.background: Material.accent
    width: card.width
    height: card.showTitleBar ? 50 : 0
    x: 0
    y: -50
    z: 100

    // For drag
    MouseArea {
      anchors.fill: parent
      drag {
        target: card
        minimumX: 0
        minimumY: 0
        maximumX: card.parent ? card.parent.width - card.width : card.width
        maximumY: card.parent ? card.parent.height - card.height : card.height
        smoothed: true
      }
    }

    /**
     * The toolbar contents
     */
    RowLayout {
      spacing: 0
      anchors.fill: parent
      anchors.leftMargin: 10

      Label {
        id: titleLabel
        font.pixelSize: 16
        color: card.Material.background
        elide: Label.ElideRight
        horizontalAlignment: Qt.AlignHLeft
        verticalAlignment: Qt.AlignVCenter
        Layout.fillWidth: true
      }

      // Dock / undock button
      ToolButton {
        id: dockButton
        text: card.state === "docked" ? undockIcon : dockIcon
        contentItem: Text {
          text: dockButton.text
          font: dockButton.font
          opacity: enabled ? 1.0 : 0.3
          color: card.Material.background
          horizontalAlignment: Text.AlignHCenter
          verticalAlignment: Text.AlignVCenter
        }
        visible: card.showDockButton && !card.standalone
        onClicked: {
          const docked = card.state === "docked"
          card.state = docked ? "undocked" : "docked"
          undockedWindow.visible = docked
        }
      }

      // Close button
      ToolButton {
        id: closeButton
        visible: card.showCloseButton && !card.standalone
        text: closeIcon
        contentItem: Text {
          text: closeButton.text
          font: closeButton.font
          opacity: enabled ? 1.0 : 0.3
          color: card.Material.background
          horizontalAlignment: Text.AlignHCenter
          verticalAlignment: Text.AlignVCenter
        }
        onClicked: {
          card.close();
        }
      }
    }
  }

  // For context menu
  MouseArea {
    anchors.fill: content
    acceptedButtons: Qt.RightButton
    onClicked: {
      contextMenu.x = mouseX
      contextMenu.y = mouseY
      contextMenu.open()
    }
  }

  Menu {
    id: contextMenu
    transformOrigin: Menu.TopRight
    MenuItem {
      text: "Settings"
      onTriggered: card.showSettingsDialog();
    }
    MenuItem {
      text: "Close"
      onTriggered: card.close();
    }
  }

  IgnCardSettings {
    id: settingsDialog
    modal: false
    focus: true
    title: pluginName + " settings"
    parent: card.parent
    x: parent ? (parent.width - width) / 2 : 0
    y: parent ? (parent.height - height) / 2 : 0
  }

  /**
   * Card contents
   */
  Rectangle {
    objectName: "content"
    id: content
    anchors.fill: parent
    clip: true
    color: "transparent"

    /**
     * Conveniently expose card to children
     */
    function card() {
      return card;
    }
  }

  IgnRulers {
    anchors.fill: parent
    enabled: resizable
    minSize: card.minSize
    target: card
  }
}
