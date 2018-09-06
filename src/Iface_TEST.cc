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

#include <stdlib.h>
#include <gtest/gtest.h>
#include <ignition/common/Console.hh>

#include "test_config.h"  // NOLINT(build/include)
#include "ignition/gui/Dialog.hh"
#include "ignition/gui/MainWindow.hh"
#include "ignition/gui/Iface.hh"

using namespace ignition;
using namespace gui;

//////////////////////////////////////////////////
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

//////////////////////////////////////////////////
TEST(IfaceTest, stop)
{
  setVerbosity(4);

  // Stop before anything else
  EXPECT_TRUE(stop());
}

//////////////////////////////////////////////////
TEST(IfaceTest, loadPlugin)
{
  setVerbosity(4);

  // Before init
  {
    EXPECT_FALSE(loadPlugin("ImageDisplay"));
  }

  // Official plugin
  {
    EXPECT_TRUE(initApp());

    EXPECT_TRUE(loadPlugin("Publisher"));

    EXPECT_TRUE(stop());
  }

  // Inexistent plugin
  {
    EXPECT_TRUE(initApp());

    EXPECT_FALSE(loadPlugin("_doesnt_exist"));

    EXPECT_TRUE(stop());
  }

  // Plugin path added programmatically
  {
    addPluginPath(std::string(PROJECT_BINARY_PATH) + "/lib");

    EXPECT_TRUE(initApp());

    EXPECT_TRUE(loadPlugin("TestPlugin"));

    EXPECT_TRUE(stop());
  }

  // Plugin path added by env var
  {
    setenv("TEST_ENV_VAR",
        (std::string(PROJECT_BINARY_PATH) + "/lib").c_str(), 1);

    setPluginPathEnv("TEST_ENV_VAR");

    EXPECT_TRUE(initApp());

    EXPECT_TRUE(loadPlugin("TestPlugin"));

    EXPECT_TRUE(stop());
  }

  // Plugin which doesn't inherit from ignition::gui::Plugin
  {
    addPluginPath(std::string(PROJECT_BINARY_PATH) + "/lib");

    EXPECT_TRUE(initApp());

    EXPECT_FALSE(loadPlugin("TestBadInheritancePlugin"));

    EXPECT_TRUE(stop());
  }

  // Display plugin
  {
    addPluginPath(std::string(PROJECT_BINARY_PATH) + "/lib");

    EXPECT_TRUE(initApp());

    EXPECT_FALSE(loadPlugin("TestDisplayPlugin"));

    EXPECT_TRUE(stop());
  }

  // Plugin which is not registered
  {
    addPluginPath(std::string(PROJECT_BINARY_PATH) + "/lib");

    EXPECT_TRUE(initApp());

    EXPECT_FALSE(loadPlugin("TestNotRegisteredPlugin"));

    EXPECT_TRUE(stop());
  }
}

//////////////////////////////////////////////////
TEST(IfaceTest, loadDisplayPlugin)
{
  setVerbosity(4);

  // Before init
  {
    EXPECT_EQ(nullptr, loadDisplayPlugin("GridDisplay"));
  }

  // Official plugin
  {
    EXPECT_TRUE(initApp());

    EXPECT_NE(nullptr, loadDisplayPlugin("GridDisplay"));

    EXPECT_TRUE(stop());
  }

  // Inexistent plugin
  {
    EXPECT_TRUE(initApp());

    EXPECT_EQ(nullptr, loadDisplayPlugin("_doesnt_exist"));

    EXPECT_TRUE(stop());
  }

  // Display plugin path located using the env var
  {
    setenv("IGN_GUI_DISPLAY_PLUGIN_PATH",
        (std::string(PROJECT_BINARY_PATH) + "/lib").c_str(), 1);

    EXPECT_TRUE(initApp());

    EXPECT_NE(nullptr, loadDisplayPlugin("TestDisplayPlugin"));

    EXPECT_TRUE(stop());
  }

  // Plugin that inherits from ignition::gui::Plugin
  {
    EXPECT_TRUE(initApp());

    EXPECT_EQ(nullptr, loadDisplayPlugin("Publisher"));

    EXPECT_TRUE(stop());
  }
}

