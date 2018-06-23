import QtQuick 2.0
import QtQuick.Controls 2.0
import RenderWindow 1.0

Rectangle {
  width: 1000
  height: 800

  Connections {
    target: MainWindow
    onCardResized: {
      width = parent.width
      height = parent.height
    }
  }

  Rectangle {
    anchors.fill: parent

    RenderWindow {
      id: renderWindow
      objectName: "rw"
      anchors.fill: parent
    }
  }
}


