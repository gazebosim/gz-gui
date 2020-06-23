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
 * Custom drawer
 */
Rectangle {
  id: customDrawer
  anchors.fill: parent

  /**
   * Callback for list items
   */
  function onAction(_action) {
    switch(_action) {
      // Handle custom actions
      case "cppActionFromQml":
        CustomActions.cppActionFromQml()
        break
      // Forward others to default drawer
      default:
        parent.onAction(_action);
        break
    }
  }

  ListModel {
    id: drawerModel

    // Custom action which calls custom C++ code
    ListElement {
      title: "Call C++ action"
      actionElement: "cppActionFromQml"
    }

    // Actions provided by Ignition GUI, with custom titles
    ListElement {
      title: "Call default action (Style)"
      actionElement: "styleSettings"
    }

    ListElement {
      title: "Call default action (Quit)"
      actionElement: "close"
    }
  }

  ListView {
    id: listView
    anchors.fill: parent

    delegate: ItemDelegate {
      // TODO(anyone): follow the application's style
      Material.theme: Material.theme
      width: parent.width
      text: title
      highlighted: ListView.isCurrentItem
      onClicked: {
        customDrawer.onAction(actionElement);
        customDrawer.parent.closeDrawer();
      }
    }

    model: drawerModel

    ScrollIndicator.vertical: ScrollIndicator { }
  }
}
