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
   * Add a plugin
   * @param type:string _pluginFile Plugin Qml file.
   */
  function addPlugin(_pluginFile) {
    cardComponent.createObject(background, {
     "sourceFile": _pluginFile,
     "x": background.width / 4,
     "y": background.height / 4,
     "width": background.width / 2,
     "height": background.width / 2
    })
  }

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
    id: background
    anchors.fill: parent
    color: bgColor

    Label {
      text: "Insert plugins to start!"
      anchors.margins: 20
      anchors.left: parent.left
      anchors.right: parent.right
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
        text: model.title
        highlighted: ListView.isCurrentItem
        onClicked: {
          listView.currentIndex = index
          addPlugin(model.source);
          drawer.close()
        }
      }

      model: ListModel {
        ListElement { title: "ExamplePlugin"; source: "qrc:qml/ExamplePlugin.qml" }
        ListElement { title: "Publisher"; source: "qrc:Publisher/Publisher.qml" }
      }

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

  /**
   * Resizable card which contains a plugin
   */
  Component {
    id: cardComponent

    Pane {
      id: cardPane
      Material.elevation: 6

      property int rulersThickness: 25
      property int minSize: 100
      property var sourceFile: ""

      Loader {
        anchors.fill: parent
        source: sourceFile;
      }

      MouseArea {
        anchors.fill: parent
        drag{
          target: parent.parent
          minimumX: 0
          minimumY: 0
          maximumX: parent.parent.parent.width - parent.parent.width
          maximumY: parent.parent.parent.height - parent.parent.height
          smoothed: true
        }
      }

      // Left ruler
      Rectangle {
        width: rulersThickness
        height: parent.parent.height
        color: "transparent"
        anchors.horizontalCenter: parent.left
        anchors.verticalCenter: parent.verticalCenter

        MouseArea {
          anchors.fill: parent
          cursorShape: Qt.SplitHCursor
          drag{ target: parent; axis: Drag.XAxis }
          onMouseXChanged: {
            if(drag.active){
              cardPane.width = cardPane.width - mouseX
              cardPane.x = cardPane.x + mouseX
              if(cardPane.width < minSize)
                cardPane.width = minSize
            }
          }
        }
      }

      // Right ruler
      Rectangle {
        width: rulersThickness
        height: parent.parent.height
        color: "transparent"
        anchors.horizontalCenter: parent.right
        anchors.verticalCenter: parent.verticalCenter

        MouseArea {
          anchors.fill: parent
          cursorShape: Qt.SplitHCursor
          drag{ target: parent; axis: Drag.XAxis }
          onMouseXChanged: {
            if(drag.active){
              cardPane.width = cardPane.width + mouseX
              if(cardPane.width < minSize)
                  cardPane.width = minSize
            }
          }
        }
      }

      // Top ruler
      Rectangle {
        width: parent.parent.width
        height: rulersThickness
        x: parent.x / 2
        y: 0
        color: "transparent"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.top

        MouseArea {
          anchors.fill: parent
          cursorShape: Qt.SplitVCursor
          drag{ target: parent; axis: Drag.YAxis }
          onMouseYChanged: {
            if(drag.active){
              cardPane.height = cardPane.height - mouseY
              cardPane.y = cardPane.y + mouseY
              if(cardPane.height < minSize)
                cardPane.height = minSize
            }
          }
        }
      }

      // Bottom ruler
      Rectangle {
        width: parent.parent.width
        height: rulersThickness
        x: parent.x / 2
        y: parent.y
        color: "transparent"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.bottom

        MouseArea {
          anchors.fill: parent
          cursorShape: Qt.SplitVCursor
          drag{ target: parent; axis: Drag.YAxis }
          onMouseYChanged: {
            if(drag.active){
              cardPane.height = cardPane.height + mouseY
              if(cardPane.height < minSize)
                cardPane.height = minSize
            }
          }
        }
      }
    }
  }
}