//////////////////////////////////////////////////
TEST(IfaceTest, loadConfig)
{
  setVerbosity(4);

  // Before init
  {
    EXPECT_FALSE(loadConfig("file.config"));
  }

  // Empty string
  {
    EXPECT_TRUE(initApp());

    EXPECT_FALSE(loadConfig(""));

    EXPECT_TRUE(stop());
  }

  // Test config file
  {
    EXPECT_TRUE(initApp());

    // Add test plugin to path (referenced in config)
    auto testBuildPath = std::string(PROJECT_BINARY_PATH) + "/lib/";
    addPluginPath(testBuildPath);

    // Load test config file
    auto testSourcePath = std::string(PROJECT_SOURCE_PATH) + "/test/";
    EXPECT_TRUE(loadConfig(testSourcePath + "config/test.config"));

    EXPECT_TRUE(stop());
  }
}

//////////////////////////////////////////////////
TEST(IfaceTest, loadDefaultConfig)
{
  setVerbosity(4);

  EXPECT_FALSE(defaultConfigPath().empty());

  // Test config file
  {
    EXPECT_TRUE(initApp());

    // Add test plugin to path (referenced in config)
    auto testBuildPath = ignition::common::joinPaths(
      std::string(PROJECT_BINARY_PATH), "lib");
    addPluginPath(testBuildPath);

    // Set default config file
    auto configPath = ignition::common::joinPaths(
      std::string(PROJECT_SOURCE_PATH), "test", "config", "test.config");
    setDefaultConfigPath(configPath);
    EXPECT_TRUE(loadDefaultConfig());
    EXPECT_EQ(defaultConfigPath(), configPath);

    EXPECT_TRUE(stop());
  }
}

//////////////////////////////////////////////////
TEST(IfaceTest, StyleSheet)
{
  setVerbosity(4);

  // Before init
  {
    EXPECT_FALSE(setStyleFromFile(""));
    EXPECT_FALSE(setStyleFromString(""));
  }

  // Qss file with window
  {
    EXPECT_TRUE(initApp());

    // Create main window
    EXPECT_TRUE(createMainWindow());

    auto win = mainWindow();
    EXPECT_TRUE(win != nullptr);

    // Ignition GUI's QSS
    auto bg = win->palette().window().color();
    EXPECT_EQ(bg.name(), "#ededed") << bg.name().toStdString();

    // Load test qss file
    auto testSourcePath = std::string(PROJECT_SOURCE_PATH) + "/test/";
    EXPECT_TRUE(setStyleFromFile(testSourcePath + "styles/red_bg.qss"));

    // Check new style
    bg = win->palette().window().color();
    EXPECT_EQ(bg.name(), "#ff0000");

    // Cleanup
    EXPECT_TRUE(stop());
  }

  // Qss file with dialog
  {
    // Add test plugin to path
    auto testBuildPath = std::string(PROJECT_BINARY_PATH) + "/lib/";
    addPluginPath(testBuildPath);

    // Create app
    EXPECT_TRUE(initApp());

    // Load test plugin
    EXPECT_TRUE(loadPlugin("TestPlugin"));

    // Run dialog
    EXPECT_TRUE(runDialogs());

    // Check it was open
    auto ds = dialogs();
    EXPECT_EQ(ds.size(), 1u);

    // Ignition GUI's QSS
    auto bg = ds[0]->palette().window().color();
    EXPECT_EQ(bg.name(), "#ededed");

    // Load test qss file
    auto testSourcePath = std::string(PROJECT_SOURCE_PATH) + "/test/";
    EXPECT_TRUE(setStyleFromFile(testSourcePath + "styles/red_bg.qss"));

    // Check new style
    bg = ds[0]->palette().window().color();
    EXPECT_EQ(bg.name(), "#ff0000");

    // Wait until it is closed
    auto closed = false;
    ds[0]->connect(ds[0], &QDialog::finished, ds[0], [&](){
      closed = true;
    });

    // Close dialog after some time
    QTimer::singleShot(300, ds[0], SLOT(close()));

    while (!closed)
      QCoreApplication::processEvents();

    EXPECT_TRUE(stop());
  }

  // App with native settings (OS-dependant)
  QColor defaultBg;
  {
    int argc = 1;
    char **argv = nullptr;
    auto app = new QApplication(argc, argv);
    auto win = new MainWindow();
    defaultBg = win->palette().window().color();
    igndbg << "Default bg: " << defaultBg.name().toStdString() << std::endl;
    delete win;
    app->quit();
    delete app;
  }

  // Qt's default style (empty string for sheet)
  {
    EXPECT_TRUE(initApp());

    // Create main window
    EXPECT_TRUE(createMainWindow());

    auto win = mainWindow();
    EXPECT_TRUE(win != nullptr);

    // Ignition GUI's QSS
    auto bg = win->palette().window().color();
    EXPECT_EQ(bg.name(), "#ededed") << bg.name().toStdString();

    // Set style to empty string
    EXPECT_TRUE(setStyleFromString(""));

    // Check new style
    bg = win->palette().window().color();
    EXPECT_EQ(bg.name(), defaultBg.name()) << bg.name().toStdString();

    // Cleanup
    EXPECT_TRUE(stop());
  }

  // Qt's default style (empty <stylesheet> on config)
  {
    EXPECT_TRUE(initApp());

    // Load test config file
    EXPECT_TRUE(loadConfig(std::string(PROJECT_SOURCE_PATH) +
        "/test/config/empty_stylesheet.config"));

    // Create main window
    EXPECT_TRUE(createMainWindow());
    auto win = mainWindow();
    EXPECT_TRUE(win != nullptr);

    // Check style
    auto bg = win->palette().window().color();
    EXPECT_EQ(bg.name(), defaultBg.name()) << bg.name().toStdString();

    // Cleanup
    EXPECT_TRUE(stop());
  }

  // Empty string for file
  {
    EXPECT_TRUE(initApp());

    EXPECT_FALSE(setStyleFromFile(""));

    EXPECT_TRUE(stop());
  }

  // Inexistent file
  {
    EXPECT_TRUE(initApp());

    EXPECT_FALSE(setStyleFromFile("banana"));

    EXPECT_TRUE(stop());
  }
}

