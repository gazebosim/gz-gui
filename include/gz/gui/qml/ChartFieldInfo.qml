/*
 * Copyright (C) 2026 Open Source Robotics Foundation
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
  Field or component shown in the fields bar of a Chart,
  with a button to remove it from the chart
*/
Rectangle {
  id: component

  /**
    Field or Component
  */
  property string type: ""

  /**
    field data:
    topic name
    path field key
  */
  property string topic: ""
  property string path: ""

  /**
    component data:
    entity entity ID
    typeId type ID
    type type of the component attribute (Pose3d, Vector3d .. etc)
    attribute (X, Y, Z, Roll, ... etc)
  */
  property string entity: ""
  property string typeId: ""
  property string componentType: ""
  property string attribute: ""
  property string typeName: ""
  property string componentId: entity + "," + typeId + "," + attribute;
  property string displayText: ""

  /**
    set the field name text
  */
  function setText(text) {
      fieldname.text = text;
  }
  signal unsubscribe(string topic, string path);

  /**
    the remove button is clicked
  */
  signal removeClicked();

  radius: width/4
  Rectangle {
    height: parent.height
    width: parent.width
    radius: width/4
    color: Material.accentColor
    clip: true

    MouseArea {
      id : fieldInfoMouse
      anchors.fill: parent
      hoverEnabled: true
      onEntered: enterAnimation.start();
      onExited: exitAnimation.start();
    }

    Text {
      id: fieldname
      text: (component.type === "Field") ? component.topic + "/"+ component.path :
            (component.type === "Component") ? component.entity + "," + component.typeName
                                               + "," + component.attribute : ""
      color: "white"
      elide: Text.ElideRight
      width: parent.width * 0.9
      anchors.verticalCenter: parent.verticalCenter
      leftPadding: 20;
    }

    ToolTip {
      delay: 1000
      timeout: 2000
      text: (component.type === "Field" ) ? component.topic + "-"+ component.path :
            (component.type === "Component") ? "entity: " + component.entity + "\n" +
                                                "typeId: " + component.typeId + "\n" +
                                                "typeName: " + component.typeName + "\n" +
                                                "dataType: " + component.componentType + "\n" +
                                                "attribute: " + component.attribute : ""
      visible: fieldInfoMouse.containsMouse
      y: fieldInfoMouse.mouseY
      x: fieldInfoMouse.mouseX
      enter: null
      exit: null
    }
  }

  Rectangle {
    id: exitBtn
    radius: width / 2
    height: parent.height * 0.8;
    width: height
    color: "red"
    opacity: 0
    anchors.right: parent.right
    anchors.verticalCenter: parent.verticalCenter
    anchors.rightMargin: parent.width * 0.05
    Text { anchors.centerIn: parent; text: "x"; color: "white"}

    MouseArea {
      anchors.fill: parent
      onClicked: {
        exitAnimation.start();
        component.removeClicked();
      }
    }
    NumberAnimation {
      id: enterAnimation
      target: exitBtn; property: "opacity"; duration: 100
      easing.type: Easing.InOutQuad; from: 0; to: 1;
    }
    NumberAnimation {
      id: exitAnimation
      target: exitBtn; property: "opacity"; duration: 100;
      easing.type: Easing.InOutQuad; from: 0.85; to: 0;
    }
  }
}
