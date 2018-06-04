import QtQuick 2.3
import QtQuick.Controls 1.2

ApplicationWindow {

  /**
   * The plugin name goes on top of the dialog window
   */
  property alias pluginName: dialog.title

  id: dialog
  visible: true
  minimumHeight: 320
  minimumWidth: 250
}
