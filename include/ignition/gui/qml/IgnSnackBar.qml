/*
 * Copyright (C) 2022 Open Source Robotics Foundation
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

import QtGraphicalEffects 1.0
import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2
import QtQuick.Dialogs 1.0
import QtQuick.Layouts 1.3
import QtQuick.Window 2.2

/*
 To use the snackbar you need to call the methods in the MainWindow class:
  - notify()
  - notifyWithDuration

For example:
  // This code will show the message "Message" during one second
  App()->findChild<MainWindow *>()->notifyWithDuration("Message", 1000);

  // This code will show the message "Message2" but the dialog will be there
  // until you press the button "Dismiss"
  App()->findChild<MainWindow *>()->notifyWithDuration("Message2", 1000);
*/

Popup {
  id: snackbar
  modal: duration == 0
  focus: duration == 0
  x: (window.width - width) / 2
  y: window.height - window.height / 6
  width: window.width - window.width / 6
  contentHeight: notificationColumn.height

  closePolicy: Popup.NoAutoClose

  // Array that contains a dictionary with two keys "text" and "duration"
  // This structure keeps the message to show using FIFO
  property var popupArray: []

  // Duration of the snackbar. If duration is equal to zero then
  // you should click on the button "Dismiss" to close the dialog",
  // otherwise you need to wait the duration defined.
  property int duration: 0

  // This method is called when the dialog is closed
  onClosed: {
    timer.stop()
    checkArray();
  }

  background: Rectangle {
    color: Material.background
    layer.enabled: true
    layer.effect: DropShadow {
      color: "#aa000000"
      samples: 9
      spread: 0
      radius: 8.0
    }
  }

  // this function is called when notify() o notifyWithDuration() are called
  function setTextDuration(_message, _duration) {
    popupArray.push({"text": _message, "duration": _duration})
    checkArray();
  }

  // This method check if the popupArray has remaining messages to show.
  function checkArray()
  {
    if (popupArray.length == 0)
    {
      return
    }

    if(!timer.running)
    {
      if (popupArray.length > 0)
      {
        var values = popupArray[0]
        notificationText.text = values.text
        duration = values.duration
        snackbar.open()

        // Note that objects cannot be individually added to or removed from
        // the list once created; to modify the contents of a list, it must be
        // reassigned to a new list.
        var newpopupArray = []
        for (var i = 1; i < popupArray.length; i++)
        {
          newpopupArray.push(popupArray[i])
        }

        if (newpopupArray != undefined)
        {
          popupArray = newpopupArray
        }
        else
        {
          popupArray = []
        }
        if (duration > 0)
        {
          timer.restart()
        }
      }
    }
  }

  Row {
    id: notificationColumn

    Label {
      id: notificationText
      width: snackbar.availableWidth * 6 / 8
      wrapMode: Label.Wrap
      font.pixelSize: 18
      anchors.verticalCenter: parent.verticalCenter
    }
    Button {
      visible: duration == 0
      flat: true
      background: Rectangle {
        color: parent.down ? "#cccccc" :
                (parent.hovered ? "#d6d6d6" : "#f6f6f6")
      }
      text: "Dismiss"
        onClicked: snackbar.close()
    }
  }
  Timer {
    id: timer
    interval: snackbar.duration
    onTriggered: {
        if (!running) {
            snackbar.close();
        }
        checkArray();
    }
  }

}
