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

#include "test_config.h"
#include "ignition/gui/Iface.hh"

using namespace ignition;
using namespace gui;

/////////////////////////////////////////////////
TEST(IfaceTest, initApp)
{
  setVerbosity(4);

  // Works the first time
  EXPECT_TRUE(initApp());

  // Fails if tried again
  EXPECT_FALSE(initApp());

  // Stop
  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(IfaceTest, MainWindowNoPlugins)
{
  setVerbosity(4);

  // Create app
  EXPECT_TRUE(initApp());

  // Create main window
  EXPECT_TRUE(createMainWindow());

  auto win = mainWindow();
  EXPECT_TRUE(win != nullptr);

  // Close window after 1 second
  QTimer::singleShot(1000, win, SLOT(close()));

  // Show window
  EXPECT_TRUE(runMainWindow());

  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(IfaceTest, Dialog)
{
  setVerbosity(4);

  // Add test plugin to path
  auto testBuildPath = std::string(PROJECT_BINARY_PATH) + "/test/";
  addPluginPath(testBuildPath + "plugins");

  // Create app
  EXPECT_TRUE(initApp());

  // Load test plugin
  EXPECT_TRUE(loadPlugin("libTestPlugin.so"));

  // Run dialogs
  EXPECT_TRUE(runDialogs());

  auto ds = dialogs();
  EXPECT_EQ(ds.size(), 1u);

  EXPECT_TRUE(stop());
}
/////////////////////////////////////////////////
TEST(IfaceTest, runStandalone)
{
  setVerbosity(4);

  // Bad file
  {
    EXPECT_FALSE(runStandalone("badfile"));
  }

  // Good file
  {
    // Add test plugin to path
    auto testBuildPath = std::string(PROJECT_BINARY_PATH) + "/test/";
    addPluginPath(testBuildPath + "plugins");

    // Close dialog after 1 s
    QTimer::singleShot(1000, [&] {
      stop();
    });

    // Run test plugin
    EXPECT_TRUE(runStandalone("libTestPlugin.so"));
  }
}


/////////////////////////////////////////////////
TEST(IfaceTest, runConfig)
{
  setVerbosity(4);

  // Bad file
  {
    EXPECT_FALSE(runConfig("badfile"));
  }

  // Good file
  {
    // Add test plugin to path
    auto testBuildPath = std::string(PROJECT_BINARY_PATH) + "/test/";
    addPluginPath(testBuildPath + "plugins");

    // Close window after 1 s
    QTimer::singleShot(1000, [&] {
      auto win = mainWindow();
      EXPECT_TRUE(win != nullptr);
      win->close();
    });

    // Run test config file
    auto testSourcePath = std::string(PROJECT_SOURCE_PATH) + "/test/";
    EXPECT_TRUE(runConfig(testSourcePath + "config/test.config"));
  }
}

