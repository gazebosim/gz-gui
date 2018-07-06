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
   * Thickness of rulers used to resize card
   */
  property int rulersThickness: 25

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
   * Resized signal
   */
  signal resized()
  onWidthChanged: resized()
  onHeightChanged: resized()

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

//  Material.elevation: 6
  background: Rectangle {
    color: "transparent"
  }
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
    Material.elevation: 0
    width: card.width
    height: card.showTitleBar ? 50 : 0
    x: 0
    y: 0
    z: 100

    // For drag
    MouseArea {
      anchors.fill: parent
      drag{
        target: card
        minimumX: 0
        minimumY: 0
        maximumX: card.parent ? card.parent.width - card.width : card.width
        maximumY: card.parent ? card.parent.height - card.height : card.height
        smoothed: true
      }
    }

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
        visible: card.showDockButton
        onClicked: {
          const docked = card.state === "docked"
          card.state = docked ? "undocked" : "docked"
          undockedWindow.visible = docked
        }
      }

      // Close button
      ToolButton {
        id: closeButton
        visible: card.showCloseButton
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

  // For drag
  MouseArea {
    enabled: !showTitleBar
    anchors.fill: content
    drag{
      target: card
      minimumX: 0
      minimumY: 0
      maximumX: card.parent ? card.parent.width - card.width : card.width
      maximumY: card.parent ? card.parent.height - card.height : card.height
      smoothed: true
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

  Dialog {
    id: settingsDialog
    modal: false
    focus: true
    title: pluginName + " settings"
    parent: card.parent
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2

    Column {
      id: settingsColumn
      anchors.horizontalCenter: settingsDialog.horizontalCenter
      width: settingsDialog.width * 0.6

      Switch {
        id: titleSwitch
        text: "Show title bar"
        checked: card.showTitleBar
        onToggled: {
          card.showTitleBar = checked
          // why is binding not working?
          closeSwitch.enabled = checked
          dockSwitch.enabled = checked
        }
      }

      Switch {
        id: closeSwitch
        text: "Show close button"
        enabled: card.showTitleBar
        checked: card.showCloseButton
        onToggled: {
          card.showCloseButton = checked
        }
      }

      Switch {
        id: dockSwitch
        text: "Show dock button"
        enabled: card.showTitleBar
        checked: card.showDockButton
        onToggled: {
          card.showDockButton = checked
        }
      }

      Switch {
        id: resizableSwitch
        text: "Resizable"
        checked: card.resizable
        onToggled: {
          card.resizable = checked
        }
      }

      GridLayout {
        width: parent.width
        columns: 2

        Label {
          text: "Position"
          font.weight: Font.DemiBold
        }

        Text {
          text: ""
        }

        IgnSpinBox {
          maximumValue: card.parent.width - card.width
          onVisibleChanged: value = card.x
          onValueChanged: {
            card.x = value;
          }
        }
        Label {
          text: "X"
        }
        IgnSpinBox {
          maximumValue: card.parent.height - card.height
          onVisibleChanged: value = card.y
          onValueChanged: {
            card.y = value;
          }
        }
        Label {
          text: "Y"
        }
        IgnSpinBox {
          maximumValue: 1000
          onVisibleChanged: value = card.z
          onValueChanged: {
            card.z = value;
          }
        }
        Label {
          text: "Z"
        }
        Label {
          text: "Size"
          font.weight: Font.DemiBold
        }
        Text {
          text: ""
        }
        IgnSpinBox {
          maximumValue: card.parent.width
          onVisibleChanged: value = card.width
          onValueChanged: {
            card.width = value;
          }
        }
        Label {
          text: "Width"
        }
        IgnSpinBox {
          maximumValue: card.parent.height
          onVisibleChanged: value = card.height
          onValueChanged: {
            card.height = value;
          }
        }
        Label {
          text: "Height"
        }
      }
    }
  }

  Rectangle {
    objectName: "content"
    id: content
    y: cardToolbar.height
    width: card.width
    height: card.height - cardToolbar.height
    clip: true
    color: "transparent"

    /**
     * Conveniently expose card to children
     */
    function card() {
      return card;
    }
  }

  // Left ruler
  Rectangle {
    width: rulersThickness
    height: parent.parent.height
    visible: card.resizable
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
    visible: card.resizable
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
    visible: card.resizable
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
    visible: card.resizable
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
