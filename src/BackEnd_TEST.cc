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
#include "ignition/gui/BackEnd.hh"
#include "ignition/gui/Dialog.hh"
#include "ignition/gui/MainWindow.hh"

using namespace ignition;
using namespace gui;

//////////////////////////////////////////////////
TEST(BackEndTest, ConstructorNoSingleton)
{
  // No Qt app
  EXPECT_EQ(nullptr, qGuiApp);
  EXPECT_EQ(nullptr, App());

  // One backend construct - destruct
  {
    BackEnd backEnd;
    backEnd.SetVerbosity(4);

    EXPECT_NE(nullptr, qGuiApp);
    ASSERT_NE(nullptr, App());
    EXPECT_NE(nullptr, App()->Engine());

    // No crash if argc and argv were correctly set
    QCoreApplication::arguments();
  }

  // No Qt app
  EXPECT_EQ(nullptr, qGuiApp);
  EXPECT_EQ(nullptr, App());

  // Two backends - nothing crashes - 2nd one unusable
  {
    // One backend
    BackEnd backEnd1;
    backEnd1.SetVerbosity(4);

    // Check now there's an app and an engine
    auto app = App();
    EXPECT_NE(nullptr, app);

    auto engine = App()->Engine();
    ASSERT_NE(nullptr, engine);
    EXPECT_NE(nullptr, engine->rootContext());

    // Another backend
    BackEnd backEnd2;
    backEnd2.SetVerbosity(4);

    // Check the app is still the same
    EXPECT_EQ(app, App());
    EXPECT_EQ(engine, App()->Engine());
  }

  // No Qt app
  EXPECT_EQ(nullptr, qGuiApp);
  EXPECT_EQ(nullptr, App());
}

//////////////////////////////////////////////////
TEST(BackEndTest, LoadPlugin)
{
  // No Qt app
  EXPECT_EQ(nullptr, qGuiApp);

  // Official plugin
  {
    BackEnd backEnd;
    backEnd.SetVerbosity(4);

    EXPECT_TRUE(backEnd.LoadPlugin("Publisher"));
  }

  // Inexistent plugin
  {
    BackEnd backEnd;
    backEnd.SetVerbosity(4);

    EXPECT_FALSE(backEnd.LoadPlugin("_doesnt_exist"));
  }

  // Plugin path added programmatically
  {
    BackEnd backEnd;
    backEnd.SetVerbosity(4);
    backEnd.AddPluginPath(std::string(PROJECT_BINARY_PATH) + "/lib");

    EXPECT_TRUE(backEnd.LoadPlugin("TestPlugin"));
  }

  // Plugin path added by env var
  {
    setenv("TEST_ENV_VAR",
        (std::string(PROJECT_BINARY_PATH) + "/lib").c_str(), 1);

    BackEnd backEnd;
    backEnd.SetVerbosity(4);
    backEnd.SetPluginPathEnv("TEST_ENV_VAR");

    EXPECT_TRUE(backEnd.LoadPlugin("TestPlugin"));
  }

  // Plugin which doesn't inherit from ignition::gui::Plugin
  {
    BackEnd backEnd;
    backEnd.SetVerbosity(4);
    backEnd.AddPluginPath(std::string(PROJECT_BINARY_PATH) + "/lib");

    EXPECT_FALSE(backEnd.LoadPlugin("TestBadInheritancePlugin"));
  }

  // Plugin which is not registered
  {
    BackEnd backEnd;
    backEnd.SetVerbosity(4);
    backEnd.AddPluginPath(std::string(PROJECT_BINARY_PATH) + "/lib");

    EXPECT_FALSE(backEnd.LoadPlugin("TestNotRegisteredPlugin"));
  }
}