//////////////////////////////////////////////////
TEST(IfaceTest, MainWindowNoPlugins)
{
  setVerbosity(4);

  // Try to create window before initializing app
  {
    EXPECT_FALSE(createMainWindow());
    EXPECT_FALSE(runMainWindow());
  }

  // Init app, but don't create window
  {
    EXPECT_TRUE(initApp());

    EXPECT_FALSE(runMainWindow());

    EXPECT_TRUE(stop());
  }

  // Steps in order
  {
    // Create app
    EXPECT_TRUE(initApp());

    // Create main window
    EXPECT_TRUE(createMainWindow());

    auto win = mainWindow();
    EXPECT_TRUE(win != nullptr);

    // Close window after some time
    QTimer::singleShot(300, win, SLOT(close()));

    // Show window
    EXPECT_TRUE(runMainWindow());

    EXPECT_TRUE(stop());
  }
}

//////////////////////////////////////////////////
TEST(IfaceTest, Dialog)
{
  setVerbosity(4);

  // Try to run dialogs before initializing app
  {
    EXPECT_FALSE(runDialogs());
  }

  // Init app first
  {
    // Add test plugin to path
    auto testBuildPath = std::string(PROJECT_BINARY_PATH) + "/lib/";
    addPluginPath(testBuildPath);

    // Create app
    EXPECT_TRUE(initApp());

    // Load test plugin
    EXPECT_TRUE(loadPlugin("TestPlugin"));

    // Run dialog
    EXPECT_TRUE(runDialogs());

    // Check it was open
    auto ds = dialogs();
    EXPECT_EQ(ds.size(), 1u);

    // Wait until it is closed
    auto closed = false;
    ds[0]->connect(ds[0], &QDialog::finished, ds[0], [&](){
      closed = true;
    });

    // Close dialog after some time
    QTimer::singleShot(300, ds[0], SLOT(close()));

    while (!closed)
      QCoreApplication::processEvents();

    EXPECT_TRUE(stop());
  }

  // Multiple dialogs
  {
    // Add test plugin to path
    auto testBuildPath = std::string(PROJECT_BINARY_PATH) + "/lib/";
    addPluginPath(testBuildPath);

    // Create app
    EXPECT_TRUE(initApp());

    // Load 2 test plugins
    EXPECT_TRUE(loadPlugin("TestPlugin"));
    EXPECT_TRUE(loadPlugin("TestPlugin"));

    // Run dialog
    EXPECT_TRUE(runDialogs());

    // Check they were open
    auto ds = dialogs();
    EXPECT_EQ(ds.size(), 2u);

    // Wait until they are closed
    int closed{0};
    for (auto d : ds)
    {
      d->connect(d, &QDialog::finished, d, [&](){
        closed++;
      });
    }

    // Close dialogs after some time
    QTimer::singleShot(300, ds[0], SLOT(close()));
    QTimer::singleShot(300, ds[1], SLOT(close()));

    while (closed != 2)
      QCoreApplication::processEvents();

    EXPECT_TRUE(stop());
  }
}

