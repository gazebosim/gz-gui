import QtQuick 2.9
import QtQuick.Controls 2.2

Menu {
  ListView {
    anchors.fill: parent

    delegate: ItemDelegate {
      width: parent.width
      text: modelData
      highlighted: ListView.isCurrentItem
      onClicked: {
        MainWindow.OnAddPlugin(modelData);
        drawer.close()
      }
    }

    model: MainWindow.PluginListModel()

    ScrollIndicator.vertical: ScrollIndicator {
      active: true
    }
  }
}

