import QtQuick 2.0
import QtQuick.Controls 2.0

Button {
  text: qsTr("Hello, plugin!")
  highlighted: true
  onClicked: { HelloPlugin.OnButton(); }
}
