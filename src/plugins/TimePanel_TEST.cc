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

#include "ignition/gui/Iface.hh"
#include "ignition/gui/Plugin.hh"
#include "ignition/gui/MainWindow.hh"

using namespace ignition;
using namespace gui;

/////////////////////////////////////////////////
TEST(TimePanelTest, Load)
{
  EXPECT_TRUE(initApp());

  EXPECT_TRUE(loadPlugin("TimePanel"));

  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(TimePanelTest, DefaultConfig)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // Load plugin
  EXPECT_TRUE(loadPlugin("TimePanel"));

  // Create main window
  EXPECT_TRUE(createMainWindow());
  auto win = mainWindow();
  EXPECT_TRUE(win != nullptr);

  // Get plugin
  auto plugins = win->findChildren<Plugin *>();
  EXPECT_EQ(plugins.size(), 1);
  auto plugin = plugins[0];
  EXPECT_EQ(plugin->Title(), "Time panel");

  // Empty
  auto children = plugin->findChildren<QWidget *>();
  EXPECT_EQ(children.size(), 0);

  // Cleanup
  plugins.clear();
  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(TimePanelTest, WorldControl)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // Load plugin
  const char *pluginStr =
    "<plugin filename=\"TimePanel\">"
      "<title>World Control!</title>"
      "<world_control>"
        "<play_pause>true</play_pause>"
        "<service>/world_control_test</service>"
      "</world_control>"
    "</plugin>";

  tinyxml2::XMLDocument pluginDoc;
  pluginDoc.Parse(pluginStr);
  EXPECT_TRUE(ignition::gui::loadPlugin("TimePanel",
      pluginDoc.FirstChildElement("plugin")));

  // Create main window
  EXPECT_TRUE(createMainWindow());
  auto win = mainWindow();
  EXPECT_TRUE(win != nullptr);

  // Show, but don't exec, so we don't block
  win->show();

  // Get plugin
  auto plugins = win->findChildren<Plugin *>();
  EXPECT_EQ(plugins.size(), 1);
  auto plugin = plugins[0];
  EXPECT_EQ(plugin->Title(), "World Control!");

  // Buttons
  auto playButton = plugin->findChild<QPushButton *>("playButton");
  EXPECT_TRUE(playButton != nullptr);
  EXPECT_FALSE(playButton->isVisible());
  auto pauseButton = plugin->findChild<QPushButton *>("pauseButton");
  EXPECT_TRUE(pauseButton != nullptr);
  EXPECT_TRUE(pauseButton->isVisible());
  auto stepButton = plugin->findChild<QPushButton *>("stepButton");
  EXPECT_TRUE(stepButton != nullptr);
  EXPECT_TRUE(stepButton->isVisible());
  EXPECT_FALSE(stepButton->isEnabled());

  // World control service
  bool playCalled = false;
  bool pauseCalled = false;
  bool multiStepCalled = false;
  std::function<void(const msgs::WorldControl &, msgs::Boolean &, bool &)> cb =
      [&](const msgs::WorldControl &_req, msgs::Boolean &, bool &_result)
  {
    pauseCalled = _req.has_pause() && _req.pause();
    playCalled = _req.has_pause() && !_req.pause();
    multiStepCalled = _req.has_multi_step();
    _result = true;
  };
  transport::Node node;
  node.Advertise("/world_control_test", cb);

  // Pause
  pauseButton->click();
  EXPECT_TRUE(pauseCalled);
  EXPECT_TRUE(playButton->isVisible());
  EXPECT_FALSE(pauseButton->isVisible());
  EXPECT_TRUE(stepButton->isEnabled());

  // Step
  stepButton->click();
  EXPECT_TRUE(multiStepCalled);
  EXPECT_TRUE(playButton->isVisible());
  EXPECT_FALSE(pauseButton->isVisible());
  EXPECT_TRUE(stepButton->isEnabled());

  // Play
  playButton->click();
  EXPECT_TRUE(playCalled);
  EXPECT_FALSE(playButton->isVisible());
  EXPECT_TRUE(pauseButton->isVisible());
  EXPECT_FALSE(stepButton->isEnabled());

  // Cleanup
  plugins.clear();
  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(TimePanelTest, IncorrectWorldStats)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // Load plugin
  const char *pluginStr =
    "<plugin filename=\"TimePanel\">"
      "<world_stats>"
        "<sim_time>true</sim_time>"
        "<real_time>true</real_time>"
        "<topic>incorrect   topic  with spaces</topic>"
      "</world_stats>"
      "<world_control>"
        "<play_pause>true</play_pause>"
        "<start_paused>true</start_paused>"
        "<service>/world_control_test</service>"
      "</world_control>"
    "</plugin>";

  tinyxml2::XMLDocument pluginDoc;
  pluginDoc.Parse(pluginStr);
  EXPECT_TRUE(ignition::gui::loadPlugin("TimePanel",
      pluginDoc.FirstChildElement("plugin")));

  // Create main window
  EXPECT_TRUE(createMainWindow());
  auto win = mainWindow();
  EXPECT_TRUE(win != nullptr);

  // Show, but don't exec, so we don't block
  win->show();

  // Get plugin
  auto plugins = win->findChildren<Plugin *>();
  EXPECT_EQ(plugins.size(), 1);
  auto plugin = plugins[0];

  // Labels
  auto simTime = plugin->findChild<QLabel *>("simTimeLabel");
  EXPECT_TRUE(simTime == nullptr);
  auto realTime = plugin->findChild<QLabel *>("realTimeLabel");
  EXPECT_TRUE(realTime == nullptr);

  // Cleanup
  plugins.clear();
  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(TimePanelTest, WorldStats)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // Load plugin
  const char *pluginStr =
    "<plugin filename=\"TimePanel\">"
      "<world_stats>"
        "<sim_time>true</sim_time>"
        "<real_time>true</real_time>"
        "<topic>/world_stats_test</topic>"
      "</world_stats>"
      "<world_control>"
        "<play_pause>true</play_pause>"
        "<start_paused>true</start_paused>"
        "<service>/world_control_test</service>"
      "</world_control>"
    "</plugin>";

  tinyxml2::XMLDocument pluginDoc;
  pluginDoc.Parse(pluginStr);
  EXPECT_TRUE(ignition::gui::loadPlugin("TimePanel",
      pluginDoc.FirstChildElement("plugin")));

  // Create main window
  EXPECT_TRUE(createMainWindow());
  auto win = mainWindow();
  EXPECT_TRUE(win != nullptr);

  // Show, but don't exec, so we don't block
  win->show();

  // Get plugin
  auto plugins = win->findChildren<Plugin *>();
  EXPECT_EQ(plugins.size(), 1);
  auto plugin = plugins[0];

  // Labels
  auto simTime = plugin->findChild<QLabel *>("simTimeLabel");
  EXPECT_TRUE(simTime != nullptr);
  EXPECT_EQ(simTime->text(), "N/A");
  auto realTime = plugin->findChild<QLabel *>("realTimeLabel");
  EXPECT_TRUE(realTime != nullptr);
  EXPECT_EQ(realTime->text(), "N/A");

  // Buttons
  auto playButton = plugin->findChild<QPushButton *>("playButton");
  EXPECT_TRUE(playButton != nullptr);
  EXPECT_TRUE(playButton->isVisible());
  auto pauseButton = plugin->findChild<QPushButton *>("pauseButton");
  EXPECT_TRUE(pauseButton != nullptr);
  EXPECT_FALSE(pauseButton->isVisible());
  auto stepButton = plugin->findChild<QPushButton *>("stepButton");
  EXPECT_TRUE(stepButton != nullptr);
  EXPECT_TRUE(stepButton->isVisible());
  EXPECT_TRUE(stepButton->isEnabled());

  // Publish stats
  transport::Node node;
  auto pub = node.Advertise<msgs::WorldStatistics>("/world_stats_test");

  {
    msgs::WorldStatistics msg;
    auto simTimeMsg = msg.mutable_sim_time();
    simTimeMsg->set_sec(3600);
    simTimeMsg->set_nsec(123456789);
    pub.Publish(msg);
  }

  EXPECT_EQ(simTime->text().toStdString(), "00 01:00:00.123");
  EXPECT_EQ(realTime->text().toStdString(), "N/A");
  EXPECT_TRUE(playButton->isVisible());
  EXPECT_FALSE(pauseButton->isVisible());
  EXPECT_TRUE(stepButton->isVisible());
  EXPECT_TRUE(stepButton->isEnabled());

  {
    msgs::WorldStatistics msg;
    auto realTimeMsg = msg.mutable_real_time();
    realTimeMsg->set_sec(86400);
    realTimeMsg->set_nsec(1000000);
    pub.Publish(msg);
  }

  EXPECT_EQ(simTime->text().toStdString(), "00 01:00:00.123");
  EXPECT_EQ(realTime->text().toStdString(), "01 00:00:00.001");
  EXPECT_TRUE(playButton->isVisible());
  EXPECT_FALSE(pauseButton->isVisible());
  EXPECT_TRUE(stepButton->isVisible());
  EXPECT_TRUE(stepButton->isEnabled());

  {
    msgs::WorldStatistics msg;
    msg.set_paused(true);
    pub.Publish(msg);
  }

  EXPECT_EQ(simTime->text().toStdString(), "00 01:00:00.123");
  EXPECT_EQ(realTime->text().toStdString(), "01 00:00:00.001");
  EXPECT_TRUE(playButton->isVisible());
  EXPECT_FALSE(pauseButton->isVisible());
  EXPECT_TRUE(stepButton->isVisible());
  EXPECT_TRUE(stepButton->isEnabled());

  {
    msgs::WorldStatistics msg;
    msg.set_paused(false);
    pub.Publish(msg);
  }

  EXPECT_EQ(simTime->text().toStdString(), "00 01:00:00.123");
  EXPECT_EQ(realTime->text().toStdString(), "01 00:00:00.001");
  EXPECT_FALSE(playButton->isVisible());
  EXPECT_TRUE(pauseButton->isVisible());
  EXPECT_TRUE(stepButton->isVisible());
  EXPECT_FALSE(stepButton->isEnabled());

  // Cleanup
  plugins.clear();
  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(TimePanelTest, ControlWithoutService)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // Load plugin
  const char* pluginStr =
    "<plugin filename=\"TimePanel\">"
      "<world_control>"
      "</world_control>"
    "</plugin>";

  tinyxml2::XMLDocument pluginDoc;
  pluginDoc.Parse(pluginStr);
  EXPECT_TRUE(ignition::gui::loadPlugin("TimePanel",
      pluginDoc.FirstChildElement("plugin")));

  // Create main window
  EXPECT_TRUE(createMainWindow());
  auto win = mainWindow();
  EXPECT_TRUE(win != nullptr);

  // Get plugin
  auto plugins = win->findChildren<Plugin *>();
  EXPECT_EQ(plugins.size(), 1);
  auto plugin = plugins[0];

  // Empty
  auto children = plugin->findChildren<QWidget *>();
  EXPECT_EQ(children.size(), 0);

  // Cleanup
  plugins.clear();
  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(TimePanelTest, StatsWithoutTopic)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // Load plugin
  const char *pluginStr =
    "<plugin filename=\"TimePanel\">"
      "<world_stats>"
      "</world_stats>"
    "</plugin>";

  tinyxml2::XMLDocument pluginDoc;
  pluginDoc.Parse(pluginStr);
  EXPECT_TRUE(ignition::gui::loadPlugin("TimePanel",
      pluginDoc.FirstChildElement("plugin")));

  // Create main window
  EXPECT_TRUE(createMainWindow());
  auto win = mainWindow();
  EXPECT_TRUE(win != nullptr);

  // Get plugin
  auto plugins = win->findChildren<Plugin *>();
  EXPECT_EQ(plugins.size(), 1);
  auto plugin = plugins[0];

  // Empty
  auto children = plugin->findChildren<QWidget *>();
  EXPECT_EQ(children.size(), 0);

  // Cleanup
  plugins.clear();
  EXPECT_TRUE(stop());
}
