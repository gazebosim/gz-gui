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
#include "WorldStats.hh"

int g_argc = 1;
char **g_argv = new char *[g_argc];

using namespace ignition;
using namespace gui;

/////////////////////////////////////////////////
TEST(WorldStatsTest, Load)
{
  common::Console::SetVerbosity(4);

  Application app(g_argc, g_argv);
  app.AddPluginPath(std::string(PROJECT_BINARY_PATH) + "/lib");

  EXPECT_TRUE(app.LoadPlugin("WorldStats"));

  // Get main window
  auto win = app.findChild<MainWindow *>();
  ASSERT_NE(nullptr, win);

  // Get plugin
  auto plugins = win->findChildren<Plugin *>();
  EXPECT_EQ(plugins.size(), 1);

  auto plugin = plugins[0];
  EXPECT_EQ(plugin->Title(), "World stats");

  // Cleanup
  plugins.clear();
}

/////////////////////////////////////////////////
TEST(WorldStatsTest, WorldStats)
{
  common::Console::SetVerbosity(4);

  Application app(g_argc, g_argv);
  app.AddPluginPath(std::string(PROJECT_BINARY_PATH) + "/lib");

  // Load plugin
  const char *pluginStr =
    "<plugin filename=\"WorldStats\">"
      "<ignition-gui>"
        "<title>World Stats!</title>"
      "</ignition-gui>"
      "<sim_time>true</sim_time>"
      "<real_time>true</real_time>"
      "<real_time_factor>true</real_time_factor>"
      "<topic>/world_stats_test</topic>"
    "</plugin>";

  tinyxml2::XMLDocument pluginDoc;
  pluginDoc.Parse(pluginStr);
  EXPECT_TRUE(app.LoadPlugin("WorldStats",
      pluginDoc.FirstChildElement("plugin")));

  // Get main window
  auto win = app.findChild<MainWindow *>();
  ASSERT_NE(nullptr, win);

  // Show, but don't exec, so we don't block
  win->QuickWindow()->show();

  // Get plugin
  auto plugin = win->findChild<plugins::WorldStats *>();
  ASSERT_NE(nullptr, plugin);

  EXPECT_EQ(plugin->SimTime(), "N/A");
  EXPECT_EQ(plugin->RealTime(), "N/A");
  EXPECT_EQ(plugin->RealTimeFactor(), "N/A");

  // Publish stats
  transport::Node node;
  auto pub = node.Advertise<msgs::WorldStatistics>("/world_stats_test");

  // Sim time
  {
    msgs::WorldStatistics msg;
    auto simTimeMsg = msg.mutable_sim_time();
    simTimeMsg->set_sec(3600);
    simTimeMsg->set_nsec(123456789);
    msg.set_paused(true);
    pub.Publish(msg);
  }

  // Give it time to be processed
  int sleep = 0;
  int maxSleep = 10;
  while (plugin->SimTime() == "N/A" && sleep < maxSleep)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    QCoreApplication::processEvents();
    sleep++;
  }

  EXPECT_EQ(plugin->SimTime().toStdString(), "00 01:00:00.123");
  EXPECT_EQ(plugin->RealTime().toStdString(), "N/A");
  EXPECT_EQ(plugin->RealTimeFactor().toStdString(), "0.00 %");

  // Real time
  {
    msgs::WorldStatistics msg;
    auto realTimeMsg = msg.mutable_real_time();
    realTimeMsg->set_sec(86400);
    realTimeMsg->set_nsec(1000000);
    msg.set_paused(true);
    pub.Publish(msg);
  }

  // Give it time to be processed
  sleep = 0;
  while (plugin->RealTime() == "N/A" && sleep < maxSleep)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    QCoreApplication::processEvents();
    sleep++;
  }

  EXPECT_EQ(plugin->SimTime().toStdString(), "00 01:00:00.123");
  EXPECT_EQ(plugin->RealTime().toStdString(), "01 00:00:00.001");
  EXPECT_EQ(plugin->RealTimeFactor().toStdString(), "0.00 %");

  // Real time factor
  {
    msgs::WorldStatistics msg;
    msg.set_real_time_factor(1.0);
    pub.Publish(msg);
  }

  // Give it time to be processed
  sleep = 0;
  while (plugin->RealTimeFactor() == "0.00 %" && sleep < maxSleep)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    QCoreApplication::processEvents();
    sleep++;
  }

  EXPECT_EQ(plugin->SimTime().toStdString(), "00 01:00:00.123");
  EXPECT_EQ(plugin->RealTime().toStdString(), "01 00:00:00.001");
  EXPECT_EQ(plugin->RealTimeFactor().toStdString(), "100.00 %");
}

