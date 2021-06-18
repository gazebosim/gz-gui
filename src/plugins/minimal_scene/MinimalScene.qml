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
import QtQuick 2.0
import QtQuick.Controls 2.0
import RenderWindow 1.0
import QtGraphicalEffects 1.0

Rectangle {
  width: 1000
  height: 800

  /**
   * True to enable gamma correction
   */
  property bool gammaCorrect: false


  RenderWindow {
    id: renderWindow
    objectName: "rw"
    anchors.fill: parent
  }

  /*
   * Gamma correction for sRGB output. Enabled when engine is set to ogre2
   */
  GammaAdjust {
      anchors.fill: renderWindow
      source: renderWindow
      gamma: 2.4
      enabled: gammaCorrect
      visible: gammaCorrect
  }

  onParentChanged: {
    if (undefined === parent)
      return;

      width = Qt.binding(function() {return parent.parent.width})
      height = Qt.binding(function() {return parent.parent.height})
  }
}
