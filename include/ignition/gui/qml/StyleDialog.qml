import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.1
import QtQuick.Dialogs 1.0

/**
 * Style dialog
 */
Dialog {
  id: styleDialog
  modal: false
  focus: true
  title: "Style settings"
  contentHeight: styleColumn.height

  property int initialPrimary: -1
  property int initialAccent: -1
  property int initialForeground: -1
  property int initialBackground: -1
  property int foregroundShade: Material.Shade500
  property int backgroundShade: Material.Shade200

  property var materialColorStrs: [
    "Red",
    "Pink",
    "Purple",
    "DeepPurple",
    "Indigo",
    "Blue",
    "LightBlue",
    "Cyan",
    "Teal",
    "Green",
    "LightGreen",
    "Lime",
    "Yellow",
    "Amber",
    "Orange",
    "DeepOrange",
    "Brown",
    "Grey",
    "BlueGrey",
//    "White",
//    "Black",
  ]

  property var materialColorEnums: [
    Material.Red,
    Material.Pink,
    Material.Purple,
    Material.DeepPurple,
    Material.Indigo,
    Material.Blue,
    Material.LightBlue,
    Material.Cyan,
    Material.Teal,
    Material.Green,
    Material.LightGreen,
    Material.Lime,
    Material.Yellow,
    Material.Amber,
    Material.Orange,
    Material.DeepOrange,
    Material.Brown,
    Material.Grey,
    Material.BlueGrey,
//    Material.White,
//    Material.Black,
  ]

  // Helper functions to convert a QML color to a hex string, so that they can
  // be compared
  function componentToHex(_c) {
    _c = _c * 255
    var hex = _c.toString(16);
    return hex.length == 1 ? "0" + hex : hex;
  }
  function colorToHex(_color) {
    return "#" + componentToHex(_color.r)
               + componentToHex(_color.g)
               + componentToHex(_color.b);
  }

  /**
   * Update primary color
   */
  function updatePrimary() {
    if (materialPrimaryCombo.currentIndex === -1)
      return;

    var c = Material.color(
        materialColorEnums[materialPrimaryCombo.currentIndex],
        foregroundShade)
    window.Material.primary = c
  }

  /**
   * Update accent color
   */
  function updateAccent() {
    if (materialAccentCombo.currentIndex === -1)
      return;

    var c = Material.color(
        materialColorEnums[materialAccentCombo.currentIndex],
        foregroundShade)
    window.Material.accent = c
  }

  /**
   * Update foreground color
   */
  function updateForeground() {
    if (materialForegroundCombo.currentIndex === -1)
      return;

    var c = Material.color(
        materialColorEnums[materialForegroundCombo.currentIndex],
        foregroundShade)
    window.Material.foreground = c
  }

  /**
   * Update background color
   */
  function updateBackground() {
    if (materialBackgroundCombo.currentIndex === -1)
      return;

    var c = Material.color(
        materialColorEnums[materialBackgroundCombo.currentIndex],
        backgroundShade)
    window.Material.background = c
  }

  Component.onCompleted: {

    // Get initial values
    for (var i = 0; i < materialColorEnums.length; ++i)
    {
      var hex = colorToHex(Material.color(materialColorEnums[i]))
      if (hex == window.Material.primary)
      {
        initialPrimary = i;
      }
      if (hex == window.Material.accent)
      {
        initialAccent = i;
      }
      if (hex == window.Material.foreground)
      {
        initialForeground = i;
      }
      if (hex == window.Material.background)
      {
        initialBackground = i;
      }
    }
  }

  Column {
    id: styleColumn
    anchors.horizontalCenter: styleDialog.horizontalCenter
    width: parent.width

    // TODO(anyone) extend to universal / default styles, beware that
    // changing style at runtime doesn't seem to be supported, but we could save
    // and prompt the user to restart
    Label {
      text: "Material style"
      font.weight: Font.Bold
    }

    Label {
      text: "Theme"
    }

    ComboBox {
      id: materialThemeCombo
      width: 200
      currentIndex: initialTheme
      model: ["Light", "Dark"]
      delegate: ItemDelegate {
        text: modelData
        width: parent.width
      }
      onCurrentTextChanged: {
        // Change theme
        window.Material.theme = currentText

        // Updade shade
        foregroundShade = currentText === "Light" ? Material.Shade500 :
                                                    Material.Shade200
        backgroundShade = currentText === "Dark" ? Material.Shade500 :
                                                   Material.Shade200

        // Update all colors according to new shade
        updatePrimary();
        updateAccent();
        // updateForeground();
        // updateBackground();
      }
    }

    Label {
      text: "Primary"
      anchors.horizontalCenter: styleDialog.horizontalCenter
    }

    ComboBox {
      id: materialPrimaryCombo
      width: 200
      currentIndex: initialPrimary
      displayText: currentText
      model: materialColorStrs
      delegate: ItemDelegate {
        text: materialColorStrs[index]
        width: parent.width
      }
      onCurrentIndexChanged: {
        updatePrimary()
      }
    }

    Label {
      text: "Accent"
    }

    ComboBox {
      id: materialAccentCombo
      width: 200
      currentIndex: initialAccent
      displayText: currentText
      model: materialColorStrs
      delegate: ItemDelegate {
        text: materialColorStrs[index]
        width: parent.width
      }
      onCurrentIndexChanged: {
        updateAccent()
      }
    }

//    Label {
//      text: "Foreground"
//    }
//
//    ComboBox {
//      id: materialForegroundCombo
//      width: 200
//      currentIndex: initialForeground
//      displayText: currentText
//      model: materialColorStrs
//      delegate: ItemDelegate {
//        text: materialColorStrs[index]
//        width: parent.width
//      }
//      onCurrentIndexChanged: {
//        updateForeground()
//      }
//    }
//
//    Label {
//      text: "Background"
//    }
//
//    ComboBox {
//      id: materialBackgroundCombo
//      width: 200
//      currentIndex: initialBackground
//      displayText: currentText
//      model: materialColorStrs
//      delegate: ItemDelegate {
//        text: materialColorStrs[index]
//        width: parent.width
//      }
//      onCurrentIndexChanged: {
//        updateBackground()
//      }
//    }
  }
}