//////////////////////////////////////////////////
TEST(IfaceTest, runEmptyWindow)
{
  setVerbosity(4);

  // Must initialize app before so we can use the timer on its thread
  EXPECT_TRUE(initApp());
  ASSERT_TRUE(QApplication::instance() != nullptr);

  // Close window after 1 s
  bool closed = false;
  QTimer::singleShot(300, [&] {
    auto win = mainWindow();
    EXPECT_TRUE(win != nullptr);
    win->close();
    closed = true;
  });

  // Run empty window
  EXPECT_TRUE(runEmptyWindow());

  // Make sure timer was triggered
  EXPECT_TRUE(closed);
}

//////////////////////////////////////////////////
TEST(IfaceTest, runStandalone)
{
  setVerbosity(4);

  // Empty string
  {
    EXPECT_FALSE(runStandalone(""));
  }

  // Bad file
  {
    EXPECT_FALSE(runStandalone("badfile"));
  }

  // Good file
  {
    // Must initialize app before so we can use the timer on its thread
    EXPECT_TRUE(initApp());
    ASSERT_TRUE(QApplication::instance() != nullptr);

    // Add test plugin to path
    auto testBuildPath = std::string(PROJECT_BINARY_PATH) + "/lib/";
    addPluginPath(testBuildPath);

    // Close dialog after 1 s
    bool closed = false;
    QTimer *timer = new QTimer();
    timer->setSingleShot(true);
    timer->moveToThread(QApplication::instance()->thread());
    timer->setInterval(300);
    timer->connect(timer, &QTimer::timeout, [&] {
      auto widgets = QApplication::topLevelWidgets();
      EXPECT_EQ(widgets.size(), 1);

      auto dialog = qobject_cast<QDialog *>(widgets[0]);
      EXPECT_TRUE(dialog != nullptr);

      dialog->close();
      closed = true;
    });
    timer->start();

    // Run test plugin
    EXPECT_TRUE(runStandalone("TestPlugin"));

    // Make sure timer was triggered
    EXPECT_TRUE(closed);
  }
}

//////////////////////////////////////////////////
TEST(IfaceTest, runConfig)
{
  setVerbosity(4);

  // Empty string
  {
    EXPECT_FALSE(runConfig(""));
  }

  // Bad file
  {
    EXPECT_FALSE(runConfig("badfile"));
  }

  // Good file
  {
    // Must initialize app before so we can use the timer on its thread
    EXPECT_TRUE(initApp());
    ASSERT_TRUE(QApplication::instance() != nullptr);

    // // Add test plugin to path
    auto testBuildPath = std::string(PROJECT_BINARY_PATH) + "/lib/";
    addPluginPath(testBuildPath);

    // Close window after 1 s
    bool closed = false;
    QTimer::singleShot(300, [&] {
      auto win = mainWindow();
      EXPECT_TRUE(win != nullptr);
      win->close();
      closed = true;
    });

    // Run test config file
    auto testSourcePath = std::string(PROJECT_SOURCE_PATH) + "/test/";
    EXPECT_TRUE(runConfig(testSourcePath + "config/test.config"));

    // Make sure timer was triggered
    EXPECT_TRUE(closed);
  }
}

/////////////////////////////////////////////////
TEST(IfaceTest, messageHandler)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // \todo Verify output, see ignition::commmon::Console_TEST for example
  qDebug("This came from qDebug");
  qInfo("This came from qInfo");
  qWarning("This came from qWarning");
  qCritical("This came from qCritical");

  EXPECT_TRUE(stop());
}

