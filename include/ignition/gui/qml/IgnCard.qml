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
   * True to have a collapse button
   */
  property bool showCollapseButton: true

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
   * ▼
   */
  property string collapseIcon: "\u25B4"

  /**
   * ▲
   */
  property string expandIcon: "\u25BE"

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
   * Stores last height of plugin to expand to.
   */
  property int lastHeight: 50

  /**
   * True if there's at least one anchor set for the card.
   * There's no way to check the anchors themselves, so we need
   * to keep track of this ourselves.
   */
  property bool anchored: false

  /**
   * Minimum width of the card pane
   */
  property int cardMinimumWidth: 250;

  /**
   * Minimum height of the card pane
   */
  property int cardMinimumHeight: 250;

  /**
   * Tool bar background color
   */
  property string pluginToolBarColor:
    typeof MainWindow === "undefined" ||
    MainWindow.pluginToolBarColorLight === "" ||
    MainWindow.pluginToolBarColorDark === "" ?
    Material.accent :
    (Material.theme === Material.Light) ?
    MainWindow.pluginToolBarColorLight : MainWindow.pluginToolBarColorDark

  /**
   * Tool bar text color
   */
  property string pluginToolBarTextColor:
    typeof MainWindow === "undefined" ||
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
  id: cardPane

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
    var bgItemTemp = helpers.ancestorByName(cardPane, "background")
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
    var parentSplit = helpers.ancestorByName(cardPane, /^split_item/);

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
    cardPane.anchors.right = undefined
    cardPane.anchors.left = undefined
    cardPane.anchors.top = undefined
    cardPane.anchors.bottom = undefined
    cardPane.anchors.fill = undefined
    cardPane.anchors.horizontalCenter = undefined
    cardPane.anchors.verticalCenter = undefined
    cardPane.anchors.baseline = undefined

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
//        target: cardPane;
//        parent: cardWindowContent;
//        x: 0
//        y: 0
//        width: cardWindowContent.width
//        height: cardWindowContent.height
//      }
//    },
    // Floating and Docked state are the expanded states
    State {
      name: "docked"
    },
    State {
      name: "floating"
    },
    // Docked collapsed state
    State {
      name: "docked_collapsed"
    },
    // Floating collapsed state
    State {
      name: "floating_collapsed"
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
    },
    Transition {
      from: "floating"
      to: "floating_collapsed"
      NumberAnimation {
        target: cardPane
        property: "height"
        duration: 200
        easing.type: Easing.OutCubic
        from: cardPane.height
        to: 50
      }
    },
    Transition {
      from: "floating_collapsed"
      to: "floating"
      NumberAnimation {
        target: cardPane
        property: "height"
        duration: 200
        easing.type: Easing.InCubic
        from: 50
        to: lastHeight
      }
    },
    Transition {
      from: "floating_collapsed"
      to: "docked"
      SequentialAnimation {
        ScriptAction {script: leaveFloatingState()}
        ScriptAction {script: enterDockedState()}
      }
    },
    Transition {
      from: "docked"
      to: "docked_collapsed"
      NumberAnimation {
        target: cardPane
        property: "parent.Layout.minimumHeight"
        duration: 200
        easing.type: Easing.OutCubic
        from: cardPane.height
        to: 50
      }
    },
    Transition {
      from: "docked_collapsed"
      to: "docked"
      NumberAnimation {
        target: cardPane
        property: "parent.Layout.minimumHeight"
        duration: 200
        easing.type: Easing.InCubic
        from: 50
        to: content.children[0] === undefined ? 50 : content.children[0].Layout.minimumHeight
      }
    },
    Transition {
      from: "docked_collapsed"
      to: "floating"
      SequentialAnimation {
        ScriptAction {script: leaveDockedState()}
        ScriptAction {script: enterFloatingState()}
      }
    },
    Transition {
      from: "docked"
      to: "floating_collapsed"
      SequentialAnimation {
        ScriptAction {script: leaveDockedState()}
        ScriptAction {script: enterFloatingState()}
        NumberAnimation {
          target: cardPane
          property: "height"
          duration: 200
          easing.type: Easing.OutCubic
          from: cardPane.height
          to: 50
        }
      }
    },
    Transition {
      from: "docked_collapsed"
      to: "floating_collapsed"
      SequentialAnimation {
        ScriptAction {script: leaveDockedState()}
        ScriptAction {script: enterFloatingState()}
      }
    },
    Transition {
      from: "floating_collapsed"
      to: "docked_collapsed"
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

    const collapsed = cardPane.height === 50

    // Reparent to split
    cardPane.parent = splitItem;

    // Retain collapsed or expanded state
    cardPane.parent.Layout.minimumHeight = collapsed ? 50 : content.children[0].Layout.minimumHeight;
  }

  /**
   * Called when the floating state is entered.
   */
  function enterFloatingState()
  {
    const collapsed = cardPane.parent.Layout.minimumHeight === 50;
    // Reparent to main window's background
    cardPane.parent = backgroundItem

    // Resize to minimum size
    cardPane.clearAnchors();
    cardPane.width = content.children[0].Layout.minimumWidth;

    // Retain collapsed or expanded state
    cardPane.height = collapsed ? 50 : content.children[0].Layout.minimumHeight;
    lastHeight = content.children[0].Layout.minimumHeight;
  }

  /**
   * Called when the docked state is left.
   */
  function leaveDockedState()
  {
    // Remove from split (delete split if needed)
    backgroundItem.removeSplitItem(helpers.ancestorByName(cardPane,
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
//    width: cardPane.width;
//    height: cardPane.height;
//    visible: false;
//    id: cardWindow
//
//    Rectangle {
//      id: cardWindowContent
//      anchors.fill: parent
//    }
//
//    onClosing: {
//      cardPane.state = "docked"
//    }
//  }

  /**
   * Top toolbar
   */
  ToolBar {
    id: cardToolbar
    objectName: "cardToolbar"
    visible: cardPane.showTitleBar
    Material.foreground: Material.foreground
    Material.background: pluginToolBarColor
    width: cardPane.width
    height: cardPane.showTitleBar ? 50 : 0
    x: 0
    z: 100

    // For drag
    MouseArea {
      anchors.fill: parent
      drag {
        target: cardPane
        minimumX: 0
        minimumY: 0
        maximumX: cardPane.parent ? cardPane.parent.width - cardPane.width : cardPane.width
        maximumY: cardPane.parent ? cardPane.parent.height - cardPane.height : cardPane.height
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
        text: (cardPane.state === "docked" || cardPane.state === "docked_collapsed") ? floatIcon : dockIcon
        contentItem: Text {
          text: dockButton.text
          font: dockButton.font
          opacity: enabled ? 1.0 : 0.3
          color: cardPane.Material.background
          horizontalAlignment: Text.AlignHCenter
          verticalAlignment: Text.AlignVCenter
        }
        visible: cardPane.showDockButton && !cardPane.standalone
        onClicked: {
          switch(cardPane.state) {
            case "floating_collapsed": {
              cardPane.state = "docked_collapsed"
              break;
            }
            case "floating": {
              cardPane.state = "docked"
              break;
            }
            case "docked": {
              cardPane.state = "floating"
              break;
            }
            case "docked_collapsed": {
              cardPane.state = "floating_collapsed"
              break;
            }
          }
        }
      }

      // Collapse button
      ToolButton {
        id: collapseButton
        visible: cardPane.showCollapseButton && !cardPane.standalone
        text: cardPane.height <= 50.5 ? expandIcon : collapseIcon;
        contentItem: Text {
          text: collapseButton.text
          font: collapseButton.font
          opacity: enabled ? 1.0 : 0.3
          color: cardPane.Material.background
          horizontalAlignment: Text.AlignHCenter
          verticalAlignment: Text.AlignVCenter
        }
        onClicked: {
          switch(cardPane.state) {
            case "floating_collapsed": {
              cardPane.state = "floating"
              break;
            }
            case "floating": {
              // When user manually minimized the plugin using resize
              if(cardPane.height === 50) {
                // Handles the case when a floating plugin is loaded using config
                if(lastHeight === 50) {
                  lastHeight = content.children[0].Layout.minimumHeight;
                }
                // Set state to floating collapsed and then expand for animation
                cardPane.state = "floating_collapsed"
                cardPane.state = "floating"
              } else {
                lastHeight = cardPane.height
                // Set card state to collapsed
                cardPane.state = "floating_collapsed"
              }
              break;
            }
            case "docked": {
              cardPane.state = "docked_collapsed"
              break;
            }
            case "docked_collapsed": {
              cardPane.state = "docked"
              break;
            }
          }
        }
      }

      // Close button
      ToolButton {
        id: closeButton
        visible: cardPane.showCloseButton && !cardPane.standalone
        text: closeIcon
        contentItem: Text {
          text: closeButton.text
          font: closeButton.font
          opacity: enabled ? 1.0 : 0.3
          color: cardPane.Material.background
          horizontalAlignment: Text.AlignHCenter
          verticalAlignment: Text.AlignVCenter
        }
        onClicked: {
          cardPane.close();
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
      onTriggered: cardPane.showSettingsDialog();
    }
    MenuItem {
      text: "Close"
      onTriggered: cardPane.close();
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
    parent: cardPane.parent
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
    anchors.topMargin: cardPane.showTitleBar ? 50 : 0
    clip: true
    color: cardBackground

    onChildrenChanged: {
      // Set the height and width of the cardPane when child plugin is attached
      if (children.length > 0) {
        cardMinimumWidth = content.children[0].Layout.minimumWidth;
        cardMinimumHeight = content.children[0].Layout.minimumHeight;
        cardPane.width = cardMinimumWidth
        cardPane.height = cardMinimumHeight
      }

      cardPane.syncTheFamily()
    }

    /**
     * Conveniently expose card to children
     */
    function card() {
      return cardPane;
    }
  }

  IgnRulers {
    anchors.fill: parent
    enabled: cardPane.state === "floating" && resizable
    minSize: cardPane.minSize
    target: cardPane
  }
}
