import QtQuick 2.9
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

SpinBox {
  style: SpinBoxStyle{
    background: Rectangle {
      implicitWidth: 70
      implicitHeight: 40
      border.color: "gray"
    }
  }
}
