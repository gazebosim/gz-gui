import QtQuick 2.0
import QtQuick.Controls 2.0
import RenderWindow 1.0

Rectangle {
  width: 1000
  height: 800
  color: "blue"

  RenderWindow {
    id: renderWindow
    anchors.fill: parent
    anchors.margins: 20
  }
}


