import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3

ToolBar
{
  RowLayout
  {
    anchors.fill: parent

    Label
    {
      text: "Plugin"
      elide: Label.ElideRight
      leftPadding: 10
      horizontalAlignment: Qt.AlignHLeft
      verticalAlignment: Qt.AlignVCenter
      Layout.fillWidth: true
    }
    ToolButton
    {
      text: qsTr("▫")
      onClicked: close()
    }
    ToolButton
    {
      text: qsTr("X")
      onClicked: close()
    }
  }
}
