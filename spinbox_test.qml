
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3

import "include/gz/gui/qml"

ApplicationWindow {
  width: 800
  height: 800

  Rectangle
  {
    width: 300
    height: 300
    GzSpinBox {
      id: maxForwardVelField
      maximumValue: 100.0
      minimumValue: -100.0
      decimals: 2
      // stepSize: 0.1

    }
  }
}
