/*
 * Copyright (C) 2021 Open Source Robotics Foundation
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
import QtQuick
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.1

Item {
  id: navSatMap
  property double latitude: 0.0
  property double longitude: 0.0

  Layout.minimumWidth: 280
  Layout.minimumHeight: 455
  anchors.topMargin: 5
  anchors.leftMargin: 5
  anchors.rightMargin: 5
  anchors.fill: parent

  ColumnLayout {
    id: configColumn
    width: parent.width

    RowLayout {
      width: parent.width

      Layout.fillWidth: true
      Layout.fillHeight: true

      RoundButton {
        text: "\u21bb"
        Material.background: Material.primary
        onClicked: {
          _NavSatMap.OnRefresh();
        }
      }

      ComboBox {
        id: combo
        Layout.fillWidth: true
        model: _NavSatMap.topicList
        onCurrentIndexChanged: {
          if (currentIndex < 0) {
            return;
          }

          _NavSatMap.OnTopic(textAt(currentIndex));
        }
        ToolTip.visible: hovered
        ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
        ToolTip.text: qsTr("Gazebo Transport topics publishing NavSat messages")
      }
    }

    RowLayout {
      width: parent.width
      Item {
        height: 1
        Layout.fillWidth: true
      }
      Text {
        text: "<b>Latitude: </b>" + latitude.toFixed(6)
      }

      Text {
        text: "<b>Longitude: </b>" + longitude.toFixed(6)
      }
      Item {
        height: 1
        Layout.fillWidth: true
      }
    }
  }

  // The map is in its own file so that, if the QtLocation module is missing
  // (Qt older than 6.5, or not installed), only the map is disabled.
  Loader {
    id: mapLoader
    anchors.top: configColumn.bottom
    anchors.bottom: parent.bottom
    anchors.left: parent.left
    anchors.right: parent.right

    anchors.topMargin: 5
    anchors.leftMargin: -5
    anchors.rightMargin: -5

    source: "NavSatMapView.qml"
  }

  Binding {
    target: mapLoader.item
    property: "latitude"
    value: navSatMap.latitude
    when: mapLoader.status === Loader.Ready
  }

  Binding {
    target: mapLoader.item
    property: "longitude"
    value: navSatMap.longitude
    when: mapLoader.status === Loader.Ready
  }

  Label {
    anchors.fill: mapLoader
    anchors.margins: 10
    visible: mapLoader.status === Loader.Error
    wrapMode: Text.WordWrap
    horizontalAlignment: Text.AlignHCenter
    verticalAlignment: Text.AlignVCenter
    text: qsTr("Map not available: it needs the QtLocation QML module (Qt 6.5 or newer), which is not installed.")
  }

  Connections {
    target: _NavSatMap
    function onNewMessage(_latitudeDeg, _longitudeDeg) {
      latitude = _latitudeDeg
      longitude = _longitudeDeg
    }
  }
}
