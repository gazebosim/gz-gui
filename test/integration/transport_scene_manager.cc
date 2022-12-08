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
#include <gz/msgs/pose_v.pb.h>
#include <gz/msgs/scene.pb.h>
#include <gz/msgs/uint32_v.pb.h>
#include <gz/rendering/Camera.hh>
#include <gz/rendering/RenderEngine.hh>
#include <gz/rendering/RenderingIface.hh>
#include <gz/rendering/Scene.hh>
#include <gz/transport/Node.hh>
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
  reinterpret_cast<char*>(const_cast<char*>("./TransportSceneManager_TEST")),
};

using namespace gz;
using namespace gui;

/////////////////////////////////////////////////
TEST(TransportSceneManagerTest, GZ_UTILS_TEST_ENABLED_ONLY_ON_LINUX(Load))
{
  common::Console::SetVerbosity(4);

  Application app(g_argc, g_argv);
  app.AddPluginPath(std::string(PROJECT_BINARY_PATH) + "/lib");

  EXPECT_TRUE(app.LoadPlugin("MinimalScene"));
  EXPECT_TRUE(app.LoadPlugin("TransportSceneManager"));

  // Get main window
  auto win = app.findChild<MainWindow *>();
  ASSERT_NE(nullptr, win);

  // Get plugin
  auto plugins = win->findChildren<Plugin *>();
  EXPECT_EQ(plugins.size(), 2);

  auto plugin = plugins[0];
  EXPECT_EQ(plugin->Title(), "3D Scene");

  plugin = plugins[1];
  EXPECT_EQ(plugin->Title(), "Transport Scene Manager");

  // Cleanup
  for (const auto &p : plugins)
  {
    auto pluginName = p->CardItem()->objectName().toStdString();
    EXPECT_TRUE(app.RemovePlugin(pluginName));
  }
  plugins.clear();
}

