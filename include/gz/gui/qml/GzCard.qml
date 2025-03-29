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
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.1
import QtQuick.Layouts 1.3
import QtQuick.Window 2.2
import "qrc:/qml"

// TODO: don't use "parent"
Pane {
  clip: true
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
   * True if there's at least one anchor set for the card.
   * There's no way to check the anchors themselves, so we need
   * to keep track of this ourselves.
   */
  property bool anchored: false

  /**
   * Keep track of whether the card is floating
   */
  property bool floating: false

  /**
   * Minimum width of the card pane
   */
  property alias cardMinimumWidth: content.minimumWidth;

  /**
   * Minimum height of the card pane
   */
  property double cardMinimumHeight: content.minimumHeight;


  property double cardMaximumHeight: Infinity


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


  contentWidth: content.implicitWidth
  contentHeight: content.implicitHeight

  // Stop scroll propagation to widgets below
  MouseArea {
    anchors.fill: parent
    onWheel: (wheel) => {
      wheel.accepted = true
    }
  }


  GzHelpers {
    id: helpers
    visible: false
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
    // Floating and Docked state are the expanded states
    State {
      name: "docked"
      StateChangeScript { script: enterDockedState() }
      PropertyChanges {
        target: cardPane
        cardMinimumHeight: content.minimumHeight
        cardMaximumHeight: Infinity
        anchors.fill: parent
      }
    },
    State {
      name: "floating"
      StateChangeScript { script: enterFloatingState() }
      PropertyChanges {
        target: cardPane
        height: cardPane.implicitHeight
        anchors.fill: undefined
      }
    },
    // Docked collapsed state
    State {
      name: "docked_collapsed"
      StateChangeScript { script: enterDockedState() }
      PropertyChanges {
        target: cardPane
        cardMaximumHeight: cardToolbar.height
        cardMinimumHeight: cardToolbar.height
        anchors.fill: parent
      }
    },
    // Floating collapsed state
    State {
      name: "floating_collapsed"
      StateChangeScript { script: enterFloatingState() }
      PropertyChanges {
        target: cardPane
        height: cardToolbar.height
        anchors.fill: undefined
      }
    }
  ]


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
  /**
   * Called when the docked state is entered.
   */
  function enterDockedState()
  {
    // It's possible to enter the docking state when not floating
    // (e.g. at initialization or when transitioning from docked_collapsed).
    // Do the actual docking only if the card is floating currently.
    if (cardPane.floating)
    {
      var dockingArea = cardPane.parent.dockingArea()
      dockingArea.addCard(cardPane);
    }

    cardPane.floating = false
  }

  /**
   * Called when the floating state is entered.
   */
  function enterFloatingState()
  {
    if (cardPane.floating)
      return

    parent.floatCard(cardPane)
    // Resize to minimum size
    cardPane.clearAnchors();
    // Set width since we're clearing anchors
    cardPane.width = content.minimumWidth;
    cardPane.height = content.minimumHeight;
    cardPane.floating = true
  }

  function removeFromParent()
  {
    if (!cardPane.floating)
      parent.removeFromParent()
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
              cardPane.state = "floating_collapsed"
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

  GzCardSettings {
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

    clip: true
    color: cardBackground
    readonly property double minimumWidth: children.length > 0 ? children[0].Layout.minimumWidth : 0
    readonly property double minimumHeight: children.length > 0 ? children[0].Layout.minimumHeight : 0

    // We're using minimumWidth and minimumHeight for the implicit sizes because plugins generally
    // only provide those. Ideally, each plugin would also provide implicit or preferred sizes,
    // but that hasn't been the case historically.
    implicitWidth: minimumWidth
    implicitHeight: minimumHeight

    // Anchor content under the toolbar
    anchors.top: cardToolbar.bottom
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.bottom: parent.bottom

    /**
     * Conveniently expose card to children
     */
    function card() {
      return cardPane;
    }
  }

  GzRulers {
    anchors.fill: parent
    enabled: cardPane.state === "floating" && resizable
    minSize: cardPane.minSize
    target: cardPane
  }
}
