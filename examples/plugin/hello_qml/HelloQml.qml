import QtQuick 2.0
import QtQuick.Controls 2.0

Button {
  text: qsTr("Hello, QML!")
  highlighted: true
  onClicked: { HelloQml.OnButton(); }
}