/////////////////////////////////////////////////
TEST(TransportSceneManagerTest, GZ_UTILS_TEST_ENABLED_ONLY_ON_LINUX(Config))
{
  bool sceneRequested{false};
  std::function<bool(msgs::Scene &)> sceneService =
    [&](msgs::Scene &_rep) -> bool
  {
    auto modelMsg = _rep.add_model();
    modelMsg->set_id(1);
    modelMsg->set_is_static(true);
    modelMsg->set_name("box_model");

    auto linkMsg = modelMsg->add_link();
    linkMsg->set_id(2);
    linkMsg->set_name("box_link");

    auto visMsg = linkMsg->add_visual();
    visMsg->set_id(3);
    visMsg->set_name("box_vis");

    auto geomMsg = visMsg->mutable_geometry();
    auto boxMsg = geomMsg->mutable_box();
    auto boxSize = boxMsg->mutable_size();
    boxSize->set_x(1.0);
    boxSize->set_y(2.0);
    boxSize->set_z(3.0);

    sceneRequested = true;
    return true;
  };

  // Scene service
  transport::Node node;
  node.Advertise<msgs::Scene>("/test/scene", sceneService);

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
      "<camera_pose>1 2 3 0 0 1.57</camera_pose>"
    "</plugin>";

  tinyxml2::XMLDocument pluginDoc;
  pluginDoc.Parse(pluginStr);
  EXPECT_TRUE(app.LoadPlugin("MinimalScene",
      pluginDoc.FirstChildElement("plugin")));

  pluginStr =
    "<plugin filename=\"TransportSceneManager\">"
      "<service>/test/scene</service>"
      "<pose_topic>/test/pose</pose_topic>"
      "<deletion_topic>/test/delete</deletion_topic>"
      "<scene_topic>/test/scene</scene_topic>"
    "</plugin>";

  pluginDoc.Parse(pluginStr);
  EXPECT_TRUE(app.LoadPlugin("TransportSceneManager",
      pluginDoc.FirstChildElement("plugin")));

  // Get main window
  auto win = app.findChild<MainWindow *>();
  ASSERT_NE(nullptr, win);

  // Show, but don't exec, so we don't block
  win->QuickWindow()->show();

  // Get scene
  auto engine = rendering::engine("ogre");
  ASSERT_NE(nullptr, engine);

  int sleep = 0;
  int maxSleep = 30;
  while (!sceneRequested && sleep < maxSleep)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    QCoreApplication::processEvents();
    sleep++;
  }
  EXPECT_TRUE(sceneRequested);
  EXPECT_LT(sleep, maxSleep);

  auto scene = engine->SceneByName("banana");
  ASSERT_NE(nullptr, scene);

  auto root = scene->RootVisual();
  ASSERT_NE(nullptr, root);

  for (sleep = 0; root->ChildCount() < 2 && sleep < maxSleep; ++sleep)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    QCoreApplication::processEvents();
  }
  EXPECT_LT(sleep, maxSleep);

  // Check scene is populated
  ASSERT_EQ(2u, root->ChildCount());

  // First child is user camera
  auto camera = std::dynamic_pointer_cast<rendering::Camera>(
      root->ChildByIndex(0));
  EXPECT_NE(nullptr, camera);

  // Check box
  auto modelVis = std::dynamic_pointer_cast<rendering::Visual>(
      root->ChildByIndex(1));
  ASSERT_NE(nullptr, modelVis);
  EXPECT_EQ(math::Pose3d::Zero, modelVis->LocalPose());
  EXPECT_EQ(1u, modelVis->ChildCount());
  EXPECT_EQ(0u, modelVis->GeometryCount());

  auto linkVis = std::dynamic_pointer_cast<rendering::Visual>(
      modelVis->ChildByIndex(0));
  ASSERT_NE(nullptr, linkVis);
  EXPECT_EQ(math::Pose3d::Zero, linkVis->LocalPose());
  EXPECT_EQ(1u, linkVis->ChildCount());
  EXPECT_EQ(0u, linkVis->GeometryCount());

  auto visualVis = std::dynamic_pointer_cast<rendering::Visual>(
      linkVis->ChildByIndex(0));
  ASSERT_NE(nullptr, visualVis);
  EXPECT_EQ(math::Pose3d::Zero, visualVis->LocalPose());
  EXPECT_EQ(0u, visualVis->ChildCount());
  EXPECT_EQ(1u, visualVis->GeometryCount());

  // Change model pose
  auto posePub = node.Advertise<gz::msgs::Pose_V>("/test/pose");

  gz::msgs::Pose_V poseVMsg;
  auto poseMsg = poseVMsg.add_pose();
  poseMsg->set_id(1);
  poseMsg->set_name("box_model");
  auto positionMsg = poseMsg->mutable_position();
  positionMsg->set_x(5);
  posePub.Publish(poseVMsg);

  sleep = 0;
  while (modelVis->LocalPose() == math::Pose3d::Zero && sleep < maxSleep)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    QCoreApplication::processEvents();
    sleep++;
  }
  EXPECT_EQ(math::Pose3d(5, 0, 0, 0, 0, 0), modelVis->LocalPose());

  // Delete model
  auto deletePub = node.Advertise<gz::msgs::UInt32_V>("/test/delete");

  gz::msgs::UInt32_V entityVMsg;
  entityVMsg.add_data(1);
  deletePub.Publish(entityVMsg);

  sleep = 0;
  while (root->ChildCount() > 1 && sleep < maxSleep)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    QCoreApplication::processEvents();
    sleep++;
  }
  EXPECT_EQ(1u, root->ChildCount());

  // Cleanup
  auto plugins = win->findChildren<Plugin *>();
  EXPECT_EQ(plugins.size(), 2);

  for (const auto &p : plugins)
  {
    auto pluginName = p->CardItem()->objectName().toStdString();
    EXPECT_TRUE(app.RemovePlugin(pluginName));
  }
  plugins.clear();

  win->QuickWindow()->close();
  engine->DestroyScene(scene);
  EXPECT_TRUE(rendering::unloadEngine(engine->Name()));
}

