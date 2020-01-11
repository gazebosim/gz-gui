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
