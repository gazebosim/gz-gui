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
#include <gz/common/Console.hh>
#include <gz/math/Color.hh>
#include <gz/math/Pose3.hh>
#include <gz/rendering/Camera.hh>
#include <gz/rendering/RenderEngine.hh>
#include <gz/rendering/RenderingIface.hh>
#include <gz/rendering/Scene.hh>
#include <gz/utils/ExtraTestMacros.hh>

#include "test_config.hh"  // NOLINT(build/include)
#include "../helpers/TestHelper.hh"
#include "gz/gui/Application.hh"
#include "gz/gui/GuiEvents.hh"
#include "gz/gui/Plugin.hh"
#include "gz/gui/MainWindow.hh"

int g_argc = 1;
char* g_argv[] =
{
  reinterpret_cast<char*>(const_cast<char*>("./MinimalScene_TEST")),
};

using namespace gz;
using namespace gui;

/////////////////////////////////////////////////
TEST(MinimalSceneTest, GZ_UTILS_TEST_ENABLED_ONLY_ON_LINUX(Load))
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
TEST(MinimalSceneTest, GZ_UTILS_TEST_ENABLED_ONLY_ON_LINUX(Config))
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
      "<camera_clip>"
      "  <near>0.1</near>"
      "  <far>5000</far>"
      "</camera_clip>"
      "<horizontal_fov>60</horizontal_fov>"
      "<view_controller>ortho</view_controller>"
    "</plugin>";

  tinyxml2::XMLDocument pluginDoc;
  pluginDoc.Parse(pluginStr);
  EXPECT_TRUE(app.LoadPlugin("MinimalScene",
      pluginDoc.FirstChildElement("plugin")));
  EXPECT_TRUE(app.LoadPlugin("InteractiveViewControl"));

  // Get main window
  auto win = app.findChild<MainWindow *>();
  ASSERT_NE(nullptr, win);

  // Show, but don't exec, so we don't block
  win->QuickWindow()->show();

  // Filter events
  bool receivedPreRenderEvent{false};
  bool receivedRenderEvent{false};
  auto testHelper = std::make_unique<TestHelper>();
  testHelper->forwardEvent = [&](QEvent *_event)
  {
    if (_event->type() == events::PreRender::kType)
    {
      receivedPreRenderEvent = true;
    }
    if (_event->type() == events::Render::kType)
    {
      receivedRenderEvent = true;
    }
  };

  // Check scene
  auto engine = rendering::engine("ogre");
  ASSERT_NE(nullptr, engine);

  int sleep = 0;
  int maxSleep = 30;
  while (!receivedRenderEvent && sleep < maxSleep)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    QCoreApplication::processEvents();
    ++sleep;
  }
  EXPECT_TRUE(receivedPreRenderEvent);
  EXPECT_TRUE(receivedRenderEvent);

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
  EXPECT_DOUBLE_EQ(0.1, camera->NearClipPlane());
  EXPECT_DOUBLE_EQ(5000.0, camera->FarClipPlane());

  EXPECT_NEAR(60, camera->HFOV().Degree(), 1e-4);

  EXPECT_EQ(rendering::CameraProjectionType::CPT_ORTHOGRAPHIC,
            camera->ProjectionType());

  // Cleanup
  auto plugins = win->findChildren<Plugin *>();
  EXPECT_EQ(2, plugins.size());

  auto pluginName = plugins[0]->CardItem()->objectName().toStdString();
  EXPECT_TRUE(app.RemovePlugin(pluginName));
  plugins.clear();

  win->QuickWindow()->close();
  engine->DestroyScene(scene);
  EXPECT_TRUE(rendering::unloadEngine(engine->Name()));
}
