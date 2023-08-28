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
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.1
import QtQuick.Dialogs 1.0
import QtQuick.Layouts 1.3
import ExitAction 1.0
import "qrc:/qml"

ApplicationWindow
{
  title: qsTr("Gazebo GUI")
  width: 1200
  height: 1000
  minimumWidth: 300
  minimumHeight: 300
  visible: true
  id: window
  objectName: "window"
  font.family: "Roboto"

  // Expose material properties to C++
  property string materialTheme: window.Material.theme
  property string materialPrimary: window.Material.primary
  property string materialAccent: window.Material.accent
  property string toolBarColorLight: MainWindow.toolBarColorLight
  property string toolBarTextColorLight: MainWindow.toolBarTextColorLight
  property string toolBarColorDark: MainWindow.toolBarColorDark
  property string toolBarTextColorDark: MainWindow.toolBarTextColorDark
  property string pluginToolBarColorLight: MainWindow.pluginToolBarColorLight
  property string pluginToolBarTextColorLight: MainWindow.pluginToolBarTextColorLight
  property string pluginToolBarColorDark: MainWindow.pluginToolBarColorDark
  property string pluginToolBarTextColorDark: MainWindow.pluginToolBarTextColorDark
  // Expose config properties to C++
  property int defaultExitAction: MainWindow.defaultExitAction
  property bool showDialogOnExit: MainWindow.showDialogOnExit
  property string dialogOnExitText: MainWindow.dialogOnExitText
  property bool exitDialogShowShutdown: MainWindow.exitDialogShowShutdown
  property bool exitDialogShowCloseGui: MainWindow.exitDialogShowCloseGui
  property string exitDialogShutdownText: MainWindow.exitDialogShutdownText
  property string exitDialogCloseGuiText: MainWindow.exitDialogCloseGuiText
  /**
   * Flag to indicate if the close event was triggered by the close dialog.
   */
  property bool closingFromDialog: false

  /**
   * Tool bar background color
   */
  property string toolBarColor:
    MainWindow.toolBarColorLight === "" ||
    MainWindow.toolBarColorDark === "" ?
    Material.primary :
    (Material.theme === Material.Light) ?
    MainWindow.toolBarColorLight : MainWindow.toolBarColorDark

  /**
   * Tool bar text color
   */
  property string toolBarTextColor:
    MainWindow.toolBarTextColorLight === "" ||
    MainWindow.toolBarTextColorDark === "" ?
    Material.background :
    (Material.theme === Material.Light) ?
    MainWindow.toolBarTextColorLight : MainWindow.toolBarTextColorDark

  // Not sure why the binding doesn't take care of this
  onTitleChanged: {
    titleLabel.text = window.title
  }

  // Handler for window closing
  onClosing: {
    close.accepted = !showDialogOnExit
    if(showDialogOnExit){
      if (closingFromDialog) {
        close.accepted = true;
      } else {
        confirmationDialogOnExit.open()
      }
    } else if (defaultExitAction == ExitAction.SHUTDOWN_SERVER) {
      MainWindow.OnStopServer()
    }
  }

  // C++ signals to QML slots
  Connections {
    target: MainWindow
    function onNotify(_message) {
      notificationDialog.setTextDuration(_message, 0)
    }
    function onNotifyWithDuration(_message, _duration) {
      notificationDialog.setTextDuration(_message, _duration)
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

  Shortcut {
    sequence: "/"
    onActivated: pluginMenu.open()
  }

  /**
   * Top toolbar
   */
  header: ToolBar {
    Material.background: toolBarColor
    Material.foreground: Material.foreground

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
        visible: MainWindow.showDrawer
        contentItem: Image {
          fillMode: Image.Pad
          horizontalAlignment: Image.AlignHCenter
          verticalAlignment: Image.AlignVCenter
          source: "images/drawer.png"
        }
        onClicked: drawer.open()
      }

      // Padding for title
      Rectangle {
        height: 1
        width: 1
        visible: !MainWindow.showDrawer
        color: "transparent"
      }

      Label {
        id: titleLabel
        text: window.title
        font.pixelSize: 18
        color: toolBarTextColor
        elide: Label.ElideRight
        horizontalAlignment: Qt.AlignHLeft
        verticalAlignment: Qt.AlignVCenter
        Layout.fillWidth: true
      }

      ToolButton {
        highlighted: true
        visible: MainWindow.showPluginMenu
        contentItem: Image {
          fillMode: Image.Pad
          horizontalAlignment: Image.AlignHCenter
          verticalAlignment: Image.AlignVCenter
          source: "images/menu.png"
        }
        onClicked: pluginMenu.open()

        PluginMenu {
          id: pluginMenu
          width: 200
          x: parent.width - width
          height: window.height * 0.3
          transformOrigin: Popup.TopRight
        }
      }
    }
  }

  /**
   * Background
   */
  IgnSplit {
    anchors.fill: parent
  }

  /**
   * Left menu
   */
  SideDrawer {
    id: drawer
    interactive: MainWindow.showDrawer
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
    title: "Gazebo GUI"
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
   * Style dialog
   */
  StyleDialog {
    id: styleDialog
    x: (window.width - width) / 2
    y: window.height / 6
    width: Math.min(window.width, window.height) * 0.5
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
      var selected = fileUrl.toString();

      if (!selected.endsWith(".config"))
      {
        selected += ".config";
      }

      MainWindow.OnSaveConfigAs(selected);
    }
  }

  IgnSnackBar {
    id: notificationDialog
  }

  Timer {
    id: timer
  }

  /**
   *  Confirmation dialog on close button
   */
  Dialog {
    id: confirmationDialogOnExit
    title: (dialogOnExitText ? dialogOnExitText : "Do you really want to exit?")
    objectName: "confirmationDialogOnExit"

    modal: true
    focus: true
    parent: ApplicationWindow.overlay
    width: 500
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
    closePolicy: Popup.CloseOnEscape
    standardButtons:
      (exitDialogShowCloseGui ? Dialog.Ok : Dialog.NoButton) |
      (exitDialogShowShutdown ? Dialog.Discard : Dialog.NoButton) |
      Dialog.Cancel

    // The button texts need to be changed later than in onCompleted as standardButtons change later
    onAboutToShow: function () {
      if (exitDialogShowCloseGui && exitDialogCloseGuiText)
        footer.standardButton(Dialog.Ok).text = exitDialogCloseGuiText
      if (exitDialogShowShutdown)
        footer.standardButton(Dialog.Discard).text =
          (exitDialogShutdownText ? exitDialogShutdownText : "Shutdown server and GUI")
    }

    footer:
      DialogButtonBox {
        onClicked: function (btn) {
          if (btn == this.standardButton(Dialog.Ok)) {
            closingFromDialog = true;
            window.close();
          }
          else if (btn == this.standardButton(Dialog.Discard)) {
            MainWindow.OnStopServer()
            // if GUI and server run in the same process, give server opportunity to kill the GUI
            timer.interval = 100;
            timer.repeat = false;
            timer.triggered.connect(function() {
              closingFromDialog = true;
              window.close();
            });
            timer.start();
          }
        }
      }
  }
}
