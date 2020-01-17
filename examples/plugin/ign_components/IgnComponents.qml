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
import QtQuick 2.9
import QtQuick.Controls 2.0
import "qrc:/qml"

Column {
  anchors.fill: parent
  spacing: 2

  // Integer spin box
  IgnSpinBox {
    minimumValue: -2
    maximumValue: 19
    decimals: 0
    stepSize: 3
    value: 5
  }

  // Double spin box
  IgnSpinBox {
    minimumValue: -2.5
    maximumValue: 19.3
    decimals: 4
    stepSize: 3.6
    value: 5.8
    width: 300
  }
}
