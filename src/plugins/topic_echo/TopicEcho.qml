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
import QtQuick.Layouts 1.3

Rectangle {
  id: topicEcho
  objectName: "topicEcho"
  Layout.minimumWidth: 300
  Layout.minimumHeight: 300
  color: "transparent"

  property int tooltipDelay: 500
  property int tooltipTimeout: 1000

  Column {
    anchors.fill: parent
    anchors.margins: 10

    Row {
      Column {
        Label {
          text: "Topic"
        }

        TextField {
          id: topicField
          text: TopicEcho.topic
          selectByMouse: true
        }
      }

      Switch {
        text: qsTr("Echo")
        onToggled: {
          TopicEcho.topic = topicField.text
          TopicEcho.OnEcho(checked);
        }
        ToolTip.visible: hovered
        ToolTip.delay: tooltipDelay
        ToolTip.timeout: tooltipTimeout
        ToolTip.text: checked ? qsTr("Stop echoing") : qsTr("Start echoing")
      }
    }

    Label {
      text: "Buffer"
    }

    SpinBox {
      id: bufferField
      value: 10
      onValueChanged: {
        TopicEcho.OnBuffer(value)
      }
    }

    CheckBox {
      text: qsTr("Pause")
      checked: TopicEcho.paused
      onClicked: {
        TopicEcho.SetPaused(checked)
      }
    }

    Label {
      id: msgsLabel
      text: "Messages"
    }

    Rectangle {
      width: topicEcho.parent.width - 20
      height: topicEcho.parent.height - 200
      color: "transparent"

      ListView {
        id: listView
        clip: true
        anchors.fill: parent

        focus: true
        currentIndex: -1

        delegate: ItemDelegate {
          width: parent.width
          text: display
        }

        model: TopicEchoMsgList

        ScrollIndicator.vertical: ScrollIndicator {
          active: true;
          onActiveChanged: {
            active = true;
          }
        }
      }
    }
  }
}
