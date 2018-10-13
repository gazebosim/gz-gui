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
  property string floatIcon: "\u25A1"

  /**
   * ✕
   */
  property string closeIcon: "\u2715"

  /**
   *
   */
  property var backgroundItem: null

  /**
   * Close signal
   */
  signal close()

//  Material.onBackgroundChanged: {
//    titleLabel.color = Material.background
//  }

  /**
   * ID within QML
   */
  id: card

  /**
   * Object name accessible from C++
   */
  objectName: "plugin_" + Math.floor(Math.random() * 100000);

  onParentChanged: {
    if (undefined === parent || null === parent)
      return;

    anchors.fill = Qt.binding(function() {return parent})
    anchors.fill = Qt.binding(function() {return parent})
    parent.height = Qt.binding(function() {return height})
    parent.width = Qt.binding(function() {return width})

    this.syncTheFamily();
  }

  /**
   *
   */
  function syncTheFamily() {
    if (undefined == parent)
      return;

    if (content.children.length != 1)
      return;

    parent.Layout.minimumWidth = content.children[0].Layout.minimumWidth;
    parent.Layout.minimumHeight = content.children[0].Layout.minimumHeight;
  }


//  // TODO(louise) Support choosing between:
//  // * a transparent background
//  // * a custom color, in which case there will be elevation
//  // Elevation only works if background is not transparent.
//  Material.elevation: 6
  background: Rectangle {
    color: "transparent"
  }

  padding: 0

  state: "docked"

  states: [
//    State {
//      name: "cardWindow"
//      ParentChange {
//        target: card;
//        parent: cardWindowContent;
//        x: 0
//        y: 0
//        width: cardWindowContent.width
//        height: cardWindowContent.height
//      }
//    },
    State {
      name: "docked"
    },

    State {
      name: "floating"
    }
  ]

  transitions: [
    Transition {
      from: "docked"
      to: "floating"
      SequentialAnimation {
        ScriptAction {script: leaveDockedState()}
        ScriptAction {script: enterFloatingState()}
      }
    },
    Transition {
      from: "floating"
      to: "docked"
      SequentialAnimation {
        ScriptAction {script: leaveFloatingState()}
        ScriptAction {script: enterDockedState()}
      }
    }
  ]

  function enterDockedState()
  {
    // Add new split
    var splitName = backgroundItem.addSplitItem();
    var splitItem = backgroundItem.childItems[splitName];

    // Reparent to split
    card.parent = splitItem;
  }

  function enterFloatingState()
  {
    // Reparent to main window's background
    card.parent = backgroundItem

    // Resize to minimum size
    card.anchors.right = undefined
    card.anchors.left = undefined
    card.anchors.top = undefined
    card.anchors.bottom = undefined
    card.anchors.fill = undefined
    card.width = content.children[0].Layout.minimumWidth;
    card.height = content.children[0].Layout.minimumHeight;
  }

  function leaveDockedState()
  {
    // Keep a reference to the background
    backgroundItem = ancestorByName("background")

    // Remove from split (delete split if needed)
    backgroundItem.removeSplitItem(ancestorByName(/^split_item/).objectName)
  }

  function leaveFloatingState()
  {
  }

//  /**
//   * Window for undocking
//   */
//  Window {
//    // TODO: resize
//    width: card.width;
//    height: card.height;
//    visible: false;
//    id: cardWindow
//
//    Rectangle {
//      id: cardWindowContent
//      anchors.fill: parent
//    }
//
//    onClosing: {
//      card.state = "docked"
//    }
//  }

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

      // Dock / floating button
      // TODO(louise) support window state
      ToolButton {
        id: dockButton
        text: card.state === "docked" ? floatIcon : dockIcon
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
          card.state = docked ? "floating" : "docked"
//          cardWindow.visible = docked
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

  /**
   * Show settings dialog
   */
  function showSettingsDialog() {
    settingsDialog.open()
  }

  Dialog {
    id: settingsDialog
    modal: false
    focus: true
    title: pluginName + " settings"
    parent: card.parent
    x: parent ? (parent.width - width) / 2 : 0
    y: parent ? (parent.height - height) / 2 : 0

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
        visible: !card.standalone
        enabled: card.showTitleBar
        checked: card.showCloseButton
        onToggled: {
          card.showCloseButton = checked
        }
      }

      Switch {
        id: dockSwitch
        text: "Show dock button"
        visible: !card.standalone
        enabled: card.showTitleBar
        checked: card.showDockButton
        onToggled: {
          card.showDockButton = checked
        }
      }

      Switch {
        id: resizableSwitch
        text: "Resizable"
        visible: card.state === "floating"
        checked: card.resizable
        onToggled: {
          card.resizable = checked
        }
      }

      GridLayout {
        width: parent.width
        columns: 2
        visible: card.state === "floating"

        Label {
          text: "Position"
          font.weight: Font.DemiBold
        }

        Text {
          text: ""
        }

        IgnSpinBox {
          maximumValue: card.parent ? card.parent.width - card.width : minSize
          onVisibleChanged: value = card.x
          onValueChanged: {
            card.x = value;
          }
        }
        Label {
          text: "X"
        }
        IgnSpinBox {
          maximumValue: card.parent ? card.parent.height - card.height : minSize
          onVisibleChanged: value = card.y
          onValueChanged: {
            card.y = value;
          }
        }
        Label {
          text: "Y"
        }
        IgnSpinBox {
          maximumValue: 10000
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
          maximumValue: card.parent ? card.parent.width : minSize
          onVisibleChanged: {
            if (card)
              value = card.width
          }
          onValueChanged: {
            card.width = value;
          }
        }
        Label {
          text: "Width"
        }
        IgnSpinBox {
          maximumValue: card.parent ? card.parent.height : minSize
          onVisibleChanged: {
            if (card)
              value = card.height
          }
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

  /**
   * Card contents
   */
  Rectangle {
    objectName: "content"
    id: content
    anchors.fill: parent
    anchors.topMargin: card.showTitleBar ? 50 : 0
    clip: true
    color: "transparent"

    onChildrenChanged: {
      card.syncTheFamily()
    }

    /**
     * Conveniently expose card to children
     */
    function card() {
      return card;
    }
  }

  IgnRulers {
    anchors.fill: parent
    enabled: card.state === "floating" && resizable
    minSize: card.minSize
    target: card
  }

  function ancestorByName(_name)
  {
    var result = parent;
    while (result)
    {
      if (result.objectName.match(_name) !== null)
        break;

      result = result.parent;
    }

    return result;
  }
}
