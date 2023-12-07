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
#include <chrono>
#include <gz/common/Console.hh>
#include <gz/utils/ExtraTestMacros.hh>

#include "test_config.hh"  // NOLINT(build/include)
#include "gz/gui/Application.hh"
#include "gz/gui/Dialog.hh"
#include "gz/gui/MainWindow.hh"
#include "gz/gui/Plugin.hh"

int g_argc = 1;
char* g_argv[] =
{
  reinterpret_cast<char*>(const_cast<char*>("./Application_TEST")),
};

using namespace gz;
using namespace gui;

class ApplicationTest: public ::testing::Test
{
  protected:
    void SetUp() override
    {
      common::Console::SetVerbosity(4);
      // No Qt app
      ASSERT_EQ(nullptr, qGuiApp);
      ASSERT_EQ(nullptr, App());
    }
};


// See https://github.com/gazebosim/gz-gui/issues/75
//////////////////////////////////////////////////
TEST_F(ApplicationTest, GZ_UTILS_TEST_ENABLED_ONLY_ON_LINUX(Constructor))
{
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
TEST_F(ApplicationTest, GZ_UTILS_TEST_ENABLED_ONLY_ON_LINUX(LoadPlugin))
{
  Application app(g_argc, g_argv);

  EXPECT_TRUE(app.LoadPlugin("Publisher"));
}
//////////////////////////////////////////////////
TEST_F(ApplicationTest,
    GZ_UTILS_TEST_ENABLED_ONLY_ON_LINUX(LoadNonexistantPlugin))
{
  Application app(g_argc, g_argv);

  EXPECT_FALSE(app.LoadPlugin("_doesnt_exist"));
  EXPECT_FALSE(app.RemovePlugin("_doesnt_exist"));
}

//////////////////////////////////////////////////
TEST_F(ApplicationTest,
    GZ_UTILS_TEST_ENABLED_ONLY_ON_LINUX(LoadProgrammaticPlugin))
{
  // No Qt app
  Application app(g_argc, g_argv);

  std::string pluginName;
  app.connect(&app, &Application::PluginAdded, [&pluginName](
      const QString &_pluginName)
  {
    pluginName = _pluginName.toStdString();
  });

  app.AddPluginPath(std::string(PROJECT_BINARY_PATH) + "/lib");

  EXPECT_TRUE(app.LoadPlugin("TestPlugin"));
  EXPECT_EQ(0u, pluginName.find("plugin"));

  auto plugin = app.PluginByName(pluginName);
  ASSERT_NE(nullptr, plugin);
  ASSERT_NE(nullptr, plugin->CardItem());

  EXPECT_EQ(pluginName, plugin->CardItem()->objectName().toStdString());

  EXPECT_TRUE(app.RemovePlugin(pluginName));
}

//////////////////////////////////////////////////
TEST_F(ApplicationTest, GZ_UTILS_TEST_ENABLED_ONLY_ON_LINUX(LoadEnvPlugin))
{
  Application app(g_argc, g_argv);

  setenv("TEST_ENV_VAR",
      (std::string(PROJECT_BINARY_PATH) + "/lib").c_str(), 1);
  app.SetPluginPathEnv("TEST_ENV_VAR");
  EXPECT_TRUE(app.LoadPlugin("TestPlugin"));
}

//////////////////////////////////////////////////
TEST_F(ApplicationTest,
    GZ_UTILS_TEST_ENABLED_ONLY_ON_LINUX(LoadBadInheritancePlugin))
{
  Application app(g_argc, g_argv);

  app.AddPluginPath(std::string(PROJECT_BINARY_PATH) + "/lib");
  EXPECT_FALSE(app.LoadPlugin("TestBadInheritancePlugin"));
}

//////////////////////////////////////////////////
TEST_F(ApplicationTest,
    GZ_UTILS_TEST_ENABLED_ONLY_ON_LINUX(LoadNotRegisteredPlugin))
{
  Application app(g_argc, g_argv);

  app.AddPluginPath(std::string(PROJECT_BINARY_PATH) + "/lib");
  EXPECT_FALSE(app.LoadPlugin("TestNotRegisteredPlugin"));
}

//////////////////////////////////////////////////
TEST_F(ApplicationTest,
    GZ_UTILS_TEST_ENABLED_ONLY_ON_LINUX(LoadInvalidQmlPlugin))
{
  Application app(g_argc, g_argv);

  app.AddPluginPath(std::string(PROJECT_BINARY_PATH) + "/lib");
  EXPECT_FALSE(app.LoadPlugin("TestInvalidQmlPlugin"));
}

//////////////////////////////////////////////////
TEST_F(ApplicationTest, GZ_UTILS_TEST_ENABLED_ONLY_ON_LINUX(LoadConfig))
{
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

  // Test environment variable and relative path
  {
    // Environment variable not set
    Application app(g_argc, g_argv);
    EXPECT_FALSE(app.LoadConfig("ignore.config"));

    // Invalid path
    setenv("GZ_GUI_RESOURCE_PATH", "invalidPath", 1);
    EXPECT_FALSE(app.LoadConfig("ignore.config"));

    // Valid path
    setenv("GZ_GUI_RESOURCE_PATH",
        (std::string(PROJECT_SOURCE_PATH) + "/test/config").c_str(), 1);
    EXPECT_TRUE(app.LoadConfig("ignore.config"));

    // Multiple paths, one valid
    setenv("GZ_GUI_RESOURCE_PATH",
        ("banana:" + std::string(PROJECT_SOURCE_PATH) + "/test/config" +
        ":orange").c_str(), 1);
    EXPECT_TRUE(app.LoadConfig("ignore.config"));
  }
}

//////////////////////////////////////////////////
TEST_F(ApplicationTest, GZ_UTILS_TEST_ENABLED_ONLY_ON_LINUX(LoadDefaultConfig))
{
  // Test config file
  {
    Application app(g_argc, g_argv);

    // Add test plugin to path (referenced in config)
    auto testBuildPath = common::joinPaths(
      std::string(PROJECT_BINARY_PATH), "lib");
    app.AddPluginPath(testBuildPath);

    // Set default config file
    auto configPath = common::joinPaths(
      std::string(PROJECT_SOURCE_PATH), "test", "config", "test.config");
    app.SetDefaultConfigPath(configPath);

    EXPECT_EQ(app.DefaultConfigPath(), configPath);
  }
}

//////////////////////////////////////////////////
TEST_F(ApplicationTest,
    GZ_UTILS_TEST_ENABLED_ONLY_ON_LINUX(InitializeMainWindowSimple))
{
  // No plugins
  Application app(g_argc, g_argv);

  auto wins = app.allWindows();
  ASSERT_EQ(wins.size(), 1);

  // Close window after some time
  QObject::connect(&app, &QGuiApplication::applicationStateChanged,
    [&wins](auto /*state*/){
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      wins[0]->close();
    });

  // Show window
  gz::gui::Application::exec();
}

//////////////////////////////////////////////////
TEST_F(ApplicationTest,
    GZ_UTILS_TEST_ENABLED_ONLY_ON_LINUX(InitializeMainWindowPlugin))
{
  Application app(g_argc, g_argv);

  EXPECT_TRUE(app.LoadPlugin("Publisher"));

  auto *win = App()->findChild<MainWindow *>();
  ASSERT_NE(nullptr, win);

  // Check plugin count
  auto plugins = win->findChildren<Plugin *>();
  EXPECT_EQ(1, plugins.count());

  QObject::connect(&app, &QGuiApplication::applicationStateChanged,
    [&win](auto /*state*/){
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      win->QuickWindow()->close();
    });

  // Show window
  gz::gui::Application::exec();
}

//////////////////////////////////////////////////
TEST_F(ApplicationTest,
    GZ_UTILS_TEST_ENABLED_ONLY_ON_LINUX(InitializeMainWindowConfig))
{
  // Test config
  auto testBuildPath = std::string(PROJECT_BINARY_PATH) + "/lib/";
  auto testSourcePath = std::string(PROJECT_SOURCE_PATH) + "/test/";

  // Load config
  Application app(g_argc, g_argv);

  // Add test plugin to path (referenced in config)
  app.AddPluginPath(testBuildPath);

  // Load test config file
  EXPECT_TRUE(app.LoadConfig(testSourcePath + "config/test.config"));

  auto *win = App()->findChild<MainWindow *>();
  ASSERT_NE(nullptr, win);

  // Check plugin count
  auto plugins = win->findChildren<Plugin *>();
  EXPECT_EQ(1, plugins.count());

  // Close window after some time
  QObject::connect(&app, &QGuiApplication::applicationStateChanged,
    [&win](auto /*state*/){
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      win->QuickWindow()->close();
    });

  // Show window
  gz::gui::Application::exec();
}

//////////////////////////////////////////////////
TEST_F(ApplicationTest, GZ_UTILS_TEST_ENABLED_ONLY_ON_LINUX(Dialog))
{
  // Single dialog
  {
    Application app(g_argc, g_argv, WindowType::kDialog);
    EXPECT_EQ(app.allWindows().size(), 0);

    // Add test plugin to path
    auto testBuildPath = std::string(PROJECT_BINARY_PATH) + "/lib/";
    app.AddPluginPath(testBuildPath);

    // Load plugin
    EXPECT_TRUE(app.LoadPlugin("TestPlugin"));

    // Close dialog after some time
    auto closed = false;

  // Close window after some time
  QObject::connect(&app, &QGuiApplication::applicationStateChanged,
   [&closed](auto /*state*/){
     std::this_thread::sleep_for(std::chrono::milliseconds(100));
     auto ds = gz::gui::Application::allWindows();

     // The main diaog - some systems return more, not sure why
     ASSERT_GE(ds.size(), 1);
     EXPECT_TRUE(qobject_cast<QQuickWindow *>(ds[0]));
     // Close
     ds[0]->close();
     closed = true;
    });

    // Show window
    gz::gui::Application::exec();

    // Make sure timer was triggered
    EXPECT_TRUE(closed);
  }

  // Multiple dialogs
  {
    Application app(g_argc, g_argv, WindowType::kDialog);
    EXPECT_EQ(app.allWindows().size(), 0);

    // Add test plugin to path
    auto testBuildPath = std::string(PROJECT_BINARY_PATH) + "/lib/";
    app.AddPluginPath(testBuildPath);

    // Load plugins
    EXPECT_TRUE(app.LoadPlugin("TestPlugin"));
    EXPECT_TRUE(app.LoadPlugin("TestPlugin"));

    // Close dialogs after some time
    auto closed = false;
    QObject::connect(&app, &QGuiApplication::applicationStateChanged,
     [&closed](auto /*state*/){
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        auto ds = gz::gui::Application::allWindows();

        // 2 dialog - some systems return more, not sure why
        ASSERT_GE(ds.size(), 2);

        for (auto *dialog : ds)
          dialog->close();
        closed = true;
      });

    // Exec dialog
    gz::gui::Application::exec();

    // Make sure timer was triggered
    EXPECT_TRUE(closed);
  }
}

/////////////////////////////////////////////////
TEST_F(ApplicationTest, GZ_UTILS_TEST_ENABLED_ONLY_ON_LINUX(messageHandler))
{
  Application app(g_argc, g_argv);

  // \todo Verify output, see commmon::Console_TEST for example
  qDebug("This came from qDebug");
  qInfo("This came from qInfo");
  qWarning("This came from qWarning");
  qCritical("This came from qCritical");
}
