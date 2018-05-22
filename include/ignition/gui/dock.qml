import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0

// External pane - fills whole dock
Pane
{
  background: Rectangle
  {
    color: "#ededed"
  }

  // Internal pane - a bit smaller so we see elevation
  Pane
  {
    width: parent.width
    height: parent.height

    Material.elevation: 6
  }
}
