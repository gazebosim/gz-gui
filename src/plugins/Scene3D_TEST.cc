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
#include <ignition/math/Color.hh>
#include <ignition/math/Pose3.hh>
#include <ignition/rendering.hh>

#include "ignition/gui/Iface.hh"
#include "ignition/gui/MainWindow.hh"
#include "ignition/gui/Plugin.hh"

using namespace ignition;
using namespace gui;

/////////////////////////////////////////////////
TEST(Scene3DTest, Load)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  EXPECT_TRUE(loadPlugin("Scene3D"));

  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(Scene3DTest, Resize)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // Load plugin
  EXPECT_TRUE(loadPlugin("Scene3D"));

  // Create main window
  EXPECT_TRUE(createMainWindow());

  // Close window after some time
  auto win = mainWindow();
  ASSERT_NE(nullptr, win);

  QTimer::singleShot(300, [&win]()
  {
    // Check there are no segfaults when resizing
    for (auto i : {100, 300, 200, 500, 400})
    {
      win->resize(i + (qrand() % 100), i + (qrand() % 100));
      QCoreApplication::processEvents();
    }
    win->close();
  });

  // Show window
  EXPECT_TRUE(runMainWindow());

  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(Scene3DTest, Config)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // Load plugin
  const char *pluginStr =
    "<plugin filename=\"Scene3D\">"
      "<engine>ogre</engine>"
      "<scene>banana</scene>"
      "<ambient_light>1.0 0 0</ambient_light>"
      "<background_color>0 1 0</background_color>"
      "<camera_pose>1 2 3 0 0 1.57</camera_pose>"
    "</plugin>";

  tinyxml2::XMLDocument pluginDoc;
  pluginDoc.Parse(pluginStr);
  EXPECT_TRUE(ignition::gui::loadPlugin("Scene3D",
      pluginDoc.FirstChildElement("plugin")));

  // Create main window
  EXPECT_TRUE(createMainWindow());

  // Check scene
  auto engine = rendering::engine("ogre");
  ASSERT_NE(nullptr, engine);

  auto scene = engine->SceneByName("banana");
  ASSERT_NE(nullptr, scene);

  EXPECT_EQ(math::Color(0, 1, 0), scene->BackgroundColor());
  EXPECT_EQ(math::Color(1, 0, 0), scene->AmbientLight());

  auto root = scene->RootVisual();
  ASSERT_NE(nullptr, root);
  EXPECT_EQ(1u, root->ChildCount());

  // Check camera
  auto camera = std::dynamic_pointer_cast<rendering::Camera>(
      root->ChildByIndex(0));
  ASSERT_NE(nullptr, camera);

  EXPECT_EQ(math::Pose3d(1, 2, 3, 0, 0, 1.57), camera->WorldPose());

  EXPECT_TRUE(stop());
}

