/*
 * Copyright (C) 2017 Open Source Robotics Foundation
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

#include <gtest/gtest.h>
#include <ignition/common/Console.hh>
#include <ignition/rendering.hh>

#include "ignition/gui/EnumWidget.hh"
#include "ignition/gui/Iface.hh"
#include "ignition/gui/MainWindow.hh"
#include "ignition/gui/Plugin.hh"
#include "ignition/gui/Vector3dWidget.hh"

using namespace ignition;
using namespace gui;

/////////////////////////////////////////////////
TEST(CameraTrack3DTest, WithoutScene)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // Load CameraTrack3D without Scene3D
  EXPECT_TRUE(loadPlugin("CameraTrack3D"));

  // Create main window
  EXPECT_TRUE(createMainWindow());

  // Check there is no scene
  auto engine = rendering::engine("ogre");
  ASSERT_NE(nullptr, engine);

  EXPECT_EQ(0u, engine->SceneCount());

  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(CameraTrack3DTest, Fields)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // Load Scene3D
  EXPECT_TRUE(loadPlugin("Scene3D"));

  // Load Grid3D so we have more nodes in the scene
  const char *pluginStr =
    "<plugin filename='Grid3D' delete_later='true'>"
      "<insert/>"
    "</plugin>";

  tinyxml2::XMLDocument pluginDoc;
  pluginDoc.Parse(pluginStr);
  EXPECT_TRUE(ignition::gui::loadPlugin("Grid3D",
      pluginDoc.FirstChildElement("plugin")));

  // Load CameraTrack3D
  EXPECT_TRUE(loadPlugin("CameraTrack3D"));

  // Create main window
  EXPECT_TRUE(createMainWindow());

  auto win = mainWindow();
  ASSERT_NE(nullptr, win);

  // Get dropdowns
  auto enums = win->findChildren<EnumWidget *>();
  ASSERT_EQ(enums.count(), 3);

  // Check there are 2 camera options, None and the camera
  EXPECT_EQ(enums[0]->Combo()->count(), 2);

  // Check there are 3 node options, None, the grid and the camera
  EXPECT_EQ(enums[1]->Combo()->count(), 3);
  EXPECT_EQ(enums[2]->Combo()->count(), 3);

  // Check the track and follow options are read-only
  EXPECT_TRUE(enums[1]->ReadOnly());
  EXPECT_TRUE(enums[2]->ReadOnly());

  auto vector3ds = win->findChildren<Vector3dWidget *>();
  EXPECT_EQ(vector3ds.count(), 2);

  for (auto v : vector3ds)
    EXPECT_TRUE(v->ReadOnly());

  // Choose the camera
  enums[0]->Combo()->setCurrentIndex(1);

  for (auto e : enums)
    EXPECT_FALSE(e->ReadOnly());
  for (auto v : vector3ds)
    EXPECT_TRUE(v->ReadOnly());

  // Choose the grid to track
  enums[1]->Combo()->setCurrentIndex(2);

  for (auto e : enums)
    EXPECT_FALSE(e->ReadOnly());

  EXPECT_FALSE(vector3ds[0]->ReadOnly());
  EXPECT_TRUE(vector3ds[1]->ReadOnly());

  // Choose the grid to follow
  enums[2]->Combo()->setCurrentIndex(2);

  for (auto e : enums)
    EXPECT_FALSE(e->ReadOnly());

  for (auto v : vector3ds)
    EXPECT_FALSE(v->ReadOnly());

  // Choose no camera
  enums[0]->Combo()->setCurrentIndex(0);

  EXPECT_FALSE(enums[0]->ReadOnly());
  EXPECT_TRUE(enums[1]->ReadOnly());
  EXPECT_TRUE(enums[2]->ReadOnly());
  for (auto v : vector3ds)
    EXPECT_TRUE(v->ReadOnly());

  EXPECT_TRUE(stop());
}

