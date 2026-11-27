/*
 * Copyright (C) 2026 Open Source Robotics Foundation
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

#include <chrono>
#include <string>
#include <thread>

#include <gz/msgs/navsat.pb.h>

#include <gz/common/Console.hh>
#include <gz/common/Filesystem.hh>
#include <gz/transport/Node.hh>
#include <gz/utils/ExtraTestMacros.hh>

#include "gz/gui/Application.hh"
#include "gz/gui/MainWindow.hh"
#include "gz/gui/Plugin.hh"
#include "test_config.hh"  // NOLINT(build/include)

int g_argc = 1;
char* g_argv[] =
{
  reinterpret_cast<char*>(const_cast<char*>("./NavSatMap_TEST")),
};

using namespace gz;
using namespace gui;

/////////////////////////////////////////////////
/// \brief Check that the map was loaded or, when it can't be (for example,
/// without the QtLocation QML module), that the plugin says so.
void CheckMapOrLabel(QQuickItem *_pluginItem)
{
  auto mapLoader = _pluginItem->findChild<QObject *>("mapLoader");
  ASSERT_NE(mapLoader, nullptr);
  auto label = _pluginItem->findChild<QObject *>("mapNotAvailableLabel");
  ASSERT_NE(label, nullptr);

  // Values of Loader.status
  const int ready = 1;
  const int error = 3;
  const int status = mapLoader->property("status").toInt();
  gzdbg << "Map loader status: " << status << std::endl;
  EXPECT_TRUE(status == ready || status == error) << status;
  EXPECT_EQ(status == error, label->property("visible").toBool());
}

/////////////////////////////////////////////////
TEST(NavSatMapTest, GZ_UTILS_TEST_DISABLED_ON_WIN32(DefaultConfig))
{
  common::Console::SetVerbosity(4);

  Application app(g_argc, g_argv);
  app.AddPluginPath(
    common::joinPaths(std::string(PROJECT_BINARY_PATH), "lib"));

  // Load plugin
  EXPECT_TRUE(app.LoadPlugin("NavSatMap"));

  // Get main window
  auto win = app.findChild<MainWindow *>();
  ASSERT_NE(win, nullptr);

  // Get plugin
  // The NavSatMap class is not exported, so look for its base class
  auto plugins = win->findChildren<Plugin *>();
  ASSERT_EQ(plugins.size(), 1);
  auto plugin = plugins[0];
  EXPECT_EQ(plugin->Title(), "Navigation satellite map");

  // Has a topic picker
  auto topicsCombo = plugin->PluginItem()->findChild<QObject *>("topicsCombo");
  ASSERT_NE(topicsCombo, nullptr);
  auto picker =
    topicsCombo->parent();  // RowLayout that holds `visible: showPicker`
  ASSERT_NE(picker, nullptr);
  EXPECT_TRUE(picker->property("visible").toBool());

  CheckMapOrLabel(plugin->PluginItem());

  // Cleanup
  plugins.clear();
}

/////////////////////////////////////////////////
TEST(NavSatMapTest, GZ_UTILS_TEST_DISABLED_ON_WIN32(NoPickerWithTopic))
{
  common::Console::SetVerbosity(4);

  Application app(g_argc, g_argv);
  app.AddPluginPath(
    common::joinPaths(std::string(PROJECT_BINARY_PATH), "lib"));

  // Load plugin
  const char *pluginStr =
    "<plugin filename=\"NavSatMap\">"
      "<topic>/navsat_test</topic>"
      "<topic_picker>false</topic_picker>"
    "</plugin>";

  tinyxml2::XMLDocument pluginDoc;
  pluginDoc.Parse(pluginStr);
  EXPECT_TRUE(app.LoadPlugin("NavSatMap",
      pluginDoc.FirstChildElement("plugin")));

  // Get main window
  auto win = app.findChild<MainWindow *>();
  ASSERT_NE(win, nullptr);

  // Get plugin
  // The NavSatMap class is not exported, so look for its base class
  auto plugins = win->findChildren<Plugin *>();
  ASSERT_EQ(plugins.size(), 1);
  auto plugin = plugins[0];

  // The topic picker is hidden
  auto topicsCombo = plugin->PluginItem()->findChild<QObject *>("topicsCombo");
  ASSERT_NE(topicsCombo, nullptr);
  auto picker =
    topicsCombo->parent();  // RowLayout that holds `visible: showPicker`
  ASSERT_NE(picker, nullptr);
  EXPECT_FALSE(picker->property("visible").toBool());

  auto topicList = topicsCombo->property("model").toStringList();
  ASSERT_EQ(topicList.size(), 1);
  EXPECT_EQ(topicList[0], "/navsat_test");

  CheckMapOrLabel(plugin->PluginItem());

  // The position is shown, with or without the map
  transport::Node node;
  auto pub = node.Advertise<msgs::NavSat>("/navsat_test");

  msgs::NavSat msg;
  msg.set_latitude_deg(-22.9);
  msg.set_longitude_deg(-43.2);

  auto item = plugin->PluginItem();
  int sleep = 0;
  const int maxSleep = 30;
  while (item->property("latitude").toDouble() > -1.0 && sleep < maxSleep)
  {
    pub.Publish(msg);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    QCoreApplication::processEvents();
    sleep++;
  }
  EXPECT_LT(sleep, maxSleep);
  EXPECT_DOUBLE_EQ(-22.9, item->property("latitude").toDouble());
  EXPECT_DOUBLE_EQ(-43.2, item->property("longitude").toDouble());

  // Cleanup
  plugins.clear();
}
