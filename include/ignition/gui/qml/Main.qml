import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.1
import QtQuick.Dialogs 1.0
import QtQuick.Layouts 1.3
import "qrc:/qml"

ApplicationWindow
{
  title: qsTr("Ignition GUI")
  width: 1200
  height: 1000
  visible: true
  id: window
  property string bgColor: "#eeeeee"

  // Not sure why the binding doesn't take care of this
  onTitleChanged: {
    titleLabel.text = window.title
  }

  // C++ signals to QML slots
  Connections {
    target: MainWindow
    onNotify: {
      notificationText.text = _message
      notificationDialog.open()
    }
  }

  /**
   * Load a configuration file
   */
  function loadConfig() {
    loadFileDialog.open()
  }

  /**
   * Save a configuration file
   */
  function saveConfig() {
    MainWindow.OnSaveConfig()
  }

  /**
   * Save a configuration file to a given file
   */
  function saveConfigAs() {
    saveFileDialog.open()
  }

  // Shortcuts (why not working on menu?)
  Shortcut {
    sequence: "Ctrl+O"
    onActivated: loadConfig()
  }

  Shortcut {
    sequence: "Ctrl+S"
    onActivated: saveConfig()
  }

  Shortcut {
    sequence: "Ctrl+Shift+S"
    onActivated: saveConfigAs()
  }

  Shortcut {
    sequence: "Ctrl+Q"
    onActivated: close()
  }

  /**
   * Top toolbar
   */
  header: ToolBar {
    Material.foreground: "white"
    Material.elevation: 0

    MouseArea {
      anchors.fill: parent;
      property variant clickPos: "1,1"
      onPressed: {
        clickPos  = Qt.point(mouse.x,mouse.y)
      }
      onPositionChanged: {
        var delta = Qt.point(mouse.x-clickPos.x, mouse.y-clickPos.y)
        window.x += delta.x;
        window.y += delta.y;
      }
      onDoubleClicked: {
        window.showMaximized()
      }
    }

    RowLayout {
      spacing: 20
      anchors.fill: parent

      ToolButton {
        highlighted: true
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
        text: window.title
        font.pixelSize: 18
        elide: Label.ElideRight
        horizontalAlignment: Qt.AlignHLeft
        verticalAlignment: Qt.AlignVCenter
        Layout.fillWidth: true
      }

      ToolButton {
        highlighted: true
        contentItem: Image {
          fillMode: Image.Pad
          horizontalAlignment: Image.AlignHCenter
          verticalAlignment: Image.AlignVCenter
          source: "images/menu.png"
        }
        onClicked: pluginMenu.open()

        PluginMenu {
          id: pluginMenu
          x: parent.width - width
          transformOrigin: Menu.TopRight
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
  SideDrawer {
    id: drawer
    width: Math.min(window.width * 0.3, 500)
    height: window.height
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
   * Load file dialog
   */
  FileDialog {
    id: loadFileDialog
    title: "Load configuration"
    folder: shortcuts.home
    nameFilters: [ "Config files (*.config)" ]
    selectMultiple: false
    selectExisting: true
    onAccepted: {
      MainWindow.OnLoadConfig(fileUrl)
    }
  }

  /**
   * Save file dialog
   */
  FileDialog {
    id: saveFileDialog
    title: "Save configuration"
    folder: shortcuts.home
    nameFilters: [ "Config files (*.config)" ]
    selectMultiple: false
    selectExisting: false
    onAccepted: {
      MainWindow.OnSaveConfigAs(fileUrl)
    }
  }

  /**
   * TODO: change to a snackbar / toast
   */
  Dialog {
    id: notificationDialog
    modal: true
    focus: true
    x: (window.width - width) / 2
    y: window.height / 6
    width: Math.min(window.width, window.height) / 3 * 2
    contentHeight: notificationColumn.height

    Column {
      id: notificationColumn
      spacing: 20

      Label {
        id: notificationText
        width: notificationDialog.availableWidth
        wrapMode: Label.Wrap
        font.pixelSize: 18
      }
    }
  }
}
