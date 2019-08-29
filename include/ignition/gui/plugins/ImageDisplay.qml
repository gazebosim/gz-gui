import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.1
import QtQuick.Layouts 1.3

Rectangle {
  id: "imageDisplay"
  color: "transparent"
  anchors.fill: parent
  Layout.minimumWidth: 250
  Layout.minimumHeight: 375

  /**
   * True to show topic picker
   */
  property bool showPicker: false

  /**
   * Unique name for this plugin instance
   */
  property string uniqueName: ""

  property int tooltipDelay: 500
  property int tooltipTimeout: 1000

  onParentChanged: {
    if (undefined === parent)
      return;

    uniqueName = parent.card().objectName + "imagedisplay";
    image.reload();
  }

  Connections {
    target: ImageDisplay
    onNewImage: image.reload();
  }

  ColumnLayout {
    id: imageDisplayColumn
    anchors.fill: parent
    anchors.margins: 10

    RowLayout {
      visible: showPicker
      RoundButton {
        text: "\u21bb"
        Material.background: Material.primary
        onClicked: {
          ImageDisplay.OnRefresh();
        }
        ToolTip.visible: hovered
        ToolTip.delay: tooltipDelay
        ToolTip.timeout: tooltipTimeout
        ToolTip.text: qsTr("Refresh list of topics publishing images")
      }
      ComboBox {
        id: combo
        Layout.fillWidth: true
        model: ImageDisplay.topicList
        onCurrentIndexChanged: {
          if (currentIndex < 0)
            return;

          ImageDisplay.OnTopic(textAt(currentIndex));
        }
        ToolTip.visible: hovered
        ToolTip.delay: tooltipDelay
        ToolTip.timeout: tooltipTimeout
        ToolTip.text: qsTr("Ignition transport topics publishing Image messages")
      }
    }
    Image {
      id: image
      fillMode: Image.PreserveAspectFit
      Layout.fillHeight: true
      Layout.fillWidth: true
      verticalAlignment: Image.AlignTop
      function reload() {
        // Force image request to C++
        source = "image://" + uniqueName + "/" + Math.random().toString(36).substr(2, 5);
      }
    }
  }
}
