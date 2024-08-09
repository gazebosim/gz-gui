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
import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts

/**
 * Main split view, which provides functions to add and remove child items
 * and splits.
 */
SplitView {

  id: background
  objectName: "background"
  clip: true

  GzHelpers {
    id: helpers
  }

  /**
   * Dictionary of all split items contained in this split.
   */
  property variant childItems: new Object()

  /**
   * Dictionary of all splits nested into this split.
   */
  property variant childSplits: new Object()

  Rectangle {
    id: startLabel;
    visible: false; //MainWindow.pluginCount === 0
    anchors.fill: parent
    color: "red" //Material.background
    Label {
      text: "Insert plugins to start!"
      anchors.fill: parent
      font.pointSize: 24
      horizontalAlignment: Label.AlignHCenter
      verticalAlignment: Label.AlignVCenter
      wrapMode: Label.Wrap
    }
    SplitView.fillWidth: true
  }

  /**
   * Recalculate minimum size for all splits
   */
  function recalculateMinimumSizes()
  {
    for (var name in childSplits)
    {
      childSplits[name].split.recalculateMinimumSize()
    }
  }

  /**
   * This function will appropriately create new items and splits according to
   * the current main window state.
   * @return Name of added item, which is prefixed by `split_item_`. The item
   * is empty, so the caller can add content to it.
   * TODO(louise) Accept configuration, so we know how to add the item
   * (orientation, size)
   * TODO(louise) Make this more flexible so we can have different window
   * arrangements
   */
  function addSplitItem()
  {
    var addedItem = "";
    // First section goes in the top level SplitView, which is Qt.Horizontal
    // 2 for helpers and startLabel
    console.log("bg count:", background.count, background.contentChildren.length, Object.keys(childSplits).length)
    if (background.contentChildren.length <= 2)
    {
      console.log("Add to bg split")
      addedItem = _addNewItem(background);
    }
    // The next one adds a Qt.Vertical split to the right
    else if (Object.keys(childSplits).length === 0)
    {
      console.log("Creating side split")
      // Add the split
      var split = _addNewSplit(background);
      console.log("Created ", split.objectName)


      // Then add a new item to the newly created split
      addedItem = _addNewItem(split);
    }
    // All subsequent ones are added to the vertical child split on the right
    else
    {
      console.log("Adding side split")
      // Get desired split (for now we have only one)
      var firstChildSplit = childSplits[Object.keys(childSplits)[0]];

      // Then add a new item to it
      addedItem = _addNewItem(firstChildSplit);
    }

    helpers.dump(background, "", 5)
    return addedItem
  }

  /**
   * Remove a split item according to its name.
   * @param Name of item, which must start with `split_item_`.
   */
  function removeSplitItem(_name)
  {
    console.log("Removing ", _name)
    // Remove from split
    _removeFromSplits(childItems[_name]);

    // Remove from dictionary and destroy
    delete childItems[_name];
  }

  /**
   * Create a new item and add it to a split.
   * Meant for internal use.
   * @param _split Split to add item to
   * @return Unique name of newly created item; starts with `split_item_`.
   */
  function _addNewItem(_split)
  {
    // Create item
    var item = newItem.createObject(_split);

    // Unique name
    var itemName = "split_item_" + Math.floor(Math.random() * 100000)
    item.objectName = itemName;

    // Add to dictionary
    childItems[itemName] = item;

    // Add to parent
    if (_split === background)
    {
      _split.addItem(item);
      item.SplitView.fillWidth = true;
    }
    else
    {
      _split.addItem(item);
    }
    console.log(itemName, item.objectName, "size: ", item.width, item.height)

    // return childItems[itemName];
    return itemName;
  }

  /**
   * Create a new split and add it to the parent split.
   * Meant for internal use.
   * @param _parentSplit Parent split.
   * @returns Newly created split.
   */
  function _addNewSplit(_parentSplit)
  {
    // Create split
    var splitWrapper = newSplit.createObject(_parentSplit);

    // Unique name
    var splitName = "split_" + Math.floor(Math.random() * 100000)
    splitWrapper.objectName = splitName;

    // Add to dictionary
    childSplits[splitName] = splitWrapper;

    // Add to parent
    // _parentSplit.addItem(splitWrapper);

    return splitWrapper;
  }

  /**
   * Removes item from its parent split and removes the split if that was
   * the last item in it.
   * Meant for internal use.
   * @param _item Item who is supposed to be removed from its parent split.
   */
  function _removeFromSplits(_item)
  {
    if (_item === undefined)
      return;

    var split = helpers.ancestorByName(_item, /^split_|^background$/);
    console.log("Ancestor ", split.objectName)

    if (!split)
    {
      console.error("Failed to find parent split for [", _item.objectName, "]")
      return;
    }

    if (split === background)
    {
      console.log("Final remove ", _item.objectName, " from  bg")
      split.removeItem(_item);
      for (var i in split.contentChildren)
      {
        console.log("   ", split.contentChildren[i])
      }
    }
    else
    {
      split.removeItem(_item);

      // If split is now empty, remove split
      // if (split.contentChildren.length === 0)
      // {
        console.log("Removing side split from bg", split.objectName)
        // Remove from array
        delete childSplits[split.objectName]

        // Remove from parent split
        _removeFromSplits(split);

        // Destroy
        split.destroy();
        helpers.dump(background, "", 5)
      // }
      // else
      // {
      //   split.recalculateMinimumSize();
      // }
    }
  }

  /**
   * Component for creating new items
   */
  Component {
    id: newItem

    ColumnLayout {
      id: rectContainer
      SplitView.minimumWidth: children[0]? children[0].Layout.minimumWidth : 0
      SplitView.minimumHeight: children[0]? children[0].Layout.minimumHeight : 0
      clip: true
    }
  }

  /**
   * Component for creating new splits
   */
  Component {
    id: newSplit
    SplitView {
      id: __split
      orientation: Qt.Vertical
      clip: true
      SplitView.minimumWidth: __split.contentChildren[0]? __split.contentChildren[0].SplitView.minimumWidth : 0
    }
  }
}
