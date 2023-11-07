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

#include <unordered_map>
#include <gtest/gtest.h>

#include <gz/common/Console.hh>
#include <gz/utils/ExtraTestMacros.hh>

#include "test_config.hh"  // NOLINT(build/include)
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
TEST(PluginTest, GZ_UTILS_TEST_ENABLED_ONLY_ON_LINUX(DeleteLater))
{
  common::Console::SetVerbosity(4);

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
      "<gz-gui>"
        "<delete_later>true</delete_later>"
      "</gz-gui>"
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
TEST(PluginTest, GZ_UTILS_TEST_ENABLED_ONLY_ON_LINUX(InvalidXmlText))
{
  common::Console::SetVerbosity(4);

  Application app(g_argc, g_argv);
  app.AddPluginPath(std::string(PROJECT_BINARY_PATH) + "/lib");

  // Load plugin config that returns null GetText
  const char *pluginStr =
    "<plugin filename=\"TestPlugin\">"
      "<gz-gui>"
        "<title><null>This results in null titleElem->GetText</null></title>"
      "</gz-gui>"
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
TEST(PluginTest, GZ_UTILS_TEST_ENABLED_ONLY_ON_LINUX(Getters))
{
  common::Console::SetVerbosity(4);

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
  auto *plugin = win->findChildren<Plugin *>().at(0);
  ASSERT_NE(nullptr, plugin->PluginItem());
  ASSERT_NE(nullptr, plugin->CardItem());
  ASSERT_NE(nullptr, plugin->Context());
}

/////////////////////////////////////////////////
TEST(PluginTest, GZ_UTILS_TEST_DISABLED_ON_WIN32(ConfigStr))
{
  common::Console::SetVerbosity(4);

  Application app(g_argc, g_argv);
  app.AddPluginPath(
      common::joinPaths(std::string(PROJECT_BINARY_PATH), "lib"));

  // Load normal plugin
  const char *pluginStr =
    "<plugin filename=\"WorldStats\" name=\"World stats\">"
    "  <gz-gui>"
    "    <title>World stats</title>"
    "    <property type=\"bool\" key=\"showTitleBar\">true</property>"
    "    <property type=\"bool\" key=\"resizable\">true</property>"
    "    <property type=\"double\" key=\"height\">200</property>"
    "    <property type=\"double\" key=\"width\">300</property>"
    "    <property type=\"double\" key=\"z\">2</property>"
    "    <property type=\"string\" key=\"state\">floating</property>"
    "    <anchors target=\"3D View\">"
    "      <line own=\"right\" target=\"right\"/>"
    "      <line own=\"bottom\" target=\"bottom\"/>"
    "    </anchors>"
    "  </gz-gui>"
    "</plugin>";

  std::unordered_map<std::string, std::string> pluginProps;
  std::unordered_map<std::string, std::string> pluginTypes;
  pluginProps["showTitleBar"] = "true";
  pluginProps["resizable"] = "true";
  pluginProps["height"] = "200";
  pluginProps["width"] = "300";
  pluginProps["z"] = "2";
  pluginProps["state"] = "floating";

  pluginTypes["showTitleBar"] = "bool";
  pluginTypes["resizable"] = "bool";
  pluginTypes["height"] = "double";
  pluginTypes["width"] = "double";
  pluginTypes["z"] = "double";
  pluginTypes["state"] = "string";

  tinyxml2::XMLDocument pluginDoc;
  pluginDoc.Parse(pluginStr);
  EXPECT_TRUE(app.LoadPlugin("WorldStats",
      pluginDoc.FirstChildElement("plugin")));

  // Create main window
  auto *win = app.findChild<MainWindow *>();
  ASSERT_NE(nullptr, win);

  // Check plugin count
  EXPECT_EQ(1, win->findChildren<Plugin *>().size());

  // Get the output for ConfigStr()
  std::string configStr;
  tinyxml2::XMLDocument configDoc;
  auto *plugin = win->findChildren<Plugin *>().at(0);
  configStr = plugin->ConfigStr();
  configDoc.Parse(configStr.c_str());

  // <plugin>
  auto *pluginElem = configDoc.FirstChildElement("plugin");
  ASSERT_NE(nullptr, pluginElem);

  // <gz-gui>
  auto *gzGuiElem = pluginElem->FirstChildElement("gz-gui");
  ASSERT_NE(nullptr, gzGuiElem);

  // Iterate properties
  for (auto *propElem = gzGuiElem->FirstChildElement("property");
      propElem != nullptr;)
  {
    // If property in map, mark it as "Verified"
    if (pluginProps.find(propElem->Attribute("key")) != pluginProps.end())
    {
      // check if the type is correct
      EXPECT_EQ(propElem->Attribute("type"),
        pluginTypes[propElem->Attribute("key")]) << propElem->Attribute("key");

      // check if the value is correct
      EXPECT_EQ(propElem->GetText(), pluginProps[propElem->Attribute("key")])
          << propElem->Attribute("key");
      pluginProps[propElem->Attribute("key")] = "Verified";
    }
    auto *nextProp = propElem->NextSiblingElement("property");
    propElem = nextProp;
  }

  // Verify all inputs properties are checked
  for (auto itr = pluginProps.begin(); itr != pluginProps.end(); itr++)
  {
    EXPECT_EQ(itr->second, "Verified") << "Did not find property: "
        <<  itr->first;
  }

}

/////////////////////////////////////////////////
TEST(PluginTest, GZ_UTILS_TEST_DISABLED_ON_WIN32(ConfigStrInputNoPlugin))
{
  common::Console::SetVerbosity(4);

  Application app(g_argc, g_argv);
  app.AddPluginPath(
      common::joinPaths(std::string(PROJECT_BINARY_PATH), "lib"));

  // Load normal plugin
  const char *pluginStr = "";

  tinyxml2::XMLDocument pluginDoc;
  pluginDoc.Parse(pluginStr);
  EXPECT_TRUE(app.LoadPlugin("WorldStats",
      pluginDoc.FirstChildElement("plugin")));

  // Create main window
  auto *win = app.findChild<MainWindow *>();
  ASSERT_NE(nullptr, win);

  // Check plugin count
  EXPECT_EQ(1, win->findChildren<Plugin *>().size());

  // Get the output for ConfigStr()
  std::string configStr;
  tinyxml2::XMLDocument configDoc;
  auto *plugin = win->findChildren<Plugin *>().at(0);
  configStr = plugin->ConfigStr();
  configDoc.Parse(configStr.c_str());

  // ConfigStr() creates a plugin with default value when input doesn't
  // contain a <plugin> tag.
  // We select a few to verify.
  std::unordered_map<std::string, std::string> pluginProps;
  std::unordered_map<std::string, std::string> pluginTypes;
  pluginProps["showTitleBar"] = "true";
  pluginProps["resizable"] = "true";
  pluginProps["cardMinimumWidth"] = "290";
  pluginProps["cardMinimumHeight"] = "110";
  pluginProps["z"] = "0";
  pluginProps["state"] = "docked";

  pluginTypes["showTitleBar"] = "bool";
  pluginTypes["resizable"] = "bool";
  pluginTypes["cardMinimumWidth"] = "int";
  pluginTypes["cardMinimumHeight"] = "int";
  pluginTypes["z"] = "double";
  pluginTypes["state"] = "string";

  // <plugin>
  auto *pluginElem = configDoc.FirstChildElement("plugin");
  ASSERT_NE(nullptr, pluginElem);

  // <gz-gui>
  auto *gzGuiElem = pluginElem->FirstChildElement("gz-gui");
  ASSERT_NE(nullptr, gzGuiElem);

  // Iterate properties
  for (auto *propElem = gzGuiElem->FirstChildElement("property");
      propElem != nullptr;)
  {
    // If property in map, mark it as "Verified"
    if (pluginProps.find(propElem->Attribute("key")) != pluginProps.end())
    {
      // check if the type is correct
      EXPECT_EQ(propElem->Attribute("type"),
        pluginTypes[propElem->Attribute("key")]) << propElem->Attribute("key");

      // check if the value is correct
      EXPECT_EQ(propElem->GetText(), pluginProps[propElem->Attribute("key")])
          << propElem->Attribute("key");
      pluginProps[propElem->Attribute("key")] = "Verified";
    }
    auto *nextProp = propElem->NextSiblingElement("property");
    propElem = nextProp;
  }

  // Verify all selected inputs properties are checked
  for (auto itr = pluginProps.begin(); itr != pluginProps.end(); itr++)
  {
    EXPECT_EQ(itr->second, "Verified") << "Did not find property: "
      << itr->first;
  }
}
