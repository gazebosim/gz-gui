import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.1

Row {
  id: timePanel
  width: 250
  height: 50
  spacing: 10

  property string playIcon: "\u25B6"
  property string pauseIcon: "\u275A\u275A"
  property string stepIcon: "\u25B8\u25B8"

  /**
   * Play / pause
   */
  RoundButton {
    id: playButton
    text: checked ? pauseIcon : playIcon
    highlighted: true
    checkable: true
    anchors.verticalCenter: parent.verticalCenter
    onClicked: {
      if (checked)
        TimePanel.OnPlay()
      else
        TimePanel.OnPause()
    }
    Material.background: Material.primary
    Material.elevation: 0
  }

  /**
   * Step
   */
  RoundButton {
    text: stepIcon
    highlighted: true
    height: playButton.height * 0.8
    width: playButton.width * 0.8
    anchors.verticalCenter: parent.verticalCenter
    onClicked: {
      TimePanel.OnStep()
    }
    Material.background: Material.primary
    Material.elevation: 0
  }
}
