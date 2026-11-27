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
 * About dialog
 */
Dialog {
  id: aboutDialog
  modal: true
  focus: true
  title: "Gazebo GUI"
  contentHeight: aboutColumn.height

  Column {
    id: aboutColumn
    spacing: 20

    Label {
      width: aboutDialog.availableWidth
      text: "Gorgeous robotic interfaces since 2018."
      wrapMode: Label.Wrap
      font.pixelSize: 12
    }
  }
}
