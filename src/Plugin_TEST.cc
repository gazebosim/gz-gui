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

#include "test_config.h"  // NOLINT(build/include)
#include "ignition/gui/Iface.hh"
#include "ignition/gui/MainWindow.hh"
#include "ignition/gui/Plugin.hh"

using namespace ignition;
using namespace gui;

/////////////////////////////////////////////////
TEST(PluginTest, DeleteLater)
{
  setVerbosity(4);
  addPluginPath(std::string(PROJECT_BINARY_PATH) + "/test/plugins");

  EXPECT_TRUE(initApp());

  // Load normal plugin
  const char *pluginStr =
    "<plugin filename=\"TestPlugin\">"
    "</plugin>";

  tinyxml2::XMLDocument pluginDoc;
  pluginDoc.Parse(pluginStr);
  EXPECT_TRUE(loadPlugin("TestPlugin",
      pluginDoc.FirstChildElement("plugin")));

  // Load normal plugin to be deleted
  pluginStr =
    "<plugin filename=\"TestPlugin\" delete_later=\"true\">"
    "</plugin>";

  pluginDoc.Parse(pluginStr);
  EXPECT_TRUE(loadPlugin("TestPlugin",
      pluginDoc.FirstChildElement("plugin")));

  // Create main window
  EXPECT_TRUE(createMainWindow());

  auto win = mainWindow();
  ASSERT_NE(nullptr, win);

  // Check plugin count
  EXPECT_EQ(1, win->findChildren<Plugin *>().size());

  EXPECT_TRUE(stop());
}

