import QtQuick 2.9
import QtQuick.Controls 2.2

Menu {
  id: pluginMenu

  Connections {
    target: MainWindow
    onConfigChanged: {
      pluginMenuListView.model = MainWindow.PluginListModel()
    }
  }

  ListView {
    id: pluginMenuListView
    height: pluginMenu.height

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

