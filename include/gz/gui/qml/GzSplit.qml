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
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.1
import QtQuick.Layouts 1.3

/**
 * Main item, which provides functions to add and remove child items.
 */
Item
{
  id: root

  Item
  {
    id: floatingArea;
    visible: children.length > 0
    anchors.fill: parent
    z: 1

    function dockingArea() {
      return background
    }

    function removeFromParent()
    {
      removeItem(this);
    }
  }

  Rectangle
  {
    id: startLabel;
    visible: _MainWindow.pluginCount === 0
    anchors.fill: parent
    color: Material.background
    Label {
      text: "Insert plugins to start!"
      anchors.fill: parent
      font.pointSize: 24
      horizontalAlignment: Label.AlignHCenter
      verticalAlignment: Label.AlignVCenter
      wrapMode: Label.Wrap
    }
  }

  /**
   * Calculate the minimum width needed for each split by taking the max
   * of the minimum widths of all the cards in a split
   */
  function calcMinimumWidth(contentChildren) {
    var maxMinWidth = 0;
    for (var key in contentChildren) {
      var child = contentChildren[key];
      if (child.children.length > 0 && child.children[0].hasOwnProperty("cardMinimumWidth")) {
        maxMinWidth = Math.max(maxMinWidth, child.children[0].cardMinimumWidth);
      }
    }

    return maxMinWidth;
  }

  /**
   * Main SplitView that holds the main view port and the side panel.
   */
  SplitView
  {
    id: background
    objectName: "background"
    clip: true
    anchors.fill: parent

    /**
     * Main view port
     * This is intended to only contain one card, so it doesn't need to be a SplitView object,
     * but keeping the parent type the same between the mainView and the sidePanel makes
     * the code cleaner.
     */
    SplitView {
      id: mainView
      clip: true
      orientation: Qt.Vertical
      visible: count > 0
      SplitView.fillWidth : true
      SplitView.fillHeight : true
      SplitView.minimumWidth: root.calcMinimumWidth(mainView.contentChildren)
    }

    ScrollView {
      id: scrollView
      clip: true
      visible: sidePanel.count > 0
      property var scrollBarWidth: 17

      contentHeight: Math.max(sidePanel.contentHeight, parent.height)
      contentWidth: availableWidth

      SplitView.minimumWidth: root.calcMinimumWidth(sidePanel.contentChildren) + scrollBarWidth

      ScrollBar.vertical.policy: ScrollBar.AlwaysOn

      SplitView {
        id: sidePanel
        orientation: Qt.Vertical
        visible: count > 0
        clip: true
        anchors.fill: parent
        anchors.rightMargin: scrollView.scrollBarWidth

        // Temporarily create the handle object to get its height. Use `helpers` as the parent
        // to avoid warnings.
        readonly property double handleHeight: handle.createObject(helpers).implicitHeight

        // Establish a binding for contentHeight on the children's minimumHeight and maximumHeight,
        // but not their height as doing so causes a subtle binding loop.
        onContentChildrenChanged: {
          for (var i in contentChildren)
          {
            var child = contentChildren[i]
            child.SplitView.onMinimumHeightChanged.connect(calcContentHeight)
            child.SplitView.onMaximumHeightChanged.connect(calcContentHeight)
          }
        }

        function calcContentHeight() {
          var height = 0
          for (var i in contentChildren)
          {
            var child = contentChildren[i]
            height += helpers.clamp(child.height, child.SplitView.minimumHeight, child.SplitView.maximumHeight)
          }

          // Account for the height of the handle that's placed between each draggable
          // item in a SplitView
          if (count > 0)
          {
            height += (count - 1) * handleHeight
          }

          contentHeight = height
        }
      }
    }

    GzHelpers {
      id: helpers
      visible: false
    }


    /**
     * This adds the container split item and places the card init. This is called from Application.cc
     */
    function addCard(card) {
      var splitItem = _addSplitItem()
      card.parent = splitItem
    }

    function _addSplitItem() {
      if (mainView.count == 0) {
        return _addNewItem(mainView);
      } else {
        return _addNewItem(sidePanel);
      }
    }

    function _addNewItem(_split) {
      // Create item
      var item = newItem.createObject(_split);

      // Unique name
      var itemName = "split_item_" + Math.floor(Math.random() * 100000)
      item.objectName = itemName;

      // Add to parent
      _split.addItem(item);
      return item;
    }

    Component {
      id: newItem
      Item {
        id: cardContainer
        clip: true
        SplitView.minimumWidth: children.length > 0 ? children[0].cardMinimumWidth: 0
        SplitView.minimumHeight: children.length > 0 ? children[0].cardMinimumHeight: 0
        SplitView.maximumHeight: children.length > 0 ? children[0].cardMaximumHeight: 0

        implicitWidth: children.length > 0 ? children[0].implicitWidth: 0
        implicitHeight: children.length > 0 ? children[0].implicitHeight: 0

        function dockingArea() {
          return background
        }

        /**
         * This is called from GzCard to put the card in the floating state.
         * This removes the card from the parent (cardContainer) and places it
         * in the floatingArea object
         */
        function floatCard(card) {
          removeFromParent();
          card.parent = floatingArea
        }

        function removeFromParent()
        {
          SplitView.view.removeItem(this);
        }
      }
    }
  }
}
