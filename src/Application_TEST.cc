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

#include <stdlib.h>
#include <gtest/gtest.h>
#include <ignition/common/Console.hh>

#include "test_config.h"  // NOLINT(build/include)
#include "ignition/gui/Application.hh"
#include "ignition/gui/Dialog.hh"
#include "ignition/gui/MainWindow.hh"

int gg_argc = 1;
char **gg_argv = new char *[gg_argc];

using namespace ignition;
using namespace gui;

//////////////////////////////////////////////////
TEST(ApplicationTest, Constructor)
{
  ignition::common::Console::SetVerbosity(4);

  // No Qt app
  EXPECT_EQ(nullptr, qGuiApp);
  EXPECT_EQ(nullptr, App());

  // One app construct - destruct
  {
    Application app(gg_argc, gg_argv);

    EXPECT_NE(nullptr, qGuiApp);
    ASSERT_NE(nullptr, App());
    EXPECT_NE(nullptr, App()->Engine());

    // No crash if argc and argv were correctly set
    QCoreApplication::arguments();
  }

  // No Qt app
  EXPECT_EQ(nullptr, qGuiApp);
  EXPECT_EQ(nullptr, App());
}

//////////////////////////////////////////////////
TEST(ApplicationTest, LoadPlugin)
{
  // No Qt app
  EXPECT_EQ(nullptr, qGuiApp);

  // Official plugin
  {
    Application app(gg_argc, gg_argv);

    EXPECT_TRUE(app.LoadPlugin("Publisher"));
  }

  // Inexistent plugin
  {
    Application app(gg_argc, gg_argv);

    EXPECT_FALSE(app.LoadPlugin("_doesnt_exist"));
  }

  // Plugin path added programmatically
  {
    Application app(gg_argc, gg_argv);
    app.AddPluginPath(std::string(PROJECT_BINARY_PATH) + "/lib");

    EXPECT_TRUE(app.LoadPlugin("TestPlugin"));
  }

  // Plugin path added by env var
  {
    setenv("TEST_ENV_VAR",
        (std::string(PROJECT_BINARY_PATH) + "/lib").c_str(), 1);

    Application app(gg_argc, gg_argv);
    app.SetPluginPathEnv("TEST_ENV_VAR");

    EXPECT_TRUE(app.LoadPlugin("TestPlugin"));
  }

  // Plugin which doesn't inherit from ignition::gui::Plugin
  {
    Application app(gg_argc, gg_argv);
    app.AddPluginPath(std::string(PROJECT_BINARY_PATH) + "/lib");

    EXPECT_FALSE(app.LoadPlugin("TestBadInheritancePlugin"));
  }

  // Plugin which is not registered
  {
    Application app(gg_argc, gg_argv);
    app.AddPluginPath(std::string(PROJECT_BINARY_PATH) + "/lib");

    EXPECT_FALSE(app.LoadPlugin("TestNotRegisteredPlugin"));
  }
}

//////////////////////////////////////////////////
TEST(ApplicationTest, LoadConfig)
{
  EXPECT_EQ(nullptr, qGuiApp);

  // Empty string
  {
    Application app(gg_argc, gg_argv);

    EXPECT_FALSE(app.LoadConfig(""));
  }

  // Test config file
  {
    Application app(gg_argc, gg_argv);

    // Add test plugin to path (referenced in config)
    auto testBuildPath = std::string(PROJECT_BINARY_PATH) + "/lib/";
    app.AddPluginPath(testBuildPath);

    // Load test config file
    auto testSourcePath = std::string(PROJECT_SOURCE_PATH) + "/test/";
    EXPECT_TRUE(app.LoadConfig(testSourcePath + "config/test.config"));
  }
}

//////////////////////////////////////////////////
TEST(ApplicationTest, LoadDefaultConfig)
{
  EXPECT_EQ(nullptr, qGuiApp);

  // Test config file
  {
    Application app(gg_argc, gg_argv);

    // Add test plugin to path (referenced in config)
    auto testBuildPath = ignition::common::joinPaths(
      std::string(PROJECT_BINARY_PATH), "lib");
    app.AddPluginPath(testBuildPath);

    // Set default config file
    auto configPath = ignition::common::joinPaths(
      std::string(PROJECT_SOURCE_PATH), "test", "config", "test.config");
    app.SetDefaultConfigPath(configPath);

    EXPECT_TRUE(app.LoadDefaultConfig());
    EXPECT_EQ(app.DefaultConfigPath(), configPath);
  }
}

//////////////////////////////////////////////////
TEST(ApplicationTest, MainWindowNoPlugins)
{
  EXPECT_EQ(nullptr, qGuiApp);

  // Try to run before creating
  {
    Application app(gg_argc, gg_argv);

    EXPECT_FALSE(app.RunMainWindow());
  }

  // Steps in order
  {
    Application app(gg_argc, gg_argv);

    // Create main window
    EXPECT_TRUE(app.CreateMainWindow());

    auto win = app.Window();
    ASSERT_NE(nullptr, win);

    auto quickWin = app.Window()->QuickWindow();
    ASSERT_NE(nullptr, quickWin);

    // Close window after some time
    QTimer::singleShot(300, quickWin, SLOT(close()));

    // Show window
    EXPECT_TRUE(app.RunMainWindow());
  }
}

