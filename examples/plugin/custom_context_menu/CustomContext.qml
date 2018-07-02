import QtQuick 2.0
import QtQuick.Controls 2.0

Rectangle {
  id: customContext
  color: "#ff8888"
  height: 300
  width: 300

  MouseArea {
    anchors.fill: parent
    acceptedButtons: Qt.RightButton
    onClicked: {
      customMenu.x = mouseX
      customMenu.y = mouseY
      customMenu.open()
    }
  }

  Menu {
    id: customMenu
    transformOrigin: Menu.TopRight
    MenuItem {
      text: "Custom option"
      onTriggered: console.log("Custom option triggered");
    }
    MenuItem {
      text: "Settings (from original menu)"
      onTriggered: customContext.parent.card().showSettingsDialog();
    }
    MenuItem {
      text: "Close"
      onTriggered: customContext.parent.card().close();
    }
  }
}