//////////////////////////////////////////////////
TEST(BackEndTest, LoadConfig)
{
  EXPECT_EQ(nullptr, qGuiApp);

  // Empty string
  {
    BackEnd backEnd;
    backEnd.SetVerbosity(4);

    EXPECT_FALSE(backEnd.LoadConfig(""));
  }

  // Test config file
  {
    BackEnd backEnd;
    backEnd.SetVerbosity(4);

    // Add test plugin to path (referenced in config)
    auto testBuildPath = std::string(PROJECT_BINARY_PATH) + "/lib/";
    backEnd.AddPluginPath(testBuildPath);

    // Load test config file
    auto testSourcePath = std::string(PROJECT_SOURCE_PATH) + "/test/";
    EXPECT_TRUE(backEnd.LoadConfig(testSourcePath + "config/test.config"));
  }
}

//////////////////////////////////////////////////
TEST(BackEndTest, LoadDefaultConfig)
{
  EXPECT_EQ(nullptr, qGuiApp);

  // Test config file
  {
    BackEnd backEnd;
    backEnd.SetVerbosity(4);

    // Add test plugin to path (referenced in config)
    auto testBuildPath = ignition::common::joinPaths(
      std::string(PROJECT_BINARY_PATH), "lib");
    backEnd.AddPluginPath(testBuildPath);

    // Set default config file
    auto configPath = ignition::common::joinPaths(
      std::string(PROJECT_SOURCE_PATH), "test", "config", "test.config");
    backEnd.SetDefaultConfigPath(configPath);

    EXPECT_TRUE(backEnd.LoadDefaultConfig());
    EXPECT_EQ(backEnd.DefaultConfigPath(), configPath);
  }
}

//////////////////////////////////////////////////
TEST(BackEndTest, MainWindowNoPlugins)
{
  EXPECT_EQ(nullptr, qGuiApp);

  // Try to run before creating
  {
    BackEnd backEnd;
    backEnd.SetVerbosity(4);

    EXPECT_FALSE(backEnd.RunMainWindow());
  }

  // Steps in order
  {
    BackEnd backEnd;
    backEnd.SetVerbosity(4);

    // Create main window
    EXPECT_TRUE(backEnd.CreateMainWindow());

    auto win = backEnd.Window();
    ASSERT_NE(nullptr, win);

    auto quickWin = backEnd.Window()->QuickWindow();
    ASSERT_NE(nullptr, quickWin);

    // Close window after some time
    QTimer::singleShot(300, quickWin, SLOT(close()));

    // Show window
    EXPECT_TRUE(backEnd.RunMainWindow());
  }
}

//////////////////////////////////////////////////
TEST(BackEndTest, Dialog)
{
  EXPECT_EQ(nullptr, qGuiApp);

  // Init app first
  {
    BackEnd backEnd;
    backEnd.SetVerbosity(4);

    // Add test plugin to path
    auto testBuildPath = std::string(PROJECT_BINARY_PATH) + "/lib/";
    backEnd.AddPluginPath(testBuildPath);

    // Load test plugin
    EXPECT_TRUE(backEnd.LoadPlugin("TestPlugin"));

    // Close dialog after some time
    auto closed = false;
    QTimer::singleShot(300, [&] {
      auto ds = backEnd.Dialogs();
      EXPECT_EQ(ds.size(), 1u);

      // Close
      ds[0]->QuickWindow()->close();
      closed = true;
    });

    // Run dialog
//    EXPECT_TRUE(backEnd.RunDialogs());

    // Make sure timer was triggered
//    EXPECT_TRUE(closed);
  }

  // Multiple dialogs
  {
    BackEnd backEnd;
    backEnd.SetVerbosity(4);

    // Add test plugin to path
    auto testBuildPath = std::string(PROJECT_BINARY_PATH) + "/lib/";
    backEnd.AddPluginPath(testBuildPath);

    // Load 2 test plugins
    EXPECT_TRUE(backEnd.LoadPlugin("TestPlugin"));
    EXPECT_TRUE(backEnd.LoadPlugin("TestPlugin"));

    // Close dialogs after some time
//    QTimer::singleShot(300, [&] {
//      auto ds = dialogs();
//      EXPECT_EQ(ds.size(), 2u);
//
//      ds[0]->QuickWindow()->close();
//      ds[1]->QuickWindow()->close();
//    });

    // Run dialog
//    EXPECT_TRUE(backEnd.RunDialogs());

  // Make sure timer was triggered
//  EXPECT_TRUE(closed);
  }
}

