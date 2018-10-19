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

  IgnHelpers {
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
    visible: MainWindow.pluginCount === 0
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
    var itemName = "";

    // First section goes in the top level SplitView, which is Qt.Horizontal
    // 2 for helpers and startLabel
    if (background.__items.length <= 2)
    {
      itemName = _addNewItem(background);
    }
    // The next one adds a Qt.Vertical split to the right
    else if (Object.keys(childSplits).length === 0)
    {
      // Add the split
      var split = _addNewSplit(background);
      split.orientation = Qt.Vertical;

      // Then add a new item to the newly created split
      itemName = _addNewItem(split);
    }
    // All subsequent ones are added to the vertical child split on the right
    else
    {
      // Get desired split (for now we have only one)
      var firstChildSplit = childSplits[Object.keys(childSplits)[0]];

      // Then add a new item to it
      itemName = _addNewItem(firstChildSplit);
    }

    return itemName
  }

  /**
   * Remove a split item according to its name.
   * @param Name of item, which must start with `split_item_`.
   */
  function removeSplitItem(_name)
  {
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
    _split.addItem(item);

    // Make sure that changes to the item's minimum size get propagated to the
    // split.
    if (_split !== background)
    {
      item.minimumSizeChanged.connect(function(){
        _split.recalculateMinimumSize()
      });
    }

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
   * Meant for internal use.
   * @param _item Item who is supposed to be removed from its parent split.
   */
  function _removeFromSplits(_item)
  {
    if (_item === undefined)
      return;

    var split = helpers.ancestorByName(_item, /^split_|^background$/);

    if (!split)
    {
      console.error("Failed to find parent split for [", _item.objectName, "]")
      return;
    }

    split.removeItem(_item);

    // If split is now empty, remove split
    if (split.__items.length === 0 && split !== background)
    {
      // Remove from array
      delete childSplits[split.objectName]

      // Remove from parent split
      _removeFromSplits(split);

      // Destroy
      split.destroy();
    }
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

      /**
       * Notifies that its minimum size has changed.
       */
      signal minimumSizeChanged();

      /**
       * Callback when the layout's minimum width changes.
       */
      Layout.onMinimumWidthChanged: {
        minimumSizeChanged();
      }

      /**
       * Callback when the layout's minimum height changes.
       */
      Layout.onMinimumHeightChanged: {
        minimumSizeChanged();
      }

      /**
       * Callback when the children array has been changed.
       */
      onChildrenChanged: {
        if (children.length === 0)
          return;

        // Propagate child's minimum size changes to the item.
        Layout.minimumWidth = Qt.binding(function() {
          return children[0].Layout.minimumWidth
        });
        Layout.minimumHeight = Qt.binding(function() {
          return children[0].Layout.minimumHeight
        });
      }
    }
  }

  /**
   * Component for creating new splits
   */
  Component {
    id: newSplit

    SplitView {
      Layout.minimumWidth: 100
      Layout.minimumHeight: 100

      /**
       * Iterate over all current child items and update the split's minimum
       * width accordingly.
       */
      function recalculateMinimumSize()
      {
        // Sync minimum sizes
        for (var i = 0; i < __items.length; i++)
        {
          var child = __items[i];

          if (child.Layout.minimumWidth > Layout.minimumWidth)
          {
            Layout.minimumWidth = child.Layout.minimumWidth;
          }
        }
      }
    }
  }
}

