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

int g_argc = 1;
char **g_argv = new char *[g_argc];

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
    Application app(g_argc, g_argv);

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
  ignition::common::Console::SetVerbosity(4);

  // No Qt app
  EXPECT_EQ(nullptr, qGuiApp);

  // Official plugin
  {
    Application app(g_argc, g_argv);

    EXPECT_TRUE(app.LoadPlugin("Publisher"));
  }

  // Inexistent plugin
  {
    Application app(g_argc, g_argv);

    EXPECT_FALSE(app.LoadPlugin("_doesnt_exist"));
  }

  // Plugin path added programmatically
  {
    Application app(g_argc, g_argv);
    app.AddPluginPath(std::string(PROJECT_BINARY_PATH) + "/lib");

    EXPECT_TRUE(app.LoadPlugin("TestPlugin"));
  }

  // Plugin path added by env var
  {
    setenv("TEST_ENV_VAR",
        (std::string(PROJECT_BINARY_PATH) + "/lib").c_str(), 1);

    Application app(g_argc, g_argv);
    app.SetPluginPathEnv("TEST_ENV_VAR");

    EXPECT_TRUE(app.LoadPlugin("TestPlugin"));
  }

  // Plugin which doesn't inherit from ignition::gui::Plugin
  {
    Application app(g_argc, g_argv);
    app.AddPluginPath(std::string(PROJECT_BINARY_PATH) + "/lib");

    EXPECT_FALSE(app.LoadPlugin("TestBadInheritancePlugin"));
  }

  // Plugin which is not registered
  {
    Application app(g_argc, g_argv);
    app.AddPluginPath(std::string(PROJECT_BINARY_PATH) + "/lib");

    EXPECT_FALSE(app.LoadPlugin("TestNotRegisteredPlugin"));
  }
}

//////////////////////////////////////////////////
TEST(ApplicationTest, LoadConfig)
{
  ignition::common::Console::SetVerbosity(4);

  EXPECT_EQ(nullptr, qGuiApp);

  // Empty string
  {
    Application app(g_argc, g_argv);

    EXPECT_FALSE(app.LoadConfig(""));
  }

  // Test config file
  {
    Application app(g_argc, g_argv);

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
  ignition::common::Console::SetVerbosity(4);

  EXPECT_EQ(nullptr, qGuiApp);

  // Test config file
  {
    Application app(g_argc, g_argv);

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
  ignition::common::Console::SetVerbosity(4);

  EXPECT_EQ(nullptr, qGuiApp);

  // Steps in order
  {
    Application app(g_argc, g_argv);

    // Create main window
    EXPECT_TRUE(app.InitializeMainWindow());

    auto wins = app.allWindows();
    ASSERT_EQ(wins.size(), 1);

    // Close window after some time
    QTimer::singleShot(300, wins[0], SLOT(close()));

    // Show window
    app.exec();
  }
}

//////////////////////////////////////////////////
TEST(ApplicationTest, Dialog)
{
  ignition::common::Console::SetVerbosity(4);

  EXPECT_EQ(nullptr, qGuiApp);

  // Init app first
  {
    Application app(g_argc, g_argv);

    // Add test plugin to path
    auto testBuildPath = std::string(PROJECT_BINARY_PATH) + "/lib/";
    app.AddPluginPath(testBuildPath);

    // Load test plugin
    EXPECT_TRUE(app.LoadPlugin("TestPlugin"));

    // Close dialog after some time
    auto closed = false;
    QTimer::singleShot(300, [&] {
      auto ds = app.allWindows();

      // The main dialog and the hidden undocked dialog from Card.qml
      EXPECT_EQ(ds.size(), 2);

      EXPECT_TRUE(qobject_cast<QQuickWindow *>(ds[0]));
      EXPECT_TRUE(qobject_cast<QQuickWindow *>(ds[1]));

      // Close
      ds[0]->close();
      closed = true;
    });

    // Run dialog
    EXPECT_TRUE(app.RunDialogs());

    // Make sure timer was triggered
    EXPECT_TRUE(closed);
  }

  // Multiple dialogs
  {
    Application app(g_argc, g_argv);

    // Add test plugin to path
    auto testBuildPath = std::string(PROJECT_BINARY_PATH) + "/lib/";
    app.AddPluginPath(testBuildPath);

    // Load 2 test plugins
    EXPECT_TRUE(app.LoadPlugin("TestPlugin"));
    EXPECT_TRUE(app.LoadPlugin("TestPlugin"));

    // Close dialogs after some time
    auto closed = false;
    QTimer::singleShot(300, [&] {
      auto ds = app.allWindows();
      EXPECT_EQ(ds.size(), 4);

      for (auto dialog : ds)
        dialog->close();
      closed = true;
    });

    // Run dialog
    EXPECT_TRUE(app.RunDialogs());

    // Make sure timer was triggered
    EXPECT_TRUE(closed);
  }
}

//////////////////////////////////////////////////
TEST(ApplicationTest, RunEmptyWindow)
{
  ignition::common::Console::SetVerbosity(4);

  EXPECT_EQ(nullptr, qGuiApp);

  Application app(g_argc, g_argv);

  // Close window after 1 s
  bool closed = false;
  QTimer::singleShot(300, [&] {

    auto wins = app.allWindows();
    ASSERT_EQ(wins.size(), 1);

    wins[0]->close();
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
  ignition::common::Console::SetVerbosity(4);

  EXPECT_EQ(nullptr, qGuiApp);

  // Empty string
  {
    Application app(g_argc, g_argv);

    EXPECT_FALSE(app.RunStandalone(""));
  }

  // Bad file
  {
    Application app(g_argc, g_argv);

    EXPECT_FALSE(app.RunStandalone("badfile"));
  }

  // Good file
  {
    Application app(g_argc, g_argv);

    // Add test plugin to path
    auto testBuildPath = std::string(PROJECT_BINARY_PATH) + "/lib/";
    app.AddPluginPath(testBuildPath);

    // Close dialog after 1 s
    bool closed = false;
    QTimer::singleShot(300, [&] {
      auto ds = app.allWindows();

      // The main dialog and the hidden undocked dialog from Card.qml
      EXPECT_EQ(ds.size(), 2);

      EXPECT_TRUE(qobject_cast<QQuickWindow *>(ds[0]));
      EXPECT_TRUE(qobject_cast<QQuickWindow *>(ds[1]));

      // Close
      ds[0]->close();
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
  ignition::common::Console::SetVerbosity(4);

  EXPECT_EQ(nullptr, qGuiApp);

  // Empty string
  {
    Application app(g_argc, g_argv);

    EXPECT_FALSE(app.RunConfig(""));
  }

  // Bad file
  {
    Application app(g_argc, g_argv);

    EXPECT_FALSE(app.RunConfig("badfile"));
  }

  // Good file
  {
    Application app(g_argc, g_argv);

    // Add test plugin to path
    auto testBuildPath = std::string(PROJECT_BINARY_PATH) + "/lib/";
    app.AddPluginPath(testBuildPath);

    // Close window after 1 s
    bool closed = false;
    QTimer::singleShot(300, [&]
    {
      auto wins = app.allWindows();
      ASSERT_EQ(wins.size(), 2);

      for (auto win : wins)
        win->close();

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
  ignition::common::Console::SetVerbosity(4);

  EXPECT_EQ(nullptr, qGuiApp);

  Application app(g_argc, g_argv);

  // \todo Verify output, see ignition::commmon::Console_TEST for example
  qDebug("This came from qDebug");
  qInfo("This came from qInfo");
  qWarning("This came from qWarning");
  qCritical("This came from qCritical");
}

