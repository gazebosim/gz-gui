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
#include "ignition/gui/MainWindow.hh"

using namespace ignition;
using namespace gui;

/////////////////////////////////////////////////
TEST(MainWindowTest, Constructor)
{
  EXPECT_TRUE(initApp());

  // Constructor
  auto mainWindow = new MainWindow;
  EXPECT_TRUE(mainWindow);

  // Menu
  auto menus = mainWindow->menuBar()->findChildren<QMenu*>();
  EXPECT_EQ(menus[0]->title(), QString("&File"));
  EXPECT_EQ(menus[1]->title(), QString("&Plugins"));

  delete mainWindow;
  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(MainWindowTest, OnSaveConfig)
{
  EXPECT_TRUE(initApp());

  auto mainWindow = new MainWindow;
  EXPECT_TRUE(mainWindow);

  // Get save action on menu
  auto menus = mainWindow->menuBar()->findChildren<QMenu*>();
  ASSERT_GT(menus.size(), 0);
  ASSERT_GT(menus[0]->actions().size(), 2);
  auto saveAct = menus[0]->actions()[1];
  EXPECT_EQ(saveAct->text(), QString("&Save configuration"));

  bool closed = false;

  // Close dialog without choosing file
  {
    // Close window after 1 s
    QTimer::singleShot(1000, [&]
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
    QTimer::singleShot(1000, [&]
    {
      auto fileDialogs = mainWindow->findChildren<QFileDialog *>();
      ASSERT_EQ(fileDialogs.size(), 1);

      // Select file
      auto edits = fileDialogs[0]->findChildren<QLineEdit *>();
      ASSERT_GT(edits.size(), 0);
      edits[0]->setText(QString("/tmp/ign-gui-test.config"));

      // Accept
      auto buttons = fileDialogs[0]->findChildren<QPushButton *>();
      EXPECT_GT(buttons.size(), 0);
      buttons[0]->click();
      closed = true;
    });

    // Trigger save
    saveAct->trigger();
    std::remove("/tmp/ign-gui-test.config");

    EXPECT_TRUE(closed);
  }

  delete mainWindow;
  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(MainWindowTest, OnLoadConfig)
{
  EXPECT_TRUE(initApp());

  auto mainWindow = new MainWindow;
  EXPECT_TRUE(mainWindow);

  // Get load action on menu
  auto menus = mainWindow->menuBar()->findChildren<QMenu*>();
  ASSERT_GT(menus.size(), 0);
  ASSERT_GT(menus[0]->actions().size(), 1);
  auto loadAct = menus[0]->actions()[0];
  EXPECT_EQ(loadAct->text(), QString("&Load configuration"));

  bool closed = false;

  // Close dialog without choosing file
  {
    // Close window after 1 s
    QTimer::singleShot(1000, [&]
    {
      auto fileDialogs = mainWindow->findChildren<QFileDialog *>();
      ASSERT_EQ(fileDialogs.size(), 1);
      fileDialogs[0]->close();
      closed = true;
    });

    // Trigger Save
    loadAct->trigger();

    EXPECT_TRUE(closed);
  }

  // Load file
  {
    // Close window after 1 s
    closed = false;
    QTimer::singleShot(1000, [&]
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
  }

  delete mainWindow;
  EXPECT_TRUE(stop());
}

