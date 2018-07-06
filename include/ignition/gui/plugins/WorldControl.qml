import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.1
import QtQuick.Layouts 1.3
import "qrc:/qml"

Row {
  id: worldControl
  width: 250
  height: 50
  spacing: 10

  /**
   * True to show play/pause button
   */
  property bool showPlay: false

  /**
   * True to show step buttons
   */
  property bool showStep: false

  /**
   * True to show multistep popup
   */
  property bool showMultiStep: false

  property string playIcon: "\u25B6"
  property string pauseIcon: "\u275A\u275A"
  property string stepIcon: "\u25B8\u25B8"

  /**
   * Play / pause
   */
  RoundButton {
    id: playButton
    visible: showPlay
    text: checked ? pauseIcon : playIcon
    checkable: true
    anchors.verticalCenter: parent.verticalCenter
    onClicked: {
      if (checked)
        WorldControl.OnPlay()
      else
        WorldControl.OnPause()
    }
    Material.background: Material.primary
  }

  /**
   * Step button
   */
  RoundButton {
    id: stepButton
    text: stepIcon
    visible: showStep
    height: playButton.height * 0.8
    width: playButton.width * 0.8
    anchors.verticalCenter: parent.verticalCenter
    onClicked: {
      WorldControl.OnStep()
    }
    Material.background: Material.primary

    function windowPos() {
      return mapToItem(worldControl.parent.card().parent, 0, 0);
    }

    MouseArea {
      id: hoverArea
      anchors.fill: parent
      hoverEnabled: true
      onEntered: {

        var minX = 0;
        var maxX = worldControl.parent.card().parent.width -
            stepPopup.width * 0.5;
        var popX = stepButton.windowPos().x - stepPopup.width * 0.5;

        stepPopup.x = Math.min(Math.max(popX, minX), maxX);
        stepPopup.y = stepButton.windowPos().y - stepPopup.height
      }
    }
  }

  Popup {
    id: stepPopup
    visible: hoverArea.containsMouse
    parent: worldControl.parent.card().parent

    contentItem: RowLayout {
      id: row

      Label {
        text: "Steps"
      }

      IgnSpinBox {
        maximumValue: 10000
        value: 1
        onValueChanged: {
          WorldControl.OnStepCount(value)
        }
      }
    }
  }
}
