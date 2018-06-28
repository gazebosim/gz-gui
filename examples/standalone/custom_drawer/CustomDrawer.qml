import QtQuick 2.9
import QtQuick.Controls 2.2

/**
 * Custom drawer
 */
Rectangle {
  anchors.fill: parent

  /**
   * Callback for list items
   */
  function onAction(_action) {
    switch(_action) {
      case "cppActionFromQml":
        CustomActions.cppActionFromQml()
        break
      default:
        parent.onAction(_action);
        break
    }
  }

  ListModel {
    id: drawerModel

    ListElement {
      title: "Call C++ action"
      action: "cppActionFromQml"
    }

    ListElement {
      title: "Call default action (Style)"
      action: "styleSettings"
    }

    ListElement {
      title: "Call default action (Quit)"
      action: "close"
    }
  }

  ListView {
    id: listView
    anchors.fill: parent

    delegate: ItemDelegate {
      width: parent.width
      text: title
      highlighted: ListView.isCurrentItem
      onClicked: {
        onAction(action)
        // drawer.close()
      }
    }

    model: drawerModel

    ScrollIndicator.vertical: ScrollIndicator { }
  }
}
