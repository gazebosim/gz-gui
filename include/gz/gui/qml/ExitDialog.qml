/*
 * Copyright (C) 2026 Open Source Robotics Foundation
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

/**
 * Confirmation dialog shown when the main window is closed.
 */
Dialog {
  id: exitDialog

  /**
   * Prompt shown as the dialog title. A default prompt is used if empty.
   */
  property string promptText: ""

  /**
   * True to show the button that closes only the GUI
   */
  property bool showCloseGui: true

  /**
   * True to show the button that shuts down the server and the GUI
   */
  property bool showShutdown: false

  /**
   * Text of the close GUI button. The style's default is used if empty.
   */
  property string closeGuiText: ""

  /**
   * Text of the shutdown button. A default text is used if empty.
   */
  property string shutdownText: ""

  /**
   * Emitted when the shutdown button is clicked, before closeRequested.
   */
  signal stopServerRequested()

  /**
   * Emitted when the GUI should close.
   */
  signal closeRequested()

  title: (promptText ? promptText : "Do you really want to exit?")

  modal: true
  focus: true
  parent: ApplicationWindow.overlay
  width: 500
  x: (parent.width - width) / 2
  y: (parent.height - height) / 2
  closePolicy: Popup.CloseOnEscape
  standardButtons:
    (showCloseGui ? Dialog.Ok : Dialog.NoButton) |
    (showShutdown ? Dialog.Discard : Dialog.NoButton) |
    Dialog.Cancel

  // The button texts need to be changed later than in onCompleted as standardButtons change later
  onAboutToShow: function () {
    if (showCloseGui && closeGuiText)
      footer.standardButton(Dialog.Ok).text = closeGuiText
    if (showShutdown)
      footer.standardButton(Dialog.Discard).text =
        (shutdownText ? shutdownText : "Shutdown server and GUI")
  }

  footer:
    DialogButtonBox {
      onClicked: function (btn) {
        if (btn == this.standardButton(Dialog.Ok)) {
          exitDialog.closeRequested();
        }
        else if (btn == this.standardButton(Dialog.Discard)) {
          exitDialog.stopServerRequested();
          // if GUI and server run in the same process, give server opportunity to kill the GUI
          closeTimer.start();
        }
      }
    }

  Timer {
    id: closeTimer
    interval: 100
    repeat: false
    onTriggered: exitDialog.closeRequested()
  }
}
