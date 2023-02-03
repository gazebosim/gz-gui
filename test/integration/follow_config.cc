/*
 * Copyright (C) 2023 Rudis Laboratories LLC
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

#include <gz/msgs/boolean.pb.h>
#include <gz/msgs/double.pb.h>
#include <gz/msgs/pose.pb.h>
#include <gz/msgs/stringmsg.pb.h>
#include <gz/msgs/vector3d.pb.h>

#include <gz/common/Console.hh>
#include <gz/math/Pose3.hh>

#include <gz/msgs/Utility.hh>
#include <gz/rendering/Camera.hh>
#include <gz/rendering/RenderEngine.hh>
#include <gz/rendering/RenderingIface.hh>
#include <gz/rendering/Scene.hh>
#include <gz/transport/Node.hh>
#include <gz/utils/ExtraTestMacros.hh>

#include "test_config.hh"  // NOLINT(build/include)
#include "gz/gui/Application.hh"
#include "gz/gui/GuiEvents.hh"
#include "gz/gui/Plugin.hh"
#include "gz/gui/MainWindow.hh"

int g_argc = 1;
char* g_argv[] =
{
  reinterpret_cast<char*>(const_cast<char*>("./follow_config")),
};

using namespace gz;
using namespace gui;
using namespace std::chrono_literals;

/////////////////////////////////////////////////
TEST(MinimalSceneTest, GZ_UTILS_TEST_ENABLED_ONLY_ON_LINUX(Config))
{
  common::Console::SetVerbosity(4);

  Application app(g_argc, g_argv);
  app.AddPluginPath(gz::common::joinPaths(
    std::string(PROJECT_BINARY_PATH), "lib"));

  // Load plugins
  const char *pluginStr =
    "<plugin filename=\"MinimalScene\">"
      "<engine>ogre</engine>"
      "<scene>banana</scene>"
      "<ambient_light>1.0 0 0</ambient_light>"
      "<background_color>0 1 0</background_color>"
      "<camera_pose>0 0 0 0 0 0</camera_pose>"
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

  pluginStr =
    "<plugin filename=\"FollowConfig\">"
      "<follow_target>track_me</follow_target>"
      "<follow_offset>0.0 0.0 0.0</follow_offset>"
      "<follow_pgain>1.0</follow_pgain>"
    "</plugin>";

  pluginDoc.Parse(pluginStr);
  EXPECT_TRUE(app.LoadPlugin("FollowConfig",
      pluginDoc.FirstChildElement("plugin")));

  // Get main window
  auto win = app.findChild<MainWindow *>();
  ASSERT_NE(nullptr, win);

  // Get plugin
  auto plugins = win->findChildren<Plugin *>();

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
  int maxSleep = 60;
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

  auto trackedVis = scene->CreateVisual("track_me");
  ASSERT_NE(nullptr, trackedVis);
  trackedVis->SetWorldPose({0, 0, 0, 0, 0, 0});

  auto root = scene->RootVisual();
  ASSERT_NE(nullptr, root);

  auto camera = std::dynamic_pointer_cast<rendering::Camera>(
      root->ChildByIndex(0));
  ASSERT_NE(nullptr, camera);

  msgs::StringMsg req;
  msgs::Boolean rep;
  req.set_data("track_me");

  bool result;
  unsigned int timeout = 2000;
  bool executed = node.Request("/gui/follow", req, timeout, rep, result);
  EXPECT_TRUE(executed);
  EXPECT_TRUE(result);
  EXPECT_TRUE(rep.data());

  sleep = 0;
  while (abs(poseMsg.mutable_position()->x()) < .01  && sleep++ < maxSleep)
  {
    std::this_thread::sleep_for(100ms);
    QCoreApplication::processEvents();
  }
  EXPECT_LT(sleep, maxSleep);

  EXPECT_NEAR(0.0, abs(poseMsg.mutable_position()->x()), 1.0);
  EXPECT_NEAR(0.0, abs(poseMsg.mutable_position()->y()), 1.0);
  EXPECT_NEAR(0.0, abs(poseMsg.mutable_position()->z()), 1.0);
}
