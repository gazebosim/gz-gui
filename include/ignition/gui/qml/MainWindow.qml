import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.1

ApplicationWindow
{
  title: qsTr("Ignition GUI")
  width: 640
  height: 480
  visible: true
  id: window
  property var bgColor: "#eeeeee"

  /**
   * Top toolbar
   */
  header: ToolBar {
    Material.foreground: "white"

    RowLayout {
      spacing: 20
      anchors.fill: parent

      ToolButton {
        contentItem: Image {
          fillMode: Image.Pad
          horizontalAlignment: Image.AlignHCenter
          verticalAlignment: Image.AlignVCenter
          source: "images/drawer.png"
        }
        onClicked: drawer.open()
      }

      Label {
        id: titleLabel
        text: "Ignition GUI"
        font.pixelSize: 20
        elide: Label.ElideRight
        horizontalAlignment: Qt.AlignHCenter
        verticalAlignment: Qt.AlignVCenter
        Layout.fillWidth: true
      }

      ToolButton {
        contentItem: Image {
          fillMode: Image.Pad
          horizontalAlignment: Image.AlignHCenter
          verticalAlignment: Image.AlignVCenter
          source: "images/menu.png"
        }
        onClicked: optionsMenu.open()

        Menu {
          id: optionsMenu
          x: parent.width - width
          transformOrigin: Menu.TopRight
          MenuItem {
            text: "About"
            onTriggered: aboutDialog.open()
          }
        }
      }
    }
  }

  /**
   * Background
   */
  Rectangle {
    objectName: "background"
    id: background
    anchors.fill: parent
    color: bgColor

    Label {
      id: startLabel;
      visible: MainWindow.pluginCount === 0
      text: "Insert plugins to start!"
      anchors.fill: parent
      font.pointSize: 24
      horizontalAlignment: Label.AlignHCenter
      verticalAlignment: Label.AlignVCenter
      wrapMode: Label.Wrap
    }
  }

  /**
   * Left menu
   */
  Drawer {
    id: drawer
    width: Math.min(window.width, window.height) / 3 * 2
    height: window.height

    ListView {
      id: listView

      focus: true
      currentIndex: -1
      anchors.fill: parent

      delegate: ItemDelegate {
        width: parent.width
        text: modelData
        highlighted: ListView.isCurrentItem
        onClicked: {
          listView.currentIndex = index
          MainWindow.OnAddPlugin(modelData);
          drawer.close()
        }
      }

      model: MainWindow.PluginListModel()

      ScrollIndicator.vertical: ScrollIndicator { }
    }
  }

  /**
   * About dialog
   */
  Dialog {
    id: aboutDialog
    modal: true
    focus: true
    title: "Ignition GUI"
    x: (window.width - width) / 2
    y: window.height / 6
    width: Math.min(window.width, window.height) / 3 * 2
    contentHeight: aboutColumn.height

    Column {
      id: aboutColumn
      spacing: 20

      Label {
        width: aboutDialog.availableWidth
        text: "Gorgeous robotic interfaces since 2018."
        wrapMode: Label.Wrap
        font.pixelSize: 12
      }
    }
  }
}