//////////////////////////////////////////////////
TEST(ApplicationTest, Dialog)
{
  EXPECT_EQ(nullptr, qGuiApp);

  // Init app first
  {
    Application app(gg_argc, gg_argv);

    // Add test plugin to path
    auto testBuildPath = std::string(PROJECT_BINARY_PATH) + "/lib/";
    app.AddPluginPath(testBuildPath);

    // Load test plugin
    EXPECT_TRUE(app.LoadPlugin("TestPlugin"));

    // Close dialog after some time
    auto closed = false;
    QTimer::singleShot(300, [&] {
      auto ds = app.Dialogs();
      EXPECT_EQ(ds.size(), 1u);

      // Close
      ds[0]->QuickWindow()->close();
      closed = true;
    });

    // Run dialog
//    EXPECT_TRUE(app.RunDialogs());

    // Make sure timer was triggered
//    EXPECT_TRUE(closed);
  }

  // Multiple dialogs
  {
    Application app(gg_argc, gg_argv);

    // Add test plugin to path
    auto testBuildPath = std::string(PROJECT_BINARY_PATH) + "/lib/";
    app.AddPluginPath(testBuildPath);

    // Load 2 test plugins
    EXPECT_TRUE(app.LoadPlugin("TestPlugin"));
    EXPECT_TRUE(app.LoadPlugin("TestPlugin"));

    // Close dialogs after some time
//    QTimer::singleShot(300, [&] {
//      auto ds = dialogs();
//      EXPECT_EQ(ds.size(), 2u);
//
//      ds[0]->QuickWindow()->close();
//      ds[1]->QuickWindow()->close();
//    });

    // Run dialog
//    EXPECT_TRUE(app.RunDialogs());

  // Make sure timer was triggered
//  EXPECT_TRUE(closed);
  }
}

//////////////////////////////////////////////////
TEST(ApplicationTest, RunEmptyWindow)
{
  EXPECT_EQ(nullptr, qGuiApp);

  Application app(gg_argc, gg_argv);

  // Close window after 1 s
  bool closed = false;
  QTimer::singleShot(300, [&] {

    auto win = app.Window();
    ASSERT_NE(nullptr, win);

    auto quickWin = win->QuickWindow();
    ASSERT_NE(nullptr, quickWin);

    quickWin->close();
    closed = true;
  });

  // Run empty window
  EXPECT_TRUE(app.RunEmptyWindow());

  // Make sure timer was triggered
  EXPECT_TRUE(closed);
}

//////////////////////////////////////////////////
TEST(ApplicationTest, RunStandalone)
{
  EXPECT_EQ(nullptr, qGuiApp);

  // Empty string
  {
    Application app(gg_argc, gg_argv);

    EXPECT_FALSE(app.RunStandalone(""));
  }

  // Bad file
  {
    Application app(gg_argc, gg_argv);

    EXPECT_FALSE(app.RunStandalone("badfile"));
  }

  // Good file
  {
    Application app(gg_argc, gg_argv);

    // Add test plugin to path
    auto testBuildPath = std::string(PROJECT_BINARY_PATH) + "/lib/";
    app.AddPluginPath(testBuildPath);

    // Close dialog after 1 s
    bool closed = false;
    QTimer::singleShot(300, [&] {
      auto ds = app.Dialogs();
      EXPECT_EQ(ds.size(), 1u);

      // Close
      ds[0]->QuickWindow()->close();
      closed = true;
    });

    // Run test plugin
    EXPECT_TRUE(app.RunStandalone("TestPlugin"));

    // Make sure timer was triggered
    EXPECT_TRUE(closed);
  }
}

//////////////////////////////////////////////////
TEST(ApplicationTest, runConfig)
{
  EXPECT_EQ(nullptr, qGuiApp);

  // Empty string
  {
    Application app(gg_argc, gg_argv);

    EXPECT_FALSE(app.RunConfig(""));
  }

  // Bad file
  {
    Application app(gg_argc, gg_argv);

    EXPECT_FALSE(app.RunConfig("badfile"));
  }

  // Good file
  {
    Application app(gg_argc, gg_argv);

    // Add test plugin to path
    auto testBuildPath = std::string(PROJECT_BINARY_PATH) + "/lib/";
    app.AddPluginPath(testBuildPath);

    // Close window after 1 s
    bool closed = false;
    QTimer::singleShot(300, [&] {
      auto win = app.Window();
      EXPECT_TRUE(win != nullptr);
      win->QuickWindow()->close();
      closed = true;
    });

    // Run test config file
    auto testSourcePath = std::string(PROJECT_SOURCE_PATH) + "/test/";
    EXPECT_TRUE(app.RunConfig(testSourcePath + "config/test.config"));

    // Make sure timer was triggered
    EXPECT_TRUE(closed);
  }
}

/////////////////////////////////////////////////
TEST(ApplicationTest, messageHandler)
{
  EXPECT_EQ(nullptr, qGuiApp);

  Application app(gg_argc, gg_argv);

  // \todo Verify output, see ignition::commmon::Console_TEST for example
  qDebug("This came from qDebug");
  qInfo("This came from qInfo");
  qWarning("This came from qWarning");
  qCritical("This came from qCritical");
}

