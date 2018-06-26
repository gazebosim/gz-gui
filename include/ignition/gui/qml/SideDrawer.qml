import QtQuick 2.9
import QtQuick.Controls 2.2

/**
 * Side menu
 */
Drawer {

  function onAction(action) {
    switch(action) {
      case "loadConfig":
        loadConfig()
        break
      case "saveConfig":
        saveConfig()
        break
      case "saveConfigAs":
        saveConfigAs()
        break
      case "styleSettings":
        styleDialog.open()
        break
      case "aboutDialog":
        aboutDialog.open()
        break
      case "close":
        window.close()
        break
      default:
        break
    }
  }

  Rectangle {
    objectName: "sideDrawer"
    id: sideDrawer
    anchors.fill: parent
    property var sDialog: styleDialog

    ListModel {
      id: drawerModel

      ListElement {
        title: "Load configuration"
        action: "loadConfig"
      }
      ListElement {
        title: "Save configuration"
        action: "saveConfig"
      }
      ListElement {
        title: "Save configuration as"
        action: "saveConfigAs"
      }
      ListElement {
        title: "Style settings"
        action: "styleSettings"
      }
      ListElement {
        title: "About"
        action: "aboutDialog"
      }
      ListElement {
        title: "Quit"
        action: "close"
      }
    }

    ListView {
      id: listView
      anchors.fill: parent
      visible: MainWindow.showDefaultPanelOpts

      delegate: ItemDelegate {
        width: parent.width
        text: title
        highlighted: ListView.isCurrentItem
        onClicked: {
          onAction(action)
          drawer.close()
        }
      }

      model: drawerModel

      ScrollIndicator.vertical: ScrollIndicator { }
    }
  }
}
