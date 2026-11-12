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
import QtQuick.Controls.Material 2.1
import QtLocation
import QtPositioning

// Map of the NavSatMap plugin, loaded by NavSatMap.qml. It is a separate file
// so that a missing QtLocation module only disables the map.
Item {
  id: mapView

  // Position to show, in degrees. Set by NavSatMap.qml.
  property double latitude: 0.0
  property double longitude: 0.0

  // True to keep the map centered on the position. Dragging the map clears
  // it and the "Track position" button sets it again.
  property bool centering: true

  Plugin {
    id: mapPlugin
    name: "osm"
  }

  Map {
    id: map
    anchors.fill: parent

    // Map coordinate under the pinch centroid when the pinch started
    property geoCoordinate startCentroid

    MapCircle {
      id: circle
      center {
        latitude: mapView.latitude
        longitude: mapView.longitude
      }
      radius: 23.0
      color: '#4285f4'
      border.width: 3
      border.color: "white"
    }

    plugin: mapPlugin
    center: mapView.centering ?
        QtPositioning.coordinate(mapView.latitude, mapView.longitude) : center
    copyrightsVisible: false
    zoomLevel: 16

    PinchHandler {
        id: pinch
        target: null
        onActiveChanged: if (active) {
            map.startCentroid = map.toCoordinate(pinch.centroid.position, false)
        }
        onScaleChanged: (delta) => {
            map.zoomLevel += Math.log2(delta)
            map.alignCoordinateToPoint(map.startCentroid, pinch.centroid.position)
        }
        onRotationChanged: (delta) => {
            map.bearing -= delta
            map.alignCoordinateToPoint(map.startCentroid, pinch.centroid.position)
        }
        grabPermissions: PointerHandler.TakeOverForbidden
    }
    WheelHandler {
        id: wheel
        // workaround for QTBUG-87646 / QTBUG-112394 / QTBUG-112432:
        // Magic Mouse pretends to be a trackpad but doesn't work with PinchHandler
        // and we don't yet distinguish mice and trackpads on Wayland either
        acceptedDevices: Qt.platform.pluginName === "cocoa" || Qt.platform.pluginName === "wayland"
                         ? PointerDevice.Mouse | PointerDevice.TouchPad
                         : PointerDevice.Mouse
        rotationScale: 1/120
        property: "zoomLevel"
    }
    DragHandler {
        id: drag
        target: null
        onActiveChanged: if (active) {
            mapView.centering = false
        }
        onTranslationChanged: (delta) => map.pan(-delta.x, -delta.y)
    }

    onZoomLevelChanged: {
      // Scaling the location marker according to zoom level.
      // Marker radius = meters per pixel * 10
      circle.radius = 156543.03392 * Math.cos(mapView.latitude * Math.PI / 180) / Math.pow(2, map.zoomLevel) * 10
    }
  }

  RoundButton {
    height: 40
    width: 40
    anchors.top: map.top
    anchors.right: map.right

    // Strangely, ⌖ doesn't work on Bionic
    text: "✲"
    font.pixelSize: 25

    Material.background: Material.primary
    Material.foreground: mapView.centering ? "white" : "black"

    onClicked: {
      mapView.centering = true
    }

    hoverEnabled: true

    ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
    ToolTip.visible: hovered
    ToolTip.text: qsTr("Track position")
  }
}
