/*
 * Copyright (C) 2021 Open Source Robotics Foundation
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

// Borrowed from
// https://martin.rpdev.net/2019/01/15/using-delegatemodel-in-qml-for-sorting-and-filtering.html

import QtQuick
import QtQml.Models

DelegateModel {
  /**
   * Used by sorting. Override it to create custom sort behaviour.
   */
  property var lessThan: function(_left, _right)
      {
        return true;
      }

  /**
   * Override this function to define what items should be accepted or not.
   */
  property var filterAcceptsItem: function(_item)
      {
        return true;
      }

  /**
   * Update the item list
   */
  function update() {
    if (items.count > 0) {
      items.setGroups(0, items.count, "items");
    }

    // Step 1: Filter items
    var visible = [];
    for (var i = 0; i < items.count; ++i) {
      var item = items.get(i);
      if (filterAcceptsItem(item.model)) {
        visible.push(item);
      }
    }

    // Step 2: Sort the list of visible items
    visible.sort(function(_a, _b) {
        return lessThan(_a.model, _b.model) ? -1 : 1;
    });

    // Step 3: Add all items to the visible group:
    for (i = 0; i < visible.length; ++i) {
      item = visible[i];
      item.inVisible = true;
      if (item.visibleIndex !== i) {
        visibleItems.move(item.visibleIndex, i, 1);
      }
    }
  }

  items.onChanged: update()
  onLessThanChanged: update()
  onFilterAcceptsItemChanged: update()

  groups: DelegateModelGroup {
    id: visibleItems

    name: "visible"
    includeByDefault: false
  }

  filterOnGroup: "visible"
}
