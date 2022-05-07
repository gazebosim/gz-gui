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

Item {
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
}
