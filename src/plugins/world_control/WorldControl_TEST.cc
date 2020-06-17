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
#include <ignition/transport/Node.hh>

#include "test_config.h"  // NOLINT(build/include)
#include "ignition/gui/Application.hh"
#include "ignition/gui/Plugin.hh"
#include "ignition/gui/MainWindow.hh"
#include "WorldControl.hh"

int g_argc = 1;
char **g_argv = new char *[g_argc];

using namespace ignition;
using namespace gui;

/////////////////////////////////////////////////
TEST(WorldControlTest, Load)
{
  common::Console::SetVerbosity(4);

  Application app(g_argc, g_argv);
  app.AddPluginPath(std::string(PROJECT_BINARY_PATH) + "/lib");

  EXPECT_TRUE(app.LoadPlugin("WorldControl"));

  // Get main window
  auto win = app.findChild<MainWindow *>();
  ASSERT_NE(nullptr, win);

  // Get plugin
  auto plugins = win->findChildren<Plugin *>();
  EXPECT_EQ(plugins.size(), 1);

  auto plugin = plugins[0];
  EXPECT_EQ(plugin->Title(), "World control");

  // Cleanup
  plugins.clear();
}

/////////////////////////////////////////////////
TEST(WorldControlTest, WorldControl)
{
  common::Console::SetVerbosity(4);

  Application app(g_argc, g_argv);
  app.AddPluginPath(std::string(PROJECT_BINARY_PATH) + "/lib");

  // Load plugin
  const char *pluginStr =
    "<plugin filename=\"WorldControl\">"
      "<ignition-gui>"
        "<title>World Control!</title>"
      "</ignition-gui>"
      "<play_pause>true</play_pause>"
      "<service>/world_control_test</service>"
    "</plugin>";

  tinyxml2::XMLDocument pluginDoc;
  EXPECT_EQ(tinyxml2::XML_SUCCESS, pluginDoc.Parse(pluginStr));
  EXPECT_TRUE(app.LoadPlugin("WorldControl",
      pluginDoc.FirstChildElement("plugin")));

  // Get main window
  auto win = app.findChild<MainWindow *>();
  ASSERT_NE(nullptr, win);

  // Show, but don't exec, so we don't block
  win->QuickWindow()->show();

  // Get plugin
  auto plugins = win->findChildren<plugins::WorldControl *>();
  EXPECT_EQ(plugins.size(), 1);

  auto plugin = plugins[0];
  EXPECT_EQ(plugin->Title(), "World Control!");

  // World control service
  bool playCalled = false;
  bool pauseCalled = false;
  bool multiStepCalled = false;
  std::function<bool(const msgs::WorldControl &, msgs::Boolean &)> cb =
      [&](const msgs::WorldControl &_req, msgs::Boolean &)
  {
    pauseCalled = _req.pause();
    playCalled = !_req.pause();
    multiStepCalled = _req.multi_step() > 0;
    return true;
  };
  transport::Node node;
  node.Advertise("/world_control_test", cb);

  // Pause
  plugin->OnPause();
  EXPECT_TRUE(pauseCalled);

  // Step
  plugin->OnStep();
  EXPECT_TRUE(multiStepCalled);

  // Play
  plugin->OnPlay();
  EXPECT_TRUE(playCalled);

  // Cleanup
  plugins.clear();
}