//////////////////////////////////////////////////
TEST(IfaceTest, menuConfig)
{
  setVerbosity(4);

  // No menus
  {
    EXPECT_TRUE(initApp());

    // Load config file
    EXPECT_TRUE(loadConfig(common::joinPaths(std::string(PROJECT_SOURCE_PATH),
        "test", "config", "no_menus.config")));

    // Create main window
    EXPECT_TRUE(createMainWindow());
    auto win = mainWindow();
    EXPECT_TRUE(win != nullptr);

    QCoreApplication::processEvents();

    // Check there are actions
    auto actions = win->menuBar()->actions();
    EXPECT_GT(actions.size(), 0);

    // Check there are no visible menus
    for (auto action : actions)
    {
      EXPECT_FALSE(action->isVisible()) << action->text().toStdString();
    }

    EXPECT_TRUE(stop());
  }

  // Selected plugins are visible on menu
  {
    EXPECT_TRUE(initApp());

    // Load config file
    EXPECT_TRUE(loadConfig(common::joinPaths(std::string(PROJECT_SOURCE_PATH),
        "test", "config", "plugins_menu.config")));

    // Create main window
    EXPECT_TRUE(createMainWindow());
    auto win = mainWindow();
    EXPECT_TRUE(win != nullptr);

    QCoreApplication::processEvents();

    // Get plugins menu
    auto pluginsMenu = win->menuBar()->findChild<QMenu *>("pluginsMenu");
    ASSERT_NE(pluginsMenu, nullptr);

    // Check it has at least 2 plugins
    EXPECT_GT(pluginsMenu->actions().size(), 2);

    auto count = 0;
    // Check that only 2 plugins are visible
    for (auto action : pluginsMenu->actions())
    {
      if (action->isVisible())
        count++;
    }
    EXPECT_EQ(2, count);

    EXPECT_TRUE(stop());
  }
}

//////////////////////////////////////////////////
TEST(IfaceTest, ignoreConfig)
{
  setVerbosity(4);

  // Ignore position and size
  {
    EXPECT_TRUE(initApp());

    // Load config file
    EXPECT_TRUE(loadConfig(common::joinPaths(std::string(PROJECT_SOURCE_PATH),
        "test", "config", "ignore.config")));

    // Create main window
    EXPECT_TRUE(createMainWindow());
    auto win = mainWindow();
    ASSERT_NE(nullptr, win);

    QCoreApplication::processEvents();

    // Check the position and size were not applied
    EXPECT_NE(1000, win->pos().x());
    EXPECT_NE(1000, win->pos().y());
    EXPECT_NE(1000, win->width());
    EXPECT_NE(1000, win->height());

    EXPECT_TRUE(stop());
  }
}

//////////////////////////////////////////////////
bool pluginInPluginList(std::string _plugin,
    std::vector<std::pair<std::string, std::vector<std::string>>> _list)
{
  for (auto pathPair : _list)
  {
    auto pluginsInPath = pathPair.second;
    bool pluginInPath = std::find(pluginsInPath.begin(),
        pluginsInPath.end(), _plugin) != pluginsInPath.end();
    if (pluginInPath)
      return true;
  }
  return false;
}

//////////////////////////////////////////////////
TEST(IfaceTest, getPluginList)
{
  setVerbosity(4);

  {
    EXPECT_TRUE(initApp());

    EXPECT_TRUE(pluginInPluginList("libPublisher.so", getPluginList()));
    EXPECT_FALSE(pluginInPluginList("libTestPlugin.so", getPluginList()));
    EXPECT_FALSE(pluginInPluginList("lib_doesnt_exist.so", getPluginList()));

    // Display plugins shouldn't be detected
    EXPECT_FALSE(pluginInPluginList("libDisplayTestPlugin.so",
        getPluginList()));

    EXPECT_TRUE(stop());
  }

  // Plugin path added programmatically
  {
    EXPECT_TRUE(initApp());

    addPluginPath(std::string(PROJECT_BINARY_PATH) + "/lib");
    EXPECT_TRUE(pluginInPluginList("libTestPlugin.so", getPluginList()));

    EXPECT_TRUE(stop());
  }

  // Plugin path added by env var
  {
    EXPECT_TRUE(initApp());

    setenv("TEST_ENV_VAR",
        (std::string(PROJECT_BINARY_PATH) + "/lib").c_str(), 1);
    setPluginPathEnv("TEST_ENV_VAR");
    EXPECT_TRUE(pluginInPluginList("libTestPlugin.so", getPluginList()));

    EXPECT_TRUE(stop());
  }
}