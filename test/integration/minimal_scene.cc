/*
 * Copyright (C) 2021 Open Source Robotics Foundation
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
#include <QtTest/QtTest>
#include <ignition/common/Console.hh>
#include <ignition/math/Color.hh>
#include <ignition/math/Pose3.hh>
#include <ignition/rendering/Camera.hh>
#include <ignition/rendering/RenderEngine.hh>
#include <ignition/rendering/RenderingIface.hh>
#include <ignition/rendering/Scene.hh>
#include <ignition/utilities/ExtraTestMacros.hh>

#include "test_config.h"  // NOLINT(build/include)
#include "../helpers/TestHelper.hh"
#include "ignition/gui/Application.hh"
#include "ignition/gui/GuiEvents.hh"
#include "ignition/gui/Plugin.hh"
#include "ignition/gui/MainWindow.hh"

int g_argc = 1;
char **g_argv = new char *[g_argc];

using namespace ignition;
using namespace gui;

/////////////////////////////////////////////////
TEST(MinimalSceneTest, IGN_UTILS_TEST_ENABLED_ONLY_ON_LINUX(Load))
{
  common::Console::SetVerbosity(4);

  Application app(g_argc, g_argv);
  app.AddPluginPath(std::string(PROJECT_BINARY_PATH) + "/lib");

  EXPECT_TRUE(app.LoadPlugin("MinimalScene"));

  // Get main window
  auto win = app.findChild<MainWindow *>();
  ASSERT_NE(nullptr, win);

  // Get plugin
  auto plugins = win->findChildren<Plugin *>();
  EXPECT_EQ(plugins.size(), 1);

  auto plugin = plugins[0];
  EXPECT_EQ(plugin->Title(), "3D Scene");

  // Cleanup
  auto pluginName = plugin->CardItem()->objectName().toStdString();
  EXPECT_TRUE(app.RemovePlugin(pluginName));
  plugins.clear();
}

/////////////////////////////////////////////////
TEST(MinimalSceneTest, IGN_UTILS_TEST_ENABLED_ONLY_ON_LINUX(Config))
{
  common::Console::SetVerbosity(4);

  Application app(g_argc, g_argv);
  app.AddPluginPath(std::string(PROJECT_BINARY_PATH) + "/lib");

  // Load plugin
  const char *pluginStr =
    "<plugin filename=\"MinimalScene\">"
      "<engine>ogre</engine>"
      "<scene>banana</scene>"
      "<ambient_light>1.0 0 0</ambient_light>"
      "<background_color>0 1 0</background_color>"
      "<camera_pose>1 2 3 0 0 1.57</camera_pose>"
    "</plugin>";

  tinyxml2::XMLDocument pluginDoc;
  pluginDoc.Parse(pluginStr);
  EXPECT_TRUE(app.LoadPlugin("MinimalScene",
      pluginDoc.FirstChildElement("plugin")));

  // Get main window
  auto win = app.findChild<MainWindow *>();
  ASSERT_NE(nullptr, win);

  // Show, but don't exec, so we don't block
  win->QuickWindow()->show();

  // Check scene
  auto engine = rendering::engine("ogre");
  ASSERT_NE(nullptr, engine);

  int sleep = 0;
  int maxSleep = 30;
  while (0 == engine->SceneCount() && sleep < maxSleep)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    QCoreApplication::processEvents();
    sleep++;
  }

  EXPECT_EQ(1u, engine->SceneCount());
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

  // Cleanup
  auto plugins = win->findChildren<Plugin *>();
  EXPECT_EQ(1, plugins.size());

  auto pluginName = plugins[0]->CardItem()->objectName().toStdString();
  EXPECT_TRUE(app.RemovePlugin(pluginName));
  plugins.clear();

  win->QuickWindow()->close();
  engine->DestroyScene(scene);
  EXPECT_TRUE(rendering::unloadEngine(engine->Name()));
}

