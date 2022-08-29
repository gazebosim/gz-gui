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
#include <string>

#include <gz/msgs/world_stats.pb.h>
#include <gz/msgs/marker.pb.h>
#include <gz/msgs/material.pb.h>

#include <gz/common/Console.hh>
#include <gz/common/Filesystem.hh>
#include <gz/msgs/Utility.hh>
#include <gz/rendering/RenderEngine.hh>
#include <gz/rendering/RenderingIface.hh>
#include <gz/rendering/Scene.hh>
#include <gz/transport/Node.hh>
#include <gz/utils/ExtraTestMacros.hh>

#include "test_config.hh"  // NOLINT(build/include)
#include "gz/gui/Application.hh"
#include "gz/gui/GuiEvents.hh"
#include "gz/gui/MainWindow.hh"
#include "gz/gui/Plugin.hh"

int g_argc = 1;
char* g_argv[] =
{
  reinterpret_cast<char*>(const_cast<char*>("./MarkerMmanager_TEST")),
};

using namespace std::chrono_literals;

using namespace gz;
using namespace gui;

class MarkerManagerTestFixture : public ::testing::Test
{

  public:
    gz::transport::Node node;
    rendering::ScenePtr scene;
    gz::transport::Node::Publisher statsPub;

    MarkerManagerTestFixture()
    {
      // Periodic world statistics
      statsPub =
        node.Advertise<gz::msgs::WorldStatistics>("/example/stats");
    }

    void sendWorldStatisticsMsg(std::chrono::steady_clock::duration &timePoint)
    {
      gz::msgs::WorldStatistics msgWorldStatistics;

      msgWorldStatistics.set_real_time_factor(1);
      auto s = std::chrono::duration_cast<std::chrono::seconds>(timePoint);
      auto ns =
        std::chrono::duration_cast<std::chrono::nanoseconds>(timePoint-s);

      msgWorldStatistics.mutable_sim_time()->set_sec(s.count());
      msgWorldStatistics.mutable_sim_time()->set_nsec(ns.count());
      statsPub.Publish(msgWorldStatistics);
    }

    void waitAndSendStatsMsgs(
      std::chrono::steady_clock::duration &timePoint,
      uint expectedValue,
      int maxSleep)
  {

    // Give it time to be processed
    int sleep = 0;
    while (scene->VisualCount() != expectedValue && sleep < maxSleep)
    {
      timePoint += 100ms;

      sendWorldStatisticsMsg(timePoint);

      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      QCoreApplication::processEvents();
      sleep++;
    }
  }
};

/////////////////////////////////////////////////
TEST_F(MarkerManagerTestFixture,
  GZ_UTILS_TEST_ENABLED_ONLY_ON_LINUX(MarkerManager))
{
  common::Console::SetVerbosity(4);

  // Load the plugin
  Application app(g_argc, g_argv);
  app.AddPluginPath(std::string(PROJECT_BINARY_PATH) + "/lib");
  // Load plugin
  const char *pluginStr =
    "<plugin filename=\"MarkerManager\">"
      "<stats_topic>/example/stats</stats_topic>"
    "</plugin>";

  const char *pluginMinimalSceneStr =
    "<plugin filename=\"MinimalScene\">"
      "<engine>ogre</engine>"
      "<scene>scene</scene>"
    "</plugin>";

  tinyxml2::XMLDocument pluginDoc;
  EXPECT_EQ(tinyxml2::XML_SUCCESS, pluginDoc.Parse(pluginStr));

  tinyxml2::XMLDocument pluginDocMinimalScene;
  EXPECT_EQ(tinyxml2::XML_SUCCESS,
    pluginDocMinimalScene.Parse(pluginMinimalSceneStr));

  EXPECT_TRUE(app.LoadPlugin("MinimalScene",
      pluginDocMinimalScene.FirstChildElement("plugin")));
  EXPECT_TRUE(app.LoadPlugin("MarkerManager",
      pluginDoc.FirstChildElement("plugin")));

  // Get main window
  auto window = app.findChild<MainWindow *>();
  ASSERT_NE(window, nullptr);

  // Get plugin
  auto plugins = window->findChildren<Plugin *>();
  EXPECT_EQ(plugins.size(), 2);

  // Show, but don't exec, so we don't block
  window->QuickWindow()->show();

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
  scene = engine->SceneByName("scene");
  ASSERT_NE(nullptr, scene);

  std::chrono::steady_clock::duration timePoint =
    std::chrono::steady_clock::duration::zero();

  // Create the marker message
  gz::msgs::Marker markerMsg;
  gz::msgs::Material matMsg;
  markerMsg.set_ns("default");
  markerMsg.set_id(0);
  markerMsg.set_action(gz::msgs::Marker::ADD_MODIFY);
  markerMsg.set_type(gz::msgs::Marker::SPHERE);
  markerMsg.set_visibility(gz::msgs::Marker::GUI);

  // Add a sphere that will be remove after 2 seconds
  markerMsg.mutable_material()->mutable_ambient()->set_r(0);
  markerMsg.mutable_material()->mutable_ambient()->set_g(0);
  markerMsg.mutable_material()->mutable_ambient()->set_b(1);
  markerMsg.mutable_material()->mutable_ambient()->set_a(1);
  markerMsg.mutable_material()->mutable_diffuse()->set_r(0);
  markerMsg.mutable_material()->mutable_diffuse()->set_g(0);
  markerMsg.mutable_material()->mutable_diffuse()->set_b(1);
  markerMsg.mutable_material()->mutable_diffuse()->set_a(1);
  markerMsg.mutable_lifetime()->set_sec(0);
  markerMsg.mutable_lifetime()->set_nsec(0);
  gz::msgs::Set(markerMsg.mutable_scale(),
                    gz::math::Vector3d(1.0, 1.0, 1.0));

  gz::msgs::Set(markerMsg.mutable_pose(),
                      gz::math::Pose3d(2, 2, 0, 0, 0, 0));
  EXPECT_EQ(0u, scene->VisualCount());

  // Wait 2 seconds, plugins need to be initialized
  std::this_thread::sleep_for(std::chrono::milliseconds(2000));

  bool executed = node.Request("/marker", markerMsg);
  if (executed)
  {
    gzdbg << "/marker request sent" << std::endl;
    waitAndSendStatsMsgs(timePoint, 1, 200);
    EXPECT_EQ(1u, scene->VisualCount());
  }
  else
  {
    FAIL();
  }

  markerMsg.set_action(gz::msgs::Marker::DELETE_ALL);
  executed = node.Request("/marker", markerMsg);
  if (executed)
  {
    waitAndSendStatsMsgs(timePoint, 0, 200);
    EXPECT_EQ(0u, scene->VisualCount());
  }
  else
  {
    FAIL();
  }

  // Cleanup
  plugins.clear();
}
