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
  property int foregroundShade: Material.Shade500

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
  ]

  Connections {
    target: MainWindow
    onMaterialThemeChanged: {
      updateTheme(MainWindow.materialTheme);
    }
  }

  Connections {
    target: MainWindow
    onMaterialPrimaryChanged: {
      updatePrimary(MainWindow.materialPrimary);
    }
  }

  Connections {
    target: MainWindow
    onMaterialAccentChanged: {
      updateAccent(MainWindow.materialAccent);
    }
  }

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
  function updatePrimary(_primary) {

    var index = -1;

    // When setting from MainWindow / ColorDialog
    if (typeof _primary === "string")
    {
      index = materialColorStrs.indexOf(_primary)
    }
    // When setting from combo box
    else if (materialPrimaryCombo.currentIndex !== -1)
    {
      index = materialPrimaryCombo.currentIndex
    }
    else
    {
      return;
    }

    var c;

    // One of the material colors
    if (index !== -1)
    {
      c = Material.color(materialColorEnums[index], foregroundShade);
      materialPrimaryCombo.currentIndex = index;
    }
    // Custom color
    else
    {
      c = _primary;
      materialPrimaryCombo.currentIndex = -1;
      materialPrimaryCombo.editText = _primary;
    }
    window.Material.primary = c
  }

  /**
   * Update accent color
   */
  function updateAccent(_accent) {

    var index = -1;

    // When setting from MainWindow / ColorDialog
    if (typeof _accent === "string")
    {
      index = materialColorStrs.indexOf(_accent)
    }
    // When setting from combo box
    else if (materialAccentCombo.currentIndex !== -1)
    {
      index = materialAccentCombo.currentIndex
    }
    else
    {
      return;
    }

    var c;

    // One of the material colors
    if (index !== -1)
    {
      c = Material.color(materialColorEnums[index], foregroundShade);
      materialAccentCombo.currentIndex = index;
    }
    // Custom color
    else
    {
      c = _accent;
      materialAccentCombo.currentIndex = -1;
      materialAccentCombo.editText = _accent;
    }
    window.Material.accent = c
  }

  /**
   * Update theme
   */
  function updateTheme(_theme) {

    // Change theme
    if (typeof _theme === "string")
    {
      materialThemeCombo.currentIndex = _theme === "Light" ? 0 : 1;
    }
    else if (materialThemeCombo.currentIndex !== -1)
    {
      _theme = materialThemeCombo.currentText
    }
    else
      return;

    window.Material.theme = _theme

    // Updade shade
    foregroundShade = _theme === "Light" ? Material.Shade500 :
                                           Material.Shade200

    // Update all colors according to new shade
    updatePrimary();
    updateAccent();
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
    width: styleDialog.width * 0.6

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
      width: styleColumn.width
      currentIndex: initialTheme
      model: ["Light", "Dark"]
      delegate: ItemDelegate {
        text: modelData
        width: parent.width
      }
      onCurrentTextChanged: {
        updateTheme();
      }
    }

    Label {
      text: "Primary"
      anchors.horizontalCenter: styleDialog.horizontalCenter
    }

    ColorDialog {
      id: materialPrimaryDialog
      title: "Primary color"
      // options: ColorDialog.NoButtons
      onCurrentColorChanged: {

        // Avoiding pure black because for some reason it is set to that as the
        // dialog opens
        if (currentColor == "#000000")
          return;

        updatePrimary(colorToHex(currentColor))
      }
    }

    Row {
      ComboBox {
        id: materialPrimaryCombo
        width: styleColumn.width
        editable: true
        // selectByMouse: true
        currentIndex: initialPrimary
        displayText: currentText
        model: materialColorStrs
        delegate: ItemDelegate {
          text: materialColorStrs[index]
          width: parent.width
        }
        onCurrentTextChanged: {
          updatePrimary()
        }
      }

      ToolButton {
        text: "\u270E"
        font.pixelSize: 20
        onClicked: {
          materialPrimaryDialog.open()
        }
      }
    }

    Label {
      text: "Accent"
    }

    ColorDialog {
      id: materialAccentDialog
      title: "Accent color"
      // options: ColorDialog.NoButtons
      onCurrentColorChanged: {

        // Avoiding pure black because for some reason it is set to that as the
        // dialog opens
        if (currentColor == "#000000")
          return;

        updateAccent(colorToHex(currentColor))
      }
    }

    Row {
      ComboBox {
        id: materialAccentCombo
        width: styleColumn.width
        editable: true
        // selectByMouse: true
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

      ToolButton {
        text: "\u270E"
        font.pixelSize: 20
        onClicked: {
          materialAccentDialog.open()
        }
      }
    }
  }
}
