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

#include "test_config.h"  // NOLINT(build/include)
#include "ignition/gui/Iface.hh"
#include "ignition/gui/Plugin.hh"

#include "ignition/gui/MainWindow.hh"

std::string kTestConfigFile = "/tmp/ign-gui-test.config";

using namespace ignition;
using namespace gui;

/////////////////////////////////////////////////
TEST(MainWindowTest, Constructor)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // Constructor
  auto mainWindow = new MainWindow;
  EXPECT_TRUE(mainWindow);

  // Menu
  auto menus = mainWindow->menuBar()->findChildren<QMenu *>();
  EXPECT_EQ(menus[0]->title(), QString("&File"));
  EXPECT_EQ(menus[1]->title(), QString("&Plugins"));

  delete mainWindow;
  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(MainWindowTest, OnSaveConfig)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // Change default config path
  setDefaultConfigPath(kTestConfigFile);

  // Create window
  auto mainWindow = new MainWindow;
  EXPECT_TRUE(mainWindow);

  // Get save action on menu
  auto menus = mainWindow->menuBar()->findChildren<QMenu *>();
  ASSERT_GT(menus.size(), 0);
  ASSERT_GT(menus[0]->actions().size(), 2);
  auto saveAct = menus[0]->actions()[1];
  EXPECT_EQ(saveAct->text().toStdString(), "&Save configuration");

  // Save to default location
  {
    // Trigger save
    saveAct->trigger();

    // Check saved file
    QFile saved(QString::fromStdString(kTestConfigFile));
    ASSERT_TRUE(saved.open(QFile::ReadOnly));

    QString savedStr = QLatin1String(saved.readAll());
    EXPECT_FALSE(savedStr.isNull());
    EXPECT_TRUE(savedStr.contains("<window>"));
    EXPECT_TRUE(savedStr.contains("<height>"));
    EXPECT_TRUE(savedStr.contains("<width>"));
    EXPECT_TRUE(savedStr.contains("<position_x>"));
    EXPECT_TRUE(savedStr.contains("<position_y>"));
    EXPECT_TRUE(savedStr.contains("<stylesheet>"));
    EXPECT_TRUE(savedStr.contains("<state>"));

    // Delete file
    std::remove(kTestConfigFile.c_str());
  }

  delete mainWindow;
  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(MainWindowTest, OnSaveConfigAs)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  auto mainWindow = new MainWindow;
  EXPECT_TRUE(mainWindow);

  // Get save action on menu
  auto menus = mainWindow->menuBar()->findChildren<QMenu *>();
  ASSERT_GT(menus.size(), 0);
  ASSERT_GT(menus[0]->actions().size(), 2);
  auto saveAct = menus[0]->actions()[2];
  EXPECT_EQ(saveAct->text().toStdString(), "Save configuration as");

  bool closed = false;

  // Close dialog without choosing file
  {
    // Close window after a while
    QTimer::singleShot(300, [&]
    {
      auto fileDialogs = mainWindow->findChildren<QFileDialog *>();
      ASSERT_EQ(fileDialogs.size(), 1);
      fileDialogs[0]->close();
      closed = true;
    });

    // Trigger Save
    saveAct->trigger();

    EXPECT_TRUE(closed);
  }

  // Save to file
  {
    // Close window after 1 s
    closed = false;
    QTimer::singleShot(300, [&]
    {
      auto fileDialogs = mainWindow->findChildren<QFileDialog *>();
      ASSERT_EQ(fileDialogs.size(), 1);

      // Select file
      auto edits = fileDialogs[0]->findChildren<QLineEdit *>();
      ASSERT_GT(edits.size(), 0);
      edits[0]->setText(QString::fromStdString(kTestConfigFile));

      // Accept
      auto buttons = fileDialogs[0]->findChildren<QPushButton *>();
      EXPECT_GT(buttons.size(), 0);
      buttons[0]->click();
      closed = true;
    });

    // Trigger save
    saveAct->trigger();

    // Check saved file
    QFile saved(QString::fromStdString(kTestConfigFile));
    ASSERT_TRUE(saved.open(QFile::ReadOnly));

    QString savedStr = QLatin1String(saved.readAll());
    EXPECT_FALSE(savedStr.isNull());
    EXPECT_TRUE(savedStr.contains("<window>"));
    EXPECT_TRUE(savedStr.contains("<height>"));
    EXPECT_TRUE(savedStr.contains("<width>"));
    EXPECT_TRUE(savedStr.contains("<position_x>"));
    EXPECT_TRUE(savedStr.contains("<position_y>"));
    EXPECT_TRUE(savedStr.contains("<stylesheet>"));
    EXPECT_TRUE(savedStr.contains("<state>"));

    // Delete file
    std::remove(kTestConfigFile.c_str());

    EXPECT_TRUE(closed);
  }

  delete mainWindow;
  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(MainWindowTest, OnLoadConfig)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // Add test plugins to path
  addPluginPath(std::string(PROJECT_BINARY_PATH) + "/test/plugins");

  // Create main window
  createMainWindow();
  auto mainWindow = ignition::gui::mainWindow();
  EXPECT_TRUE(mainWindow);

  // Check window doesn't have any plugins
  auto plugins = mainWindow->findChildren<Plugin *>();
  EXPECT_EQ(plugins.size(), 0);

  // Get load action on menu
  auto menus = mainWindow->menuBar()->findChildren<QMenu *>();
  ASSERT_GT(menus.size(), 0);
  ASSERT_GT(menus[0]->actions().size(), 1);
  auto loadAct = menus[0]->actions()[0];
  EXPECT_EQ(loadAct->text(), QString("&Load configuration"));

  bool closed = false;

  // Close dialog without choosing file
  {
    // Close window after 1 s
    QTimer::singleShot(300, [&]
    {
      auto fileDialogs = mainWindow->findChildren<QFileDialog *>();
      ASSERT_EQ(fileDialogs.size(), 1);
      fileDialogs[0]->close();
      closed = true;
    });

    // Trigger load
    loadAct->trigger();

    EXPECT_TRUE(closed);
  }

  // Load file with single plugin
  {
    // Close window after 1 s
    closed = false;
    QTimer::singleShot(300, [&]
    {
      auto fileDialogs = mainWindow->findChildren<QFileDialog *>();
      ASSERT_EQ(fileDialogs.size(), 1);

      // Select file
      auto edits = fileDialogs[0]->findChildren<QLineEdit *>();
      ASSERT_GT(edits.size(), 0);
      edits[0]->setText(QString::fromStdString(
          std::string(PROJECT_SOURCE_PATH) + "/test/config/test.config"));

      // Accept
      auto buttons = fileDialogs[0]->findChildren<QPushButton *>();
      EXPECT_GT(buttons.size(), 0);
      buttons[0]->click();
      closed = true;
    });

    // Trigger load
    loadAct->trigger();

    EXPECT_TRUE(closed);

    // Check window has 1 plugin
    plugins = mainWindow->findChildren<Plugin *>();
    EXPECT_EQ(plugins.size(), 1);
  }

  // Load file with 2 plugins and window state
  {
    // Close window after 1 s
    closed = false;
    QTimer::singleShot(300, [&]
    {
      auto fileDialogs = mainWindow->findChildren<QFileDialog *>();
      ASSERT_EQ(fileDialogs.size(), 1);

      // Select file
      auto edits = fileDialogs[0]->findChildren<QLineEdit *>();
      ASSERT_GT(edits.size(), 0);
      edits[0]->setText(QString::fromStdString(
          std::string(PROJECT_SOURCE_PATH) + "/test/config/state.config"));

      // Accept
      auto buttons = fileDialogs[0]->findChildren<QPushButton *>();
      EXPECT_GT(buttons.size(), 0);
      buttons[0]->click();
      closed = true;
    });

    // Trigger load
    loadAct->trigger();

    EXPECT_TRUE(closed);

    // Check window has 2 plugins
    plugins = mainWindow->findChildren<Plugin *>();
    EXPECT_EQ(plugins.size(), 2);
  }

  // Load file with stylesheet
  {
    // Check window style
    auto bg = mainWindow->palette().window().color();
    EXPECT_NE(bg.name(), "#0000ff");

    // Close window after 1 s
    closed = false;
    QTimer::singleShot(300, [&]
    {
      auto fileDialogs = mainWindow->findChildren<QFileDialog *>();
      ASSERT_EQ(fileDialogs.size(), 1);

      // Select file
      auto edits = fileDialogs[0]->findChildren<QLineEdit *>();
      ASSERT_GT(edits.size(), 0);
      edits[0]->setText(QString::fromStdString(
          std::string(PROJECT_SOURCE_PATH) + "/test/config/stylesheet.config"));

      // Accept
      auto buttons = fileDialogs[0]->findChildren<QPushButton *>();
      EXPECT_GT(buttons.size(), 0);
      buttons[0]->click();
      closed = true;
    });

    // Trigger load
    loadAct->trigger();

    EXPECT_TRUE(closed);

    // Check window style
    bg = mainWindow->palette().window().color();
    EXPECT_EQ(bg.name(), "#0000ff");
  }

  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(MainWindowTest, OnLoadStyleSheet)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // Create main window
  createMainWindow();
  auto mainWindow = ignition::gui::mainWindow();
  EXPECT_TRUE(mainWindow);

  // Check window has Ignition GUI's default style
  auto bg = mainWindow->palette().window().color();
  EXPECT_EQ(bg.name(), "#ededed");

  // Get load action on menu
  auto menus = mainWindow->menuBar()->findChildren<QMenu *>();
  ASSERT_GT(menus.size(), 0);
  ASSERT_GT(menus[0]->actions().size(), 3);
  auto loadAct = menus[0]->actions()[4];
  EXPECT_EQ(loadAct->text(), QString("&Load stylesheet"));

  bool closed = false;

  // Close dialog without choosing file
  {
    // Close window after 1 s
    QTimer::singleShot(300, [&]
    {
      auto fileDialogs = mainWindow->findChildren<QFileDialog *>();
      ASSERT_EQ(fileDialogs.size(), 1);
      fileDialogs[0]->close();
      closed = true;
    });

    // Trigger load
    loadAct->trigger();

    EXPECT_TRUE(closed);
  }

  // Load test stylesheet
  {
    // Close window after 1 s
    closed = false;
    QTimer::singleShot(300, [&]
    {
      auto fileDialogs = mainWindow->findChildren<QFileDialog *>();
      ASSERT_EQ(fileDialogs.size(), 1);

      // Select file
      auto edits = fileDialogs[0]->findChildren<QLineEdit *>();
      ASSERT_GT(edits.size(), 0);
      edits[0]->setText(QString::fromStdString(
          std::string(PROJECT_SOURCE_PATH) + "/test/styles/red_bg.qss"));

      // Accept
      auto buttons = fileDialogs[0]->findChildren<QPushButton *>();
      EXPECT_GT(buttons.size(), 0);
      buttons[0]->click();
      closed = true;
    });

    // Trigger load
    loadAct->trigger();

    EXPECT_TRUE(closed);

    // Check style was applied
    bg = mainWindow->palette().window().color();
    EXPECT_EQ(bg.name(), "#ff0000");
  }

  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(MainWindowTest, OnAddPlugin)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // Add test plugins to path
  addPluginPath(std::string(PROJECT_BINARY_PATH) + "/test/plugins");

  // Create window
  createMainWindow();
  auto mainWindow = ignition::gui::mainWindow();
  ASSERT_TRUE(mainWindow != nullptr);

  // Check window doesn't have any plugins
  auto plugins = mainWindow->findChildren<Plugin *>();
  EXPECT_EQ(plugins.size(), 0);

  // Get the TestPlugin plugin on menu
  auto menus = mainWindow->menuBar()->findChildren<QMenu *>();
  ASSERT_GT(menus.size(), 1);
  ASSERT_GT(menus[0]->actions().size(), 0);

  int i = 0;
  for (; menus[1]->actions().size(); ++i)
  {
    if (menus[1]->actions()[i]->text() == QString("TestPlugin"))
      break;
  }
  auto pluginAct = menus[1]->actions()[i];
  EXPECT_EQ(pluginAct->text(), QString("TestPlugin"));

  // Add plugin
  pluginAct->trigger();

  QCoreApplication::processEvents();

  // Check window has 1 plugin
  plugins = mainWindow->findChildren<Plugin *>();
  EXPECT_EQ(plugins.size(), 1);

  // Add another plugin
  pluginAct->trigger();

  QCoreApplication::processEvents();

  // Check window has 2 plugins
  plugins = mainWindow->findChildren<Plugin *>();
  EXPECT_EQ(plugins.size(), 2);

  // Clean up
  EXPECT_TRUE(stop());
}

