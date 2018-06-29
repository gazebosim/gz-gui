import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.1
import QtQuick.Layouts 1.3

GridLayout {
  width: 250
  height: 50
  columns: 2

  property int tooltipDelay: 500
  property int tooltipTimeout: 1000

  /**
   * Real time factor
   */
  Label {
    text: "RTF"
    ToolTip.visible: realTimeFactorMa.containsMouse
    ToolTip.delay: tooltipDelay
    ToolTip.timeout: tooltipTimeout
    ToolTip.text: qsTr("Real time factor")

    MouseArea {
      id: realTimeFactorMa
      anchors.fill: parent
      hoverEnabled: true
    }
  }
  Label {
    text: WorldStats.realTimeFactor
  }

  /**
   * Sim time
   */
  Label {
    text: "Sim time"
    ToolTip.visible: simTimeMa.containsMouse
    ToolTip.delay: tooltipDelay
    ToolTip.timeout: tooltipTimeout
    ToolTip.text: qsTr("Simulation time")

    MouseArea {
      id: simTimeMa
      anchors.fill: parent
      hoverEnabled: true
    }
  }
  Label {
    text: WorldStats.simTime
  }

  /**
   * Real time
   */
  Label {
    text: "Real time"
    ToolTip.visible: realTimeMa.containsMouse
    ToolTip.delay: tooltipDelay
    ToolTip.timeout: tooltipTimeout
    ToolTip.text: qsTr("Wall-clock time")

    MouseArea {
      id: realTimeMa
      anchors.fill: parent
      hoverEnabled: true
    }
  }
  Label {
    text: WorldStats.realTime
  }

  /**
   * Iterations
   */
  Label {
    text: "Iterations"
    ToolTip.visible: iterationsMa.containsMouse
    ToolTip.delay: tooltipDelay
    ToolTip.timeout: tooltipTimeout
    ToolTip.text: qsTr("Simulation iterations")

    MouseArea {
      id: iterationsMa
      anchors.fill: parent
      hoverEnabled: true
    }
  }
  Label {
    text: WorldStats.iterations
  }
}
