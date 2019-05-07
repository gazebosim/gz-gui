import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.1

/**
 * Side menu
 */
Drawer {

  Rectangle {
    objectName: "sideDrawer"
    id: sideDrawer
    anchors.fill: parent
    color: Material.background

    function closeDrawer() {
      drawer.close();
    }

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
      visible: MainWindow.showDefaultDrawerOpts

      delegate: ItemDelegate {
        width: parent.width
        text: title
        highlighted: ListView.isCurrentItem
        onClicked: {
          sideDrawer.onAction(action)
          sideDrawer.closeDrawer();
        }
      }

      model: drawerModel

      ScrollIndicator.vertical: ScrollIndicator { }
    }
  }
}
