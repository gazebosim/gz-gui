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

Popup {
  id: snackbar
  modal: duration == 0
  focus: duration == 0
  x: (window.width - width) / 2
  y: window.height - window.height / 6
  width: window.width - window.width / 6
  contentHeight: notificationColumn.height

  background: Rectangle {
    border.color: "#444"
    color: "#c9daf8"
  }

  // Duration of the snackbar. If duration is equal to zero then
  // you should click somewhere in Ignition Gazebo to close it.
  property int duration: 1000

  function setText(_message) {
    notificationText.text = _message
    if (duration > 0)
    {
      timer.restart()
    }
  }

  function setTextDuration(_message, _duration) {
    notificationText.text = _message
    duration = _duration
    if (duration > 0)
    {
      timer.restart()
    }
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
      }
  }
}
