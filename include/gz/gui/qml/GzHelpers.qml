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

Item {
  visible: false
  /**
   * Helper function to get an item's ancestor by name.
   * @param _item Item whose parent we're looking for.
   * @param _name Name to look for, accepts regex.
   * @returns The ancestor, or undefined if not found.
   */
  function ancestorByName(_item, _name)
  {
    if (_item === undefined)
      return undefined;

    var result = _item.parent;
    while (result)
    {
      if (result.objectName.match(_name) !== null)
        break;

      result = result.parent;
    }

    return result;
  }

  /**
   * Helper function to get number of decimal digits based on a width value.
   * @param _width Pixel width.
   * @returns Number of decimals that fit with the provided width.
   */
  function getDecimals(_width) {
    // Use full decimals if the width is <= 0, which allows the value
    // to appear correctly.
    if (_width <= 0 || _width > 110)
      return 6

    if (_width <= 80)
      return 2

    return 4
  }
  function dump(object, indent, depth) {
    console.log(indent + object)
    if (depth > 0) {
      for (const i in object.children)
        dump(object.children[i], indent + "    ", depth - 1)
    }
  }

}
