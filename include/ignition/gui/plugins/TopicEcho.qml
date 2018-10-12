import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

Rectangle {
  id: topicEcho
  objectName: "topicEcho"
  Layout.minimumWidth: 300
  Layout.minimumHeight: 1000
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
      value: TopicEcho.buffer
      onValueChanged: {
        TopicEcho.SetBuffer(value)
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
      width: (topicEcho.width - 2 * 10)
      height: 200
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
