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
