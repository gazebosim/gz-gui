/*
 * Copyright (C) 2018 Open Source Robotics Foundation
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
#include "ignition/gui/DisplayPlugin.hh"
#include "ignition/gui/Iface.hh"

using namespace ignition;
using namespace gui;

/////////////////////////////////////////////////
TEST(DisplayPluginTest, LoadingSavingDisplayPlugin)
{
  setenv("IGN_GUI_DISPLAY_PLUGIN_PATH",
    (std::string(PROJECT_BINARY_PATH) + "/lib").c_str(), 1);

  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // Load Scene3D before any DisplayPlugins
  EXPECT_TRUE(loadPlugin("Scene3D"));

  // Load TestDisplayPlugin with default configuration.
  auto displayPlugin = loadDisplayPlugin("TestDisplayPlugin");
  EXPECT_NE(nullptr, displayPlugin);

  // Check scene
  auto engine = rendering::engine("ogre");
  ASSERT_NE(nullptr, engine);

  auto scene = engine->SceneByName("scene");
  ASSERT_NE(nullptr, scene);

  // Visual created by the base DisplayPlugin class' constructor.
  ASSERT_EQ(1u, scene->VisualCount());

  // Geometry added to the visual in the TestDisplayPlugin's initialization.
  ASSERT_EQ(1u, scene->VisualByIndex(0)->GeometryCount());

  // Get the config of the DisplayPlugin in its default state.
  auto currentConfigStr = displayPlugin->ConfigStr();
  EXPECT_FALSE(currentConfigStr.empty());

  // Scene should be written.
  EXPECT_NE(currentConfigStr.find("<scene>scene</scene>"),
    std::string::npos) << currentConfigStr;

  // Title should be written.
  EXPECT_NE(currentConfigStr.find("<title>Test display plugin</title>"),
    std::string::npos) << currentConfigStr;

  // Should be visible by default.
  EXPECT_NE(currentConfigStr.find("<visible>true</visible>"),
    std::string::npos) << currentConfigStr;

  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(DisplayPluginTest, LoadingCustomizedDisplayPlugin)
{
  setenv("IGN_GUI_DISPLAY_PLUGIN_PATH",
    (std::string(PROJECT_BINARY_PATH) + "/lib").c_str(), 1);

  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // Load Scene3D with a custom name
  const char *pluginStr =
    "<plugin filename=\"Scene3D\">"
      "<scene>my_scene</scene>"
    "</plugin>";

  tinyxml2::XMLDocument pluginDoc;
  pluginDoc.Parse(pluginStr);
  EXPECT_TRUE(ignition::gui::loadPlugin("Scene3D",
      pluginDoc.FirstChildElement("plugin")));

  // Load TestDisplayPlugin with customized configuration.
  pluginStr =
    "<display type=\"TestDisplayPlugin\">"
      "<title>My display plugin title</title>"
      "<scene>my_scene</scene>"
      "<visible>false</visible>"
    "</display>";

  pluginDoc.Parse(pluginStr);
  auto displayPlugin = loadDisplayPlugin("TestDisplayPlugin",
      pluginDoc.FirstChildElement("display"));
  EXPECT_NE(nullptr, displayPlugin);

  // Check scene
  auto engine = rendering::engine("ogre");
  ASSERT_NE(nullptr, engine);

  auto scene = engine->SceneByName("my_scene");
  ASSERT_NE(nullptr, scene);

  // Get the config of the DisplayPlugin in its default state.
  auto currentConfigStr = displayPlugin->ConfigStr();
  EXPECT_FALSE(currentConfigStr.empty());

  // Scene should be written with the custom name.
  EXPECT_NE(currentConfigStr.find("<scene>my_scene</scene>"),
    std::string::npos) << currentConfigStr;

  // Title should be written with the custom value.
  EXPECT_NE(currentConfigStr.find("<title>My display plugin title</title>"),
    std::string::npos) << currentConfigStr;

  // Visible should be written as false.
  EXPECT_NE(currentConfigStr.find("<visible>false</visible>"),
    std::string::npos) << currentConfigStr;

  EXPECT_TRUE(stop());
}
