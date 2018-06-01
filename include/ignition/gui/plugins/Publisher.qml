import QtQuick 2.9
import QtQuick.Controls 2.2

Rectangle {

  width: 800
  color: "red"
  property int tooltipDelay: 500
  property int tooltipTimeout: 1000

  Column {
    anchors.fill: parent
    anchors.margins: 10

    Label {
      text: "Message type"
    }

    TextField {
      id: msgTypeField
      text: Publisher.msgType
      selectByMouse: true
    }

    Label {
      text: "Topic"
    }

    TextField {
      id: topicField
      text: Publisher.topic
      selectByMouse: true
    }

    Label {
      text: "Message"
    }

    TextArea {
      id: msgDataField
      text: Publisher.msgData
      selectByMouse: true
    }

    Label {
      text: "Frequency (Hz)"
      ToolTip.visible: ma.containsMouse
      ToolTip.delay: tooltipDelay
      ToolTip.timeout: tooltipTimeout
      ToolTip.text: qsTr("Set to zero to publish once")

      MouseArea {
        id: ma
        anchors.fill: parent
        hoverEnabled: true
      }
    }

    SpinBox {
      id: frequencyField
      value: 1.00
// why can't this be parsed?
//      decimals: 2
//      minimumValue: 0.0
//      maximumValue: 10000.0
    }

    Switch {
      text: qsTr("Publish")
      onToggled: {

        // Is there a way to 2-way bind properties, or is this needed?
        Publisher.msgType = msgTypeField.text
        Publisher.topic = topicField.text
        Publisher.msgData = msgDataField.text
        Publisher.frequency = frequencyField.value

        Publisher.OnPublish(checked);
      }
      ToolTip.visible: hovered
      ToolTip.delay: tooltipDelay
      ToolTip.timeout: tooltipTimeout
      ToolTip.text: checked ? qsTr("Stop publising") : qsTr("Start publishing")
    }
  }
}
