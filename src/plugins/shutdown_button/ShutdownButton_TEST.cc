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

#include <gz/common/Console.hh>
#include <gz/transport/Node.hh>
#include <gz/utilities/ExtraTestMacros.hh>

#include "test_config.h"  // NOLINT(build/include)
#include "gz/gui/Application.hh"
#include "gz/gui/Plugin.hh"
#include "gz/gui/MainWindow.hh"
#include "ShutdownButton.hh"

int g_argc = 1;
char* g_argv[] =
{
  reinterpret_cast<char*>(const_cast<char*>("./ShutdownButton_TEST")),
};

using namespace gz;
using namespace gui;

// See https://github.com/ignitionrobotics/ign-gui/issues/75
/////////////////////////////////////////////////
TEST(ShutdownButtonTest, IGN_UTILS_TEST_ENABLED_ONLY_ON_LINUX(Load))
{
  common::Console::SetVerbosity(4);

  Application app(g_argc, g_argv);
  app.AddPluginPath(std::string(PROJECT_BINARY_PATH) + "/lib");

  EXPECT_TRUE(app.LoadPlugin("ShutdownButton"));

  // Get main window
  auto win = app.findChild<MainWindow *>();
  ASSERT_NE(nullptr, win);

  // Get plugin
  auto plugins = win->findChildren<Plugin *>();
  EXPECT_EQ(plugins.size(), 1);

  auto plugin = plugins[0];
  EXPECT_EQ(plugin->Title(), "Shutdown");

  // Cleanup
  plugins.clear();
}

/////////////////////////////////////////////////
TEST(ShutdownButtonTest, IGN_UTILS_TEST_ENABLED_ONLY_ON_LINUX(ShutdownButton))
{
  common::Console::SetVerbosity(4);

  Application app(g_argc, g_argv);
  app.AddPluginPath(std::string(PROJECT_BINARY_PATH) + "/lib");
  app.LoadConfig(common::joinPaths(PROJECT_SOURCE_PATH,
    "src", "plugins", "shutdown_button", "test.config"));

  // Get main window
  auto win = app.findChild<MainWindow *>();
  ASSERT_NE(nullptr, win);

  // Show, but don't exec, so we don't block
  win->QuickWindow()->show();

  // Get plugin
  auto plugins = win->findChildren<plugins::ShutdownButton *>();
  EXPECT_EQ(plugins.size(), 1);

  auto plugin = plugins[0];
  EXPECT_EQ(plugin->Title(), "Shutdown!");

  // World control service
  bool stopCalled = false;
  std::function<bool(const msgs::ServerControl &, msgs::Boolean &)> cb =
      [&](const msgs::ServerControl &_req, msgs::Boolean &_resp)
  {
    stopCalled = _req.stop();
    _resp.set_data(true);
    return true;
  };
  transport::Node node;
  node.Advertise("/server_control_test", cb);

  EXPECT_TRUE(win->QuickWindow()->isVisible());

  plugin->OnStop();
  EXPECT_TRUE(stopCalled);

  EXPECT_FALSE(win->QuickWindow()->isVisible());

  // Cleanup
  plugins.clear();
}

/////////////////////////////////////////////////
TEST(ShutdownButtonTest, IGN_UTILS_TEST_ENABLED_ONLY_ON_LINUX(ShutdownGuiOnly))
{
  common::Console::SetVerbosity(4);

  Application app(g_argc, g_argv);
  app.AddPluginPath(std::string(PROJECT_BINARY_PATH) + "/lib");
  app.LoadConfig(common::joinPaths(PROJECT_SOURCE_PATH,
    "src", "plugins", "shutdown_button", "test.config"));

  // Get main window
  auto win = app.findChild<MainWindow *>();
  ASSERT_NE(nullptr, win);

  // override the SHUTDOWN_SERVER value from the test config
  win->SetDefaultExitAction(ExitAction::CLOSE_GUI);

  // Show, but don't exec, so we don't block
  win->QuickWindow()->show();

  // Get plugin
  auto plugins = win->findChildren<plugins::ShutdownButton *>();
  EXPECT_EQ(plugins.size(), 1);

  auto plugin = plugins[0];

  // World control service
  bool stopCalled = false;
  std::function<bool(const msgs::ServerControl &, msgs::Boolean &)> cb =
      [&](const msgs::ServerControl &_req, msgs::Boolean &_resp)
  {
    stopCalled = _req.stop();
    _resp.set_data(true);
    return true;
  };
  transport::Node node;
  node.Advertise("/server_control_test", cb);

  EXPECT_TRUE(win->QuickWindow()->isVisible());

  plugin->OnStop();
  EXPECT_FALSE(stopCalled);

  EXPECT_FALSE(win->QuickWindow()->isVisible());

  // Cleanup
  plugins.clear();
}
