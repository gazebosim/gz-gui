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

import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.0
import QtQuick.Controls.Material 2.2

Popup {
  id: snackbar
  x: (window.width - width) / 2
  y: window.height - window.height / 6
  width: window.width - window.width / 6
  contentHeight: notificationColumn.height

  closePolicy: Popup.CloseOnEscape

  property var popupArray: []

  onClosed: {
    timer.stop()
    checkArray();
  }

  background: Rectangle {
    border.color: "#444"
    color: Material.background
  }

  // Duration of the snackbar. If duration is equal to zero then
  // you should click somewhere in Ignition Gazebo to close it.
  property int duration: 4000

  function setText(_message) {
    popupArray.push({"text": _message, "duration": duration})
    checkArray();
  }

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

  function setTextDuration(_message, _duration) {
    popupArray.push({"text": _message, "duration": _duration})
    checkArray();
  }

  Column {
    id: notificationColumn
    spacing: 20

    Label {
      id: notificationText
      width: snackbar.availableWidth
      wrapMode: Label.Wrap
      font.pixelSize: 18
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
