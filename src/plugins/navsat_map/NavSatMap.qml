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
import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.1
import QtLocation 5.6
import QtPositioning 5.6

Item {
  id: navSatMap
  property double latitude: 0.0
  property double longitude: 0.0
  property bool centering: true

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
          NavSatMap.OnRefresh();
        }
      }

      ComboBox {
        id: combo
        Layout.fillWidth: true
        model: NavSatMap.topicList
        onCurrentIndexChanged: {
          if (currentIndex < 0) {
            return;
          }

          NavSatMap.OnTopic(textAt(currentIndex));
        }
        ToolTip.visible: hovered
        ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
        ToolTip.text: qsTr("Ignition Transport topics publishing NavSat messages")
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

  Plugin {
    id: mapPlugin
    name: "osm"
  }

  Map {
    id: map
    anchors.top: configColumn.bottom
    anchors.bottom: parent.bottom
    anchors.left: parent.left
    anchors.right: parent.right

    anchors.topMargin: 5
    anchors.leftMargin: -5
    anchors.rightMargin: -5

    MapCircle {
      id: circle
      center {
        latitude: latitude
        longitude: longitude
      }
      radius: 23.0
      color: '#4285f4'
      border.width: 3
      border.color: "white"
    }

    plugin: mapPlugin
    center: centering ? QtPositioning.coordinate(latitude, longitude) : center
    copyrightsVisible: false
    zoomLevel: 16

    gesture.onPanStarted: {
      centering = false
    }

    onZoomLevelChanged: {
      // Scaling the location marker according to zoom level.
      // Marker radius = meters per pixel * 10
      circle.radius = 156543.03392 * Math.cos(latitude * Math.PI / 180) / Math.pow(2, map.zoomLevel) * 10
    }
  }

  RoundButton {
    height: 40
    width: 40
    anchors.top: map.top
    anchors.right: map.right

    // Strangely, \u2316 doesn't work on Bionic
    text: "\u2732"
    font.pixelSize: 25

    Material.background: Material.primary
    Material.foreground: centering ? "white" : "black"

    onClicked: {
      centering = true
    }

    hoverEnabled: true

    ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
    ToolTip.visible: hovered
    ToolTip.text: qsTr("Keep point on center")
  }

  Connections {
    target: NavSatMap
    onNewMessage: {
      latitude = _latitudeDeg
      longitude = _longitudeDeg
    }
  }
}
