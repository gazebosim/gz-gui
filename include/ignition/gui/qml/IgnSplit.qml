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
      split.split.orientation = Qt.Vertical;

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
    if (_split === background)
    {
      _split.addItem(item);
    }
    else
    {
      _split.split.addItem(item);

      // Make sure that changes to the item's minimum size get propagated to the
      // split.
      item.minimumSizeChanged.connect(function(){
        _split.split.recalculateMinimumSize()
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
    var splitWrapper = newSplit.createObject(_parentSplit);

    // Unique name
    var splitName = "split_" + Math.floor(Math.random() * 100000)
    splitWrapper.objectName = splitName;

    // Add to dictionary
    childSplits[splitName] = splitWrapper;

    // Add to parent
    _parentSplit.addItem(splitWrapper);

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

    if (!split)
    {
      console.error("Failed to find parent split for [", _item.objectName, "]")
      return;
    }

    if (split === background)
    {
      split.removeItem(_item);
    }
    else
    {
      split.split.removeItem(_item);

      // If split is now empty, remove split
      if (split.split.__items.length === 0)
      {
        // Remove from array
        delete childSplits[split.objectName]

        // Remove from parent split
        _removeFromSplits(split);

        // Destroy
        split.destroy();
      }
      else
      {
        split.split.recalculateMinimumSize();
      }
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
      color: Material.background

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

    /**
     * For some reason, the scroll view doesn't work well within a split view,
     * so we wrap it in a rectangle.
    */
    Rectangle {
      id: splitWrapper

      /**
       * Expose the split view.
       */
      property var split: split

      Layout.minimumWidth: split.Layout.minimumWidth
      Layout.minimumHeight: split.Layout.minimumHeight

      ScrollView {
        contentHeight: split.height
        contentWidth: split.width

        // TODO(louise) This only works for a very specific split
        height: window.height - window.header.height

        SplitView {
          id: split
          width: splitWrapper.width
          height: Math.max(childItems[Object.keys(childItems)[0]].height,
                      split.Layout.minimumHeight)

          /**
           * Iterate over all current child items and update the split's minimum
           * width accordingly.
           */
          function recalculateMinimumSize()
          {
            // TODO(louise): generalize to support horizontal splits
            if (orientation === Qt.Horizontal)
            {
              return;
            }

            // Sync minimum sizes
            var heightSum = 0;
            for (var i = 0; i < __items.length; i++)
            {
              var child = __items[i];

              // Minimum width matches the largest minimum width among children
              if (child.Layout.minimumWidth > Layout.minimumWidth)
              {
                Layout.minimumWidth = child.Layout.minimumWidth;
              }

              // Minimum height is the sum of all children's minimum heights
              heightSum += child.Layout.minimumHeight;
            }
            Layout.minimumHeight = heightSum;
          }
        }
      }
    }
  }

  /**
   * Global key event handler. Propagates key event to IgnCard items until
   * the event is accepted.
   * \todo(anyone) Use FocusScope instead of a global handler?
   */
  function handleKeyEvent(event, type) {
    for (var key in childItems) {
      var obj = childItems[key];
      obj.children[0].focus = true
      if (type == "pressed")
        obj.children[0].Keys.pressed(event)
      else if (type == "released")
        obj.children[0].Keys.released(event)
      if (event.accepted)
        break
    }
  }

  focus: true
  Keys.onPressed: {
    handleKeyEvent(event, "pressed")
  }
  Keys.onReleased: {
    handleKeyEvent(event, "released")
  }

}

