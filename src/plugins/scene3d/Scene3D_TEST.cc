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
#include <ignition/utilities/ExtraTestMacros.hh>

#include "test_config.h"  // NOLINT(build/include)
#include "ignition/gui/Application.hh"
#include "ignition/gui/Plugin.hh"
#include "ignition/gui/MainWindow.hh"

#include "Scene3D.hh"

int g_argc = 1;
char **g_argv = new char *[g_argc];

using namespace ignition;
using namespace gui;

/////////////////////////////////////////////////
TEST(Scene3DTest, IGN_UTILS_TEST_ENABLED_ONLY_ON_LINUX(Load))
{
  common::Console::SetVerbosity(4);

  Application app(g_argc, g_argv);
  app.AddPluginPath(std::string(PROJECT_BINARY_PATH) + "/lib");

  EXPECT_TRUE(app.LoadPlugin("Scene3D"));

  // Get main window
  auto win = app.findChild<MainWindow *>();
  ASSERT_NE(nullptr, win);

  // Get plugin
  auto plugins = win->findChildren<Plugin *>();
  EXPECT_EQ(plugins.size(), 1);

  auto plugin = plugins[0];
  EXPECT_EQ(plugin->Title(), "3D Scene");

  // Cleanup
  plugins.clear();
}

