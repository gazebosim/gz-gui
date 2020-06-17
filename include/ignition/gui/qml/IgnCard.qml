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
   * The plugin backgroung color. Default: transparent
   */
  property string cardBackground: "#00000000"

  /**
   *
   */
  property var backgroundItem: null

  /**
   * True if there's at least one anchor set for the card.
   * There's no way to check the anchors themselves, so we need
   * to keep track of this ourselves.
   */
  property bool anchored: false

  /**
   * Tool bar background color
   */
  property string pluginToolBarColor:
    MainWindow.pluginToolBarColorLight === "" ||
    MainWindow.pluginToolBarColorDark === "" ?
    Material.accent :
    (Material.theme === Material.Light) ?
    MainWindow.pluginToolBarColorLight : MainWindow.pluginToolBarColorDark

  /**
   * Tool bar text color
   */
  property string pluginToolBarTextColor:
    MainWindow.pluginToolBarTextColorLight === "" ||
    MainWindow.pluginToolBarTextColorDark === "" ?
    Material.background :
    (Material.theme === Material.Light) ?
    MainWindow.pluginToolBarTextColorLight : MainWindow.pluginToolBarTextColorDark

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
  objectName: "plugin" + Math.floor(Math.random() * 100000);

  // Stop scroll propagation to widgets below
  MouseArea {
    anchors.fill: parent
    onWheel: {
      wheel.accepted = true
    }
  }

  /**
   * Callback when the parent has changed.
   */
  onParentChanged: {
    if (undefined === parent || null === parent)
      return;

    // Bind anchors
    anchors.fill = Qt.binding(function() {return parent})
    anchors.fill = Qt.binding(function() {return parent})
    parent.height = Qt.binding(function() {return height})
    parent.width = Qt.binding(function() {return width})

    // Keep a reference to the background
    // TODO(louise) This feels hacky, the card shouldn't care about the background,
    // but I haven't figured out yet how the card can tell IgnSplit to create
    // a new split and add the card to it. There must be a way using signals, events
    // or global functions...?
    var bgItemTemp = helpers.ancestorByName(card, "background")
    if (bgItemTemp)
      backgroundItem = bgItemTemp;

    this.syncTheFamily();
  }

  /**
   * Forward the child content's size preferences to the parent split's layout
   * TODO(louise) This looks really clunky, ideally the card shouldn't need
   * any knowledge of splits
   */
  function syncTheFamily() {
    var parentSplit = helpers.ancestorByName(card, /^split_item/);

    if (undefined == parentSplit)
      return;

    if (content.children.length != 1)
      return;

    parentSplit.Layout.minimumWidth = content.children[0].Layout.minimumWidth;
    parentSplit.Layout.minimumHeight = content.children[0].Layout.minimumHeight;
  }

  /**
   * Clear all anchors
   */
  function clearAnchors() {
    card.anchors.right = undefined
    card.anchors.left = undefined
    card.anchors.top = undefined
    card.anchors.bottom = undefined
    card.anchors.fill = undefined
    card.anchors.horizontalCenter = undefined
    card.anchors.verticalCenter = undefined
    card.anchors.baseline = undefined

    anchored = false
  }

  IgnHelpers {
    id: helpers
  }

  // TODO(louise) Support choosing between:
  // * a transparent background
  // * a custom color, in which case there will be elevation
  // Elevation only works if background is not transparent.
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

  /**
   * Called when the docked state is entered.
   */
  function enterDockedState()
  {
    // Add new split
    var splitName = backgroundItem.addSplitItem();
    var splitItem = backgroundItem.childItems[splitName];

    // Reparent to split
    card.parent = splitItem;
  }

  /**
   * Called when the floating state is entered.
   */
  function enterFloatingState()
  {
    // Reparent to main window's background
    card.parent = backgroundItem

    // Resize to minimum size
    card.clearAnchors();
    card.width = content.children[0].Layout.minimumWidth;
    card.height = content.children[0].Layout.minimumHeight;
  }

  /**
   * Called when the docked state is left.
   */
  function leaveDockedState()
  {
    // Remove from split (delete split if needed)
    backgroundItem.removeSplitItem(helpers.ancestorByName(card,
        /^split_item/).objectName)
  }

  /**
   * Called when the floating state is left.
   */
  function leaveFloatingState()
  {
    // Do nothing
  }

// TODO(louise): re-enable window state support
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
    Material.background: pluginToolBarColor
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
        color: pluginToolBarTextColor
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
    anchors.topMargin: card.showTitleBar ? 50 : 0
    clip: true
    color: cardBackground

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
}
