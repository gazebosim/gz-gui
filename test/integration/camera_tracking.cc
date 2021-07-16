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

#include <ignition/common/Console.hh>
#include <ignition/math/Pose3.hh>
#include <ignition/msgs/pose.pb.h>
#include <ignition/rendering/Camera.hh>
#include <ignition/rendering/RenderEngine.hh>
#include <ignition/rendering/RenderingIface.hh>
#include <ignition/rendering/Scene.hh>
#include <ignition/transport/Node.hh>
#include <ignition/utilities/ExtraTestMacros.hh>

#include "test_config.h"  // NOLINT(build/include)
#include "ignition/gui/Application.hh"
#include "ignition/gui/GuiEvents.hh"
#include "ignition/gui/Plugin.hh"
#include "ignition/gui/MainWindow.hh"

int g_argc = 1;
char* g_argv[] =
{
  reinterpret_cast<char*>(const_cast<char*>("./camera_tracking")),
};

using namespace ignition;
using namespace gui;
using namespace std::chrono_literals;

/////////////////////////////////////////////////
TEST(MinimalSceneTest, IGN_UTILS_TEST_ENABLED_ONLY_ON_LINUX(Config))
{
  common::Console::SetVerbosity(4);

  Application app(g_argc, g_argv);
  app.AddPluginPath(std::string(PROJECT_BINARY_PATH) + "/lib");

  // Load plugins
  const char *pluginStr =
    "<plugin filename=\"MinimalScene\">"
      "<engine>ogre</engine>"
      "<scene>banana</scene>"
      "<ambient_light>1.0 0 0</ambient_light>"
      "<background_color>0 1 0</background_color>"
      "<camera_pose>1 2 3 0 0 0</camera_pose>"
    "</plugin>";

  tinyxml2::XMLDocument pluginDoc;
  pluginDoc.Parse(pluginStr);
  EXPECT_TRUE(app.LoadPlugin("MinimalScene",
      pluginDoc.FirstChildElement("plugin")));

  pluginStr =
    "<plugin filename=\"CameraTracking\">"
    "</plugin>";

  pluginDoc.Parse(pluginStr);
  EXPECT_TRUE(app.LoadPlugin("CameraTracking",
      pluginDoc.FirstChildElement("plugin")));

  // Get main window
  auto win = app.findChild<MainWindow *>();
  ASSERT_NE(nullptr, win);

  // Get plugin
  auto plugins = win->findChildren<Plugin *>();
  EXPECT_EQ(plugins.size(), 2);

  auto plugin = plugins[0];
  EXPECT_EQ(plugin->Title(), "3D Scene");

  plugin = plugins[1];
  EXPECT_EQ(plugin->Title(), "Camera tracking");

  // Show, but don't exec, so we don't block
  win->QuickWindow()->show();

  // Get camera pose
  msgs::Pose poseMsg;
  auto poseCb = std::function<void(const msgs::Pose &)>(
      [&](const auto &_msg)
      {
        poseMsg = _msg;
      });

  transport::Node node;
  node.Subscribe("/gui/camera/pose", poseCb);

  int sleep = 0;
  int maxSleep = 30;
  while (!poseMsg.has_position() && sleep++ < maxSleep)
  {
    std::this_thread::sleep_for(100ms);
    QCoreApplication::processEvents();
  }
  EXPECT_LT(sleep, maxSleep);
  EXPECT_TRUE(poseMsg.has_position());
  EXPECT_TRUE(poseMsg.has_orientation());

  auto engine = rendering::engine("ogre");
  ASSERT_NE(nullptr, engine);

  auto scene = engine->SceneByName("banana");
  ASSERT_NE(nullptr, scene);

  auto root = scene->RootVisual();
  ASSERT_NE(nullptr, root);

  auto camera = std::dynamic_pointer_cast<rendering::Camera>(
      root->ChildByIndex(0));
  ASSERT_NE(nullptr, camera);

  EXPECT_EQ(camera->WorldPose(), msgs::Convert(poseMsg));
  EXPECT_EQ(math::Pose3d(1, 2, 3, 0, 0, 0), msgs::Convert(poseMsg));

  // Add object to be tracked
  auto trackedVis = scene->CreateVisual("track_me");
  ASSERT_NE(nullptr, trackedVis);
  trackedVis->SetWorldPose({100, 100, 100, 0, 0, 0});

  // Move to
  msgs::StringMsg req;
  msgs::Boolean rep;

  req.set_data("track_me");

  bool result;
  unsigned int timeout = 2000;
  bool executed = node.Request("/gui/move_to", req, timeout, rep, result);
  EXPECT_TRUE(executed);
  EXPECT_TRUE(result);
  EXPECT_TRUE(rep.data());

  sleep = 0;
  while (abs(camera->WorldPose().Pos().X() - 100) > 10 && sleep++ < maxSleep)
  {
    std::this_thread::sleep_for(100ms);
    QCoreApplication::processEvents();
  }
  EXPECT_LT(sleep, maxSleep);

  EXPECT_GT(10, abs(camera->WorldPose().Pos().X() - 100));
  EXPECT_GT(10, abs(camera->WorldPose().Pos().Y() - 100));
  EXPECT_GT(10, abs(camera->WorldPose().Pos().Z() - 100));

  // Move target object to new position
  trackedVis->SetWorldPose({130, 130, 130, 0, 0, 0});

  // Follow
  result = false;
  executed = node.Request("/gui/follow", req, timeout, rep, result);
  EXPECT_TRUE(executed);
  EXPECT_TRUE(result);
  EXPECT_TRUE(rep.data());

  msgs::Vector3d reqOffset;
  reqOffset.set_x(1.0);
  reqOffset.set_y(1.0);
  reqOffset.set_z(1.0);
  result = false;
  executed = node.Request("/gui/follow/offset", reqOffset, timeout, rep,
      result);
  EXPECT_TRUE(executed);
  EXPECT_TRUE(result);
  EXPECT_TRUE(rep.data());

  // Many update loops to process many events
  maxSleep = 300;
  for (auto it : {150.0, 200.0})
  {
    // Move target
    trackedVis->SetWorldPose({it, it, it, 0, 0, 0});

    // Check camera moved
    sleep = 0;
    while (abs(camera->WorldPose().Pos().X() - it) > 10 &&
        sleep++ < maxSleep)
    {
      std::this_thread::sleep_for(10ms);
      QCoreApplication::processEvents();
    }
    EXPECT_LT(sleep, maxSleep);

    EXPECT_GT(10, abs(camera->WorldPose().Pos().X() - it));
    EXPECT_GT(10, abs(camera->WorldPose().Pos().Y() - it));
    EXPECT_GT(10, abs(camera->WorldPose().Pos().Z() - it));
  }
}

