/*
 * Copyright (C) 2020 Open Source Robotics Foundation
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

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

GridLayout {
  columns: 1
  columnSpacing: 10
  Layout.minimumWidth: 350
  Layout.minimumHeight: 200
  anchors.fill: parent
  anchors.margins: 10

  Label {
    Layout.columnSpan: 1
    Layout.fillWidth: true
    wrapMode: Text.WordWrap
    text: "Keystrokes are being published to topic:\n'/keyboard/keypress'."
  }


  Item {
    Layout.columnSpan: 1
    width: 10
    Layout.fillHeight: true
  }
}

