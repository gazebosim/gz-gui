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
#ifdef _MSC_VER
#pragma warning(push, 0)
#endif
#include <ignition/msgs/boolean.pb.h>
#include <ignition/msgs/stringmsg.pb.h>
#include <ignition/rendering.hh>
#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include <gtest/gtest.h>
#include <string>

#include <ignition/common/Console.hh>
#include <ignition/common/Filesystem.hh>
#include <ignition/transport/Node.hh>
#include <ignition/utilities/ExtraTestMacros.hh>

#include "test_config.h"  // NOLINT(build/include)
#include "ignition/gui/Application.hh"
#include "ignition/gui/GuiEvents.hh"
#include "ignition/gui/MainWindow.hh"
#include "ignition/gui/Plugin.hh"
#include "Screenshot.hh"

int g_argc = 1;
char* g_argv[] =
{
  reinterpret_cast<char*>(const_cast<char*>("./Screenshot_TEST")),
};

using namespace ignition;
using namespace gui;

/////////////////////////////////////////////////
TEST(ScreenshotTest, IGN_UTILS_TEST_DISABLED_ON_WIN32(Screenshot))
{
  common::Console::SetVerbosity(4);

  // Load the plugin
  Application app(g_argc, g_argv);
  app.AddPluginPath(std::string(PROJECT_BINARY_PATH) + "/lib");

  EXPECT_TRUE(app.LoadPlugin("Screenshot"));

  // Get main window
  auto window = app.findChild<MainWindow *>();
  ASSERT_NE(window, nullptr);

  // Get plugin
  auto plugins = window->findChildren<Plugin *>();
  EXPECT_EQ(plugins.size(), 1);

  // TODO(anyone) Below is commented out because currently unable to load
  // Scene3D from another plugin. Once resolved this test should be implemented

  // EXPECT_TRUE(app.LoadPlugin("Scene3D"));
  // EXPECT_TRUE(app.LoadPlugin("Screenshot"));
  //
  // // Get main window
  // auto window = app.findChild<MainWindow *>();
  // ASSERT_NE(window, nullptr);
  //
  // // Get plugin
  // auto plugins = window->findChildren<Plugin *>();
  // ASSERT_EQ(plugins.size(), 2);
  //
  // --- Screenshot request ---
  // std::function<void(const ignition::msgs::Boolean &, const bool)> cb =
  //   [](const ignition::msgs::Boolean &/*_rep*/, const bool _result)
  // {
  //   if (!_result)
  //     ignerr << "Error saving screenshot" << std::endl;
  // };
  //
  // std::string screenshotService = "/gui/screenshot";
  // std::string screenshotDir =
  //     common::joinPaths(std::string(PROJECT_BINARY_PATH));
  //
  // transport::Node node;
  // msgs::StringMsg req;
  // req.set_data(screenshotDir);
  // node.Request(screenshotService, req, cb);
  //
  // window->QuickWindow()->show();
  //
  // int sleep = 0;
  // int maxSleep = 30;
  // while (sleep < maxSleep)
  // {
  //   std::this_thread::sleep_for(std::chrono::milliseconds(100));
  //   QCoreApplication::processEvents();
  //   ++sleep;
  // }

  // TODO(anyone) need to check for screenshot file then remove it

  // Cleanup
  plugins.clear();
}
