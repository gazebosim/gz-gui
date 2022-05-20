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

#include <gz/common/Console.hh>
#include <gz/utils/ExtraTestMacros.hh>

#include "test_config.h"  // NOLINT(build/include)
#include "gz/gui/Application.hh"
#include "gz/gui/MainWindow.hh"
#include "gz/gui/Plugin.hh"

int g_argc = 1;
char* g_argv[] =
{
  reinterpret_cast<char*>(const_cast<char*>("./Plugin_TEST")),
};

using namespace gz;
using namespace gui;

// See https://github.com/gazebosim/gz-gui/issues/75
TEST(PluginTest, IGN_UTILS_TEST_ENABLED_ONLY_ON_LINUX(DeleteLater))
{
  gz::common::Console::SetVerbosity(4);

  Application app(g_argc, g_argv);
  app.AddPluginPath(std::string(PROJECT_BINARY_PATH) + "/lib");

  // Load normal plugin
  const char *pluginStr =
    "<plugin filename=\"TestPlugin\">"
    "</plugin>";

  tinyxml2::XMLDocument pluginDoc;
  pluginDoc.Parse(pluginStr);
  EXPECT_TRUE(app.LoadPlugin("TestPlugin",
      pluginDoc.FirstChildElement("plugin")));

  // Load plugin to be deleted
  pluginStr =
    "<plugin filename=\"TestPlugin\">"
      "<ignition-gui>"
        "<delete_later>true</delete_later>"
      "</ignition-gui>"
    "</plugin>";

  pluginDoc.Parse(pluginStr);

  // Create main window
  EXPECT_TRUE(app.LoadPlugin("TestPlugin",
      pluginDoc.FirstChildElement("plugin")));;

  auto win = app.findChild<MainWindow *>();
  ASSERT_NE(nullptr, win);

  // Check plugin count
  EXPECT_EQ(1, win->findChildren<Plugin *>().size());
}

/////////////////////////////////////////////////
TEST(PluginTest, IGN_UTILS_TEST_ENABLED_ONLY_ON_LINUX(InvalidXmlText))
{
  gz::common::Console::SetVerbosity(4);

  Application app(g_argc, g_argv);
  app.AddPluginPath(std::string(PROJECT_BINARY_PATH) + "/lib");

  // Load plugin config that returns null GetText
  const char *pluginStr =
    "<plugin filename=\"TestPlugin\">"
      "<ignition-gui>"
        "<title><null>This results in null titleElem->GetText</null></title>"
      "</ignition-gui>"
    "</plugin>";

  tinyxml2::XMLDocument pluginDoc;
  pluginDoc.Parse(pluginStr);
  EXPECT_TRUE(app.LoadPlugin("TestPlugin",
      pluginDoc.FirstChildElement("plugin")));

  auto win = app.findChild<MainWindow *>();
  ASSERT_NE(nullptr, win);

  // Check plugin was loaded and has empty title
  auto plugins = win->findChildren<Plugin *>();
  ASSERT_EQ(1, plugins.size());
  EXPECT_TRUE(plugins[0]->Title().empty());
}

/////////////////////////////////////////////////
TEST(PluginTest, IGN_UTILS_TEST_ENABLED_ONLY_ON_LINUX(Getters))
{
  gz::common::Console::SetVerbosity(4);

  Application app(g_argc, g_argv);
  app.AddPluginPath(std::string(PROJECT_BINARY_PATH) + "/lib");

  // Load normal plugin
  const char *pluginStr =
    "<plugin filename=\"TestPlugin\">"
    "</plugin>";

  tinyxml2::XMLDocument pluginDoc;
  pluginDoc.Parse(pluginStr);
  EXPECT_TRUE(app.LoadPlugin("TestPlugin",
      pluginDoc.FirstChildElement("plugin")));

  // Create main window
  auto win = app.findChild<MainWindow *>();
  ASSERT_NE(nullptr, win);

  // Check plugin count
  EXPECT_EQ(1, win->findChildren<Plugin *>().size());

  // Get pointers
  auto plugin = win->findChildren<Plugin *>()[0];
  ASSERT_NE(nullptr, plugin->PluginItem());
  ASSERT_NE(nullptr, plugin->CardItem());
  ASSERT_NE(nullptr, plugin->Context());
}