//////////////////////////////////////////////////
TEST(BackEndTest, RunEmptyWindow)
{
  EXPECT_EQ(nullptr, qGuiApp);

  BackEnd backEnd;
  backEnd.SetVerbosity(4);

  // Close window after 1 s
  bool closed = false;
  QTimer::singleShot(300, [&] {

    auto win = backEnd.Window();
    ASSERT_NE(nullptr, win);

    auto quickWin = win->QuickWindow();
    ASSERT_NE(nullptr, quickWin);

    quickWin->close();
    closed = true;
  });

  // Run empty window
  EXPECT_TRUE(backEnd.RunEmptyWindow());

  // Make sure timer was triggered
  EXPECT_TRUE(closed);
}

//////////////////////////////////////////////////
TEST(BackEndTest, RunStandalone)
{
  EXPECT_EQ(nullptr, qGuiApp);

  // Empty string
  {
    BackEnd backEnd;
    backEnd.SetVerbosity(4);

    EXPECT_FALSE(backEnd.RunStandalone(""));
  }

  // Bad file
  {
    BackEnd backEnd;
    backEnd.SetVerbosity(4);

    EXPECT_FALSE(backEnd.RunStandalone("badfile"));
  }

  // Good file
  {
    BackEnd backEnd;
    backEnd.SetVerbosity(4);

    // Add test plugin to path
    auto testBuildPath = std::string(PROJECT_BINARY_PATH) + "/lib/";
    backEnd.AddPluginPath(testBuildPath);

    // Close dialog after 1 s
    bool closed = false;
    QTimer::singleShot(300, [&] {
      auto ds = backEnd.Dialogs();
      EXPECT_EQ(ds.size(), 1u);

      // Close
      ds[0]->QuickWindow()->close();
      closed = true;
    });

    // Run test plugin
    EXPECT_TRUE(backEnd.RunStandalone("TestPlugin"));

    // Make sure timer was triggered
    EXPECT_TRUE(closed);
  }
}

//////////////////////////////////////////////////
TEST(BackEndTest, runConfig)
{
  EXPECT_EQ(nullptr, qGuiApp);

  // Empty string
  {
    BackEnd backEnd;
    backEnd.SetVerbosity(4);

    EXPECT_FALSE(backEnd.RunConfig(""));
  }

  // Bad file
  {
    BackEnd backEnd;
    backEnd.SetVerbosity(4);

    EXPECT_FALSE(backEnd.RunConfig("badfile"));
  }

  // Good file
  {
    BackEnd backEnd;
    backEnd.SetVerbosity(4);

    // Add test plugin to path
    auto testBuildPath = std::string(PROJECT_BINARY_PATH) + "/lib/";
    backEnd.AddPluginPath(testBuildPath);

    // Close window after 1 s
    bool closed = false;
    QTimer::singleShot(300, [&] {
      auto win = backEnd.Window();
      EXPECT_TRUE(win != nullptr);
      win->QuickWindow()->close();
      closed = true;
    });

    // Run test config file
    auto testSourcePath = std::string(PROJECT_SOURCE_PATH) + "/test/";
    EXPECT_TRUE(backEnd.RunConfig(testSourcePath + "config/test.config"));

    // Make sure timer was triggered
    EXPECT_TRUE(closed);
  }
}

/////////////////////////////////////////////////
TEST(BackEndTest, messageHandler)
{
  EXPECT_EQ(nullptr, qGuiApp);

  BackEnd backEnd;
  backEnd.SetVerbosity(4);

  // \todo Verify output, see ignition::commmon::Console_TEST for example
  qDebug("This came from qDebug");
  qInfo("This came from qInfo");
  qWarning("This came from qWarning");
  qCritical("This came from qCritical");
}

