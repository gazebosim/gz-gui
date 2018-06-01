import QtQuick 2.0
import QtQuick.Controls 2.0

Button {
  text: qsTr("Publish")
  highlighted: true
  onClicked: { console.log("Clicked"); Publisher.OnPublish(true); }
}
