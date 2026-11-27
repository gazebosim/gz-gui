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
import QtQuick.Controls.Material 2.2
import QtQuick.Controls.Material.impl 2.2
import QtQuick.Layouts 1.3

Popup {
  id: pluginMenu
  padding: 0

  Connections {
    target: _MainWindow
    function onConfigChanged() {
      filteredModel.model = _MainWindow.PluginListModel()
    }
  }

  /**
   * Color for search bar
   */
  property color searchColor: (Material.theme == Material.Light) ?
      Material.color(Material.Grey, Material.Shade200):
      Material.color(Material.Grey, Material.Shade900);


  onOpened: {
    searchField.forceActiveFocus()
    searchField.selectAll()
  }

  ColumnLayout {
    anchors.fill: parent
    spacing: 0

    Rectangle {
      id: searchSortBar
      color: searchColor
      Layout.preferredHeight: 50
      Layout.fillWidth: true
      RowLayout {
        id: rowLayout
        anchors.fill: parent
        spacing: 0
        Rectangle {
          color: "transparent"
          Layout.preferredHeight: 25
          Layout.preferredWidth: 25
          Layout.leftMargin: 5
          Image {
            id: searchIcon
            source: "images/search.svg"
            anchors.verticalCenter: parent.verticalCenter
          }
        }
        TextField {
          id: searchField
          Layout.fillHeight: true
          Layout.fillWidth: true
          Layout.rightMargin: 5
          selectByMouse: true
          onTextEdited: {
            filteredModel.update();
          }
          Keys.onReturnPressed: {
            _MainWindow.OnAddPlugin(
              pluginMenuListView.currentItem.pluginModel.modelData);
            drawer.close();
            pluginMenu.close();
          }
          Keys.onDownPressed: {
            pluginMenuListView.incrementCurrentIndex();
          }
          Keys.onUpPressed: {
            pluginMenuListView.decrementCurrentIndex();
          }
        }
      }
    }

    ListView {
      id: pluginMenuListView
      Layout.fillHeight: true
      Layout.fillWidth: true
      clip: true

      model: filteredModel

      ScrollBar.vertical: ScrollBar {
        active: true
        width: 8
        policy: ScrollBar.AlwaysOn
      }
    }
  }

  GzSortFilterModel { // qmllint disable required
    id: filteredModel

    lessThan: function(left, right) {
      var leftStr = left.modelData.toLowerCase();
      var rightStr = right.modelData.toLowerCase();
      return leftStr < rightStr;
    }

    filterAcceptsItem: function(item) {
      var itemStr = item.modelData.toLowerCase();
      var filterStr = searchField.text.toLowerCase();
      return itemStr.includes(filterStr);
    }

    model: _MainWindow.PluginListModel()

    delegate: ItemDelegate {
      required property var model
      required property string modelData
      property variant pluginModel: model
      width: pluginMenu.width
      text: modelData
      highlighted: ListView.isCurrentItem
      onClicked: {
        _MainWindow.OnAddPlugin(modelData);
        drawer.close()
        pluginMenu.close()
      }
    }
  }
}
