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

#include "test_config.h"  // NOLINT(build/include)

#include "ignition/gui/Application.hh"
#include "ignition/gui/Plugin.hh"
#include "ignition/gui/MainWindow.hh"

int g_argc = 1;
char **g_argv = new char *[g_argc];

const char * g_plugins[] = {
  "Grid3D", 
  "ImageDisplay", 
  "KeyPublisher", 
  "TransportPlotting", 
  "Publisher",
  "Scene3D",
  "Screenshot",
  "TopicEcho",
  "TopicViewer",
  "WorldControl",
  "WorldStats"
};


/////////////////////////////////////////////////
class PluginTest : 
  public ::testing::TestWithParam<const char*> {};

/////////////////////////////////////////////////
TEST_P(PluginTest, LoadPlugin)
{
  const std::string plugin_name = GetParam();

  ignition::common::Console::SetVerbosity(4);

  ignition::gui::Application app(g_argc, g_argv);
  app.AddPluginPath(ignition::testing::BinaryPath());

  ASSERT_TRUE(app.LoadPlugin(plugin_name));

  // Get main window
  auto win = app.findChild<ignition::gui::MainWindow *>();
  ASSERT_NE(nullptr, win);

  // Get plugin
  auto plugins = win->findChildren<ignition::gui::Plugin *>();
  ASSERT_EQ(plugins.size(), 1);

  auto plugin = plugins[0];
  //EXPECT_EQ(plugin->Title(), plugin_name);

  // Cleanup
  plugins.clear();
}

INSTANTIATE_TEST_SUITE_P(LoadAllPlugins, PluginTest,
    ::testing::ValuesIn(g_plugins));
