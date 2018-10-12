import QtQuick 2.9
import QtQuick.Controls 1.1
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.1
import QtQuick.Layouts 1.3

/**
 * Main split view, which provides functions to add and remove child items
 * and splits.
 */
SplitView {

  id: background
  objectName: "background"

  property variant childItems: new Object()
  property variant childSplits: new Object()

  Rectangle {
    visible: MainWindow.pluginCount === 0
    anchors.fill: parent
    color: Material.background
    Label {
      id: startLabel;
      text: "Insert plugins to start!"
      anchors.fill: parent
      font.pointSize: 24
      horizontalAlignment: Label.AlignHCenter
      verticalAlignment: Label.AlignVCenter
      wrapMode: Label.Wrap
    }
  }

  /**
   * This function will appropriately create new items and splits according to
   * the current main window state (i.e. number of plugins...).
   * @return Name of added item.
   * TODO(louise) Accept configuration, so we know how to add the item
   * TODO(louise) Make this more flexible so we can have different window
   * arrangements
   */
  function addSplitItem()
  {
    var itemName = "";

    // First section goes in the top level SplitView, which is Qt.Horizontal
    if (MainWindow.pluginCount == 0)
    {
      itemName = addNewItem(background);
    }
    // The next one adds a Qt.Vertical split to the right
    else if (Object.keys(childSplits).length == 0)
    {
      var split = addNewSplit(background);
      split.orientation = Qt.Vertical;
      itemName = addNewItem(split);
    }
    // All subsequent ones are added to the vertical child split on the right
    else
    {
      var firstChildSplit = childSplits[Object.keys(childSplits)[0]];
      itemName = addNewItem(firstChildSplit);
    }

    return itemName
  }

  /**
   */
  function removeSplitItem(_name)
  {
    // Remove from split
    removeFromSplits(childItems[_name]);

    // Remove from dictionary and destroy
    delete childItems[_name];
  }

  /**
   * Create a new item and add it to the parent split.
   * @param Parent split
   * @return Item name
   */
  function addNewItem(_parentSplit)
  {
    // Create item
    var item = newItem.createObject(_parentSplit);

    // Unique name
    var itemName = "split_item_" + Math.floor(Math.random() * 100000)
    item.objectName = itemName;

    // Add to dictionary
    childItems[itemName] = item;

    // Add to parent
    _parentSplit.addItem(item);

    // TODO(louise) Find a way to sync item's minimum size with the split's
    // minimum size. Must keep track of several children and be aware of timing.

    return itemName;
  }

  /**
   * Create a new split and add it to the parent split.
   * @param Parent split
   */
  function addNewSplit(_parentSplit)
  {
    // Create split
    var split = newSplit.createObject(_parentSplit);

    // Unique name
    var splitName = "split_" + Math.floor(Math.random() * 100000)
    split.objectName = splitName;

    // Add to dictionary
    childSplits[splitName] = split;

    // Add to parent
    _parentSplit.addItem(split);

    return split;
  }

  /**
   * Removes item from its parent split and removes the split if that was
   * the last item in it.
   * TODO(louise) Actually find the parent split instead of trying to remove
   *              from every single split
   */
  function removeFromSplits(_item)
  {
    // Try removing from top-level split
    background.removeItem(_item);

    // Try removing from all splits
    Object.keys(childSplits).forEach(function(key)
    {
      var split = childSplits[key];

      split.removeItem(_item);

      // Is split is now empty, remove split
      if (split.__items.length === 0 && split !== background)
      {
        // Remove from array
        delete childSplits[key]

        // Remove from parent split
        removeFromSplits(split);

        // Destroy
        split.destroy();
      }
    });
  }

  /**
   * Helper function to remove from array by value
   * @param Array
   * @param Value to remove
   */
  function removeFromArray(_array, _value) {
    var id;
    while ((id = _array.indexOf(_value)) !== -1)
    {
      _array.splice(id, 1);
    }
    return _array;
  }

  /**
   * Component for creating new items
   */
  Component {
    id: newItem

    Rectangle {
      Layout.minimumWidth: 100
      Layout.minimumHeight: 100
      Layout.fillHeight: true
      Layout.fillWidth: true
    }
  }

  /**
   * Component for creating new splits
   */
  Component {
    id: newSplit

    SplitView {
    }
  }
}

