/*
 * Copyright (C) 2018 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
*/
import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.1
import QtQuick.Dialogs
import Qt.labs.platform as Platform

/**
 * Style dialog
 */
Dialog {

  // Inherited properties
  id: styleDialog
  modal: false
  focus: true
  title: "Style settings"
  contentHeight: styleColumn.height

  // Custom properties
  property int initialPrimary: -1
  property int initialAccent: -1
  property int initialTheme: -1
  property int foregroundShade: Material.Shade500

  /**
   * ✎
   */
  property string editIcon: "\u270E"

  /**
   * Array with all pre-defined material colors.
   * Must match materialColorEnums
   */
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

  /**
   * Array with all pre-defined material colors
   * Must match materialColorStrs
   */
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

  // Connections (C++ signal to QML slot)
  Connections {
    target: _MainWindow
    function onMaterialThemeChanged() {
      updateTheme(_MainWindow.materialTheme);
    }
  }

  Connections {
    target: _MainWindow
    function onMaterialPrimaryChanged() {
      updatePrimary(_MainWindow.materialPrimary);
    }
  }

  Connections {
    target: _MainWindow
    function onMaterialAccentChanged() {
      updateAccent(_MainWindow.materialAccent);
    }
  }

  /**
   * Convert a color component (R/G/B/A) to hex
   * @param type:int _c Color in the 0~1 range
   */
  function componentToHex(_c) {
    _c = (_c * 255) | 0;
    var hex = _c.toString(16);
    return hex.length == 1 ? "0" + hex : hex;
  }

  /**
   * Convert a color object to a hex string
   * @param type:color _color Color object
   */
  function colorToHex(_color) {
    return "#" + componentToHex(_color.r)
               + componentToHex(_color.g)
               + componentToHex(_color.b);
  }

  /**
   * Update primary color
   * @param type:string _primary Optional color name
   */
  function updatePrimary(_primary) {

    var index = -1;

    // When setting from _MainWindow / ColorDialog
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
    else if (_primary.length !== 0)
    {
      c = _primary;
      materialPrimaryCombo.currentIndex = -1;
      materialPrimaryCombo.editText = _primary;
    }
    else
    {
      return;
    }
    window.Material.primary = c
  }

  /**
   * Update accent color
   * @param type:string _accent Optional color name
   */
  function updateAccent(_accent) {

    var index = -1;

    // When setting from _MainWindow / ColorDialog
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
    else if (_accent.length !== 0)
    {
      c = _accent;
      materialAccentCombo.currentIndex = -1;
      materialAccentCombo.editText = _accent;
    }
    else
    {
      return;
    }
    window.Material.accent = c
  }

  /**
   * Update theme
   * @param type:string _theme Optional theme name
   */
  function updateTheme(_theme) {

    // Change theme
    if (typeof _theme === "string" && _theme.length !== 0)
    {

      materialThemeCombo.currentIndex = _theme === "Light" ? 0 : 1;
    }
    else if (materialThemeCombo.currentIndex !== -1)
    {
      _theme = materialThemeCombo.currentText
    }
    else
      return;

    if (_theme === "Light")
    {
      window.Material.theme = Material.Light
      foregroundShade = Material.Shade500
    }
    else
    {
      window.Material.theme = Material.Dark
      foregroundShade = Material.Shade200
    }

    // Update all colors according to new shade
    updatePrimary();
    updateAccent();
  }

  /**
   * Lifecycle hook
   */
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
    }

    initialTheme = window.Material.theme
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
    }

    Platform.ColorDialog {
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

    Platform.ColorDialog {
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
