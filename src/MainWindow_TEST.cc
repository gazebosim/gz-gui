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
#include <thread>

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

  // This test hangs on ubuntu bionic
  // Save to file
  // {
  //   // Choose file after a while
  //   closed = false;
  //   QTimer::singleShot(300, [&]
  //   {
  //     auto fileDialogs = mainWindow->findChildren<QFileDialog *>();
  //     ASSERT_EQ(fileDialogs.size(), 1);

  //     // Select file
  //     auto edits = fileDialogs[0]->findChildren<QLineEdit *>();
  //     ASSERT_GT(edits.size(), 0);
  //     edits[0]->setText(QString::fromStdString(kTestConfigFile));

  //     // Accept
  //     auto buttons = fileDialogs[0]->findChildren<QPushButton *>();
  //     EXPECT_GT(buttons.size(), 0);
  //     buttons[0]->click();
  //     closed = true;
  //   });

  //   // Trigger save
  //   saveAct->trigger();

  //   // Check saved file
  //   QFile saved(QString::fromStdString(kTestConfigFile));
  //   ASSERT_TRUE(saved.open(QFile::ReadOnly));

  //   QString savedStr = QLatin1String(saved.readAll());
  //   EXPECT_FALSE(savedStr.isNull());
  //   EXPECT_TRUE(savedStr.contains("<window>"));
  //   EXPECT_TRUE(savedStr.contains("<height>"));
  //   EXPECT_TRUE(savedStr.contains("<width>"));
  //   EXPECT_TRUE(savedStr.contains("<position_x>"));
  //   EXPECT_TRUE(savedStr.contains("<position_y>"));
  //   EXPECT_TRUE(savedStr.contains("<stylesheet>"));
  //   EXPECT_TRUE(savedStr.contains("<state>"));
  //   EXPECT_TRUE(savedStr.contains("<menus>"));
  //   EXPECT_TRUE(savedStr.contains("<file"));
  //   EXPECT_TRUE(savedStr.contains("<plugins"));

  //   // Delete file
  //   std::remove(kTestConfigFile.c_str());

  //   EXPECT_TRUE(closed);
  // }

  delete mainWindow;
  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(MainWindowTest, OnLoadConfig)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // Add test plugins to path
  addPluginPath(std::string(PROJECT_BINARY_PATH) + "/lib");

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

  // These tests hang on ubuntu bionic
  // Load file with single plugin
  // {
  //   // Close window after 1 s
  //   closed = false;
  //   QTimer::singleShot(300, [&]
  //   {
  //     auto fileDialogs = mainWindow->findChildren<QFileDialog *>();
  //     ASSERT_EQ(fileDialogs.size(), 1);

  //     // Select file
  //     auto edits = fileDialogs[0]->findChildren<QLineEdit *>();
  //     ASSERT_GT(edits.size(), 0);
  //     edits[0]->setText(QString::fromStdString(
  //         std::string(PROJECT_SOURCE_PATH) + "/test/config/test.config"));

  //     // Accept
  //     auto buttons = fileDialogs[0]->findChildren<QPushButton *>();
  //     EXPECT_GT(buttons.size(), 0);
  //     buttons[0]->click();
  //     closed = true;
  //   });

  //   // Trigger load
  //   loadAct->trigger();

  //   EXPECT_TRUE(closed);

  //   // Check window has 1 plugin
  //   plugins = mainWindow->findChildren<Plugin *>();
  //   EXPECT_EQ(1, plugins.size());
  // }

  // // Load file with 2 plugins and window state
  // {
  //   // Close window after 1 s
  //   closed = false;
  //   QTimer::singleShot(300, [&]
  //   {
  //     auto fileDialogs = mainWindow->findChildren<QFileDialog *>();
  //     ASSERT_EQ(fileDialogs.size(), 1);

  //     // Select file
  //     auto edits = fileDialogs[0]->findChildren<QLineEdit *>();
  //     ASSERT_GT(edits.size(), 0);
  //     edits[0]->setText(QString::fromStdString(
  //         std::string(PROJECT_SOURCE_PATH) + "/test/config/state.config"));

  //     // Accept
  //     auto buttons = fileDialogs[0]->findChildren<QPushButton *>();
  //     EXPECT_GT(buttons.size(), 0);
  //     buttons[0]->click();
  //     closed = true;
  //   });

  //   // Trigger load
  //   loadAct->trigger();

  //   EXPECT_TRUE(closed);

  //   // Check window has 2 plugins
  //   plugins = mainWindow->findChildren<Plugin *>();
  //   EXPECT_EQ(2, plugins.size());
  // }

  // // Load file with stylesheet
  // {
  //   // Check window style
  //   auto bg = mainWindow->palette().window().color();
  //   EXPECT_NE(bg.name(), "#0000ff");

  //   // Close window after 1 s
  //   closed = false;
  //   QTimer::singleShot(300, [&]
  //   {
  //     auto fileDialogs = mainWindow->findChildren<QFileDialog *>();
  //     ASSERT_EQ(fileDialogs.size(), 1);

  //     // Select file
  //     auto edits = fileDialogs[0]->findChildren<QLineEdit *>();
  //     ASSERT_GT(edits.size(), 0);
  //     edits[0]->setText(QString::fromStdString(
  //         std::string(PROJECT_SOURCE_PATH) +
  //         "/test/config/stylesheet.config"));

  //     // Accept
  //     auto buttons = fileDialogs[0]->findChildren<QPushButton *>();
  //     EXPECT_GT(buttons.size(), 0);
  //     buttons[0]->click();
  //     closed = true;
  //   });

  //   // Trigger load
  //   loadAct->trigger();

  //   EXPECT_TRUE(closed);

  //   // Check window style
  //   bg = mainWindow->palette().window().color();
  //   EXPECT_EQ(bg.name(), "#0000ff");
  // }

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

  // This test hangs on ubuntu bionic
  // Load test stylesheet
  // {
  //   // Close window after 1 s
  //   closed = false;
  //   QTimer::singleShot(300, [&]
  //   {
  //     auto fileDialogs = mainWindow->findChildren<QFileDialog *>();
  //     ASSERT_EQ(fileDialogs.size(), 1);

  //     // Select file
  //     auto edits = fileDialogs[0]->findChildren<QLineEdit *>();
  //     ASSERT_GT(edits.size(), 0);
  //     edits[0]->setText(QString::fromStdString(
  //         std::string(PROJECT_SOURCE_PATH) + "/test/styles/red_bg.qss"));

  //     // Accept
  //     auto buttons = fileDialogs[0]->findChildren<QPushButton *>();
  //     EXPECT_GT(buttons.size(), 0);
  //     buttons[0]->click();
  //     closed = true;
  //   });

  //   // Trigger load
  //   loadAct->trigger();

  //   EXPECT_TRUE(closed);

  //   // Check style was applied
  //   bg = mainWindow->palette().window().color();
  //   EXPECT_EQ(bg.name(), "#ff0000");
  // }

  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(MainWindowTest, OnAddPlugin)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // Add test plugins to path
  addPluginPath(std::string(PROJECT_BINARY_PATH) + "/lib");

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

/////////////////////////////////////////////////
TEST(WindowConfigTest, defaultValues)
{
  setVerbosity(4);

  WindowConfig c;

  EXPECT_EQ(c.posX, -1);
  EXPECT_EQ(c.posY, -1);
  EXPECT_EQ(c.width, -1);
  EXPECT_EQ(c.height, -1);
  EXPECT_TRUE(c.state.isEmpty());
  EXPECT_TRUE(c.styleSheet.empty());
  EXPECT_TRUE(c.menuVisibilityMap.empty());
  EXPECT_TRUE(c.pluginsFromPaths);
  EXPECT_TRUE(c.showPlugins.empty());
  EXPECT_TRUE(c.ignoredProps.empty());

  auto xml = c.XMLString();

  EXPECT_NE(xml.find("<window>"), std::string::npos);
  EXPECT_NE(xml.find("<position_x>"), std::string::npos);
  EXPECT_NE(xml.find("<position_y>"), std::string::npos);
  EXPECT_NE(xml.find("<width>"), std::string::npos);
  EXPECT_NE(xml.find("<height>"), std::string::npos);
  EXPECT_NE(xml.find("<menus>"), std::string::npos);
  EXPECT_NE(xml.find("<file"), std::string::npos);
  EXPECT_NE(xml.find("<plugins"), std::string::npos);
  EXPECT_EQ(xml.find("<ignore>"), std::string::npos);
}

/////////////////////////////////////////////////
TEST(WindowConfigTest, mergeFromXML)
{
  setVerbosity(4);

  WindowConfig c;

  // Set some values
  c.posX = 500;
  c.posY = 400;
  c.width = 1000;
  c.height = 600;
  c.ignoredProps.insert("state");

  // Merge from XML
  c.MergeFromXML(std::string("<window><position_x>5000</position_x>")+
    "<menus><plugins from_paths=\"false\"/></menus>" +
    "<ignore>size</ignore></window>");

  // Check values
  EXPECT_EQ(c.posX, 5000);
  EXPECT_EQ(c.posY, 400);
  EXPECT_EQ(c.width, 1000);
  EXPECT_EQ(c.height, 600);
  EXPECT_TRUE(c.state.isEmpty());
  EXPECT_TRUE(c.styleSheet.empty());
  EXPECT_TRUE(c.menuVisibilityMap.empty());
  EXPECT_FALSE(c.pluginsFromPaths);
  EXPECT_TRUE(c.showPlugins.empty());
  EXPECT_EQ(c.ignoredProps.size(), 2u);
  EXPECT_TRUE(c.IsIgnoring("state"));
  EXPECT_TRUE(c.IsIgnoring("size"));
}

/////////////////////////////////////////////////
TEST(WindowConfigTest, MenusToString)
{
  setVerbosity(4);

  WindowConfig c;

  // Set some menu-related properties
  c.menuVisibilityMap["file"] = false;
  c.menuVisibilityMap["plugins"] = true;

  c.pluginsFromPaths = false;

  c.showPlugins.push_back("PluginA");
  c.showPlugins.push_back("PluginB");

  // Check generated string
  auto str = c.XMLString();
  EXPECT_FALSE(str.empty());

  EXPECT_TRUE(str.find("<file visible=\"0\"/>") != std::string::npos ||
              str.find("<file visible=\"false\"/>") != std::string::npos);
  EXPECT_TRUE(str.find("<plugins visible=\"1\" from_paths=\"0\">") !=
      std::string::npos ||
      str.find("<plugins visible=\"true\" from_paths=\"false\">") !=
      std::string::npos);

  EXPECT_NE(str.find("<show>PluginA</show>"), std::string::npos) << str;
  EXPECT_NE(str.find("<show>PluginB</show>"), std::string::npos) << str;
  EXPECT_EQ(str.find("<show>PluginC</show>"), std::string::npos) << str;
}

/////////////////////////////////////////////////
TEST(WindowConfigTest, IgnoreToString)
{
  setVerbosity(4);

  WindowConfig c;

  // Set some ignored properties
  c.ignoredProps.insert("position");
  c.ignoredProps.insert("size");

  // Check generated string
  auto str = c.XMLString();
  EXPECT_FALSE(str.empty());

  // Ignored properties are not present
  EXPECT_EQ(str.find("<position_x>"), std::string::npos) << str;
  EXPECT_EQ(str.find("<position_y>"), std::string::npos) << str;
  EXPECT_EQ(str.find("<width>"), std::string::npos) << str;
  EXPECT_EQ(str.find("<height>"), std::string::npos) << str;

  // Ignore blocks are persisted
  EXPECT_NE(str.find("<ignore>position</ignore>"), std::string::npos) << str;
  EXPECT_NE(str.find("<ignore>size</ignore>"), std::string::npos) << str;
}

/////////////////////////////////////////////////
TEST(MainWindowTest, CloseWithoutSavingChanges)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // Create main window
  EXPECT_TRUE(createMainWindow());

  // Access window after it's open
  bool closed{false};
  QTimer::singleShot(300, [&closed]
  {
    auto win = mainWindow();
    ASSERT_NE(nullptr, win);
    EXPECT_TRUE(win->isVisible());

    for (unsigned int i = 0; i < 100; ++i)
    {
      win->resize(10+i, 10+2*i);
      QCoreApplication::processEvents();
      std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    // Access dialog after it's open
    bool dialogClosed{false};
    QTimer::singleShot(300, [&]
    {
      auto fileDialogs = win->findChildren<QDialog *>();
      ASSERT_EQ(fileDialogs.size(), 1);

      auto closeButton = fileDialogs[0]->findChild<QPushButton *>(
          "closeConfirmationDialogCloseButton");

      closeButton->click();
      dialogClosed = true;
    });

    win->close();
    EXPECT_TRUE(dialogClosed);

    closed = true;
  });

  // Show window
  EXPECT_TRUE(runMainWindow());

  EXPECT_TRUE(closed);

  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(MainWindowTest, ApplyConfig)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // Main window
  auto mainWindow = new MainWindow;
  ASSERT_TRUE(mainWindow);

  // Default config
  {
    auto c = mainWindow->CurrentWindowConfig();
    EXPECT_TRUE(c.menuVisibilityMap.empty());
    EXPECT_TRUE(c.pluginsFromPaths);
    EXPECT_TRUE(c.showPlugins.empty());
    EXPECT_TRUE(c.ignoredProps.empty());
  }

  // Apply a config
  {
    WindowConfig c;
    c.posX = 1000;
    c.posY = 2000;
    c.width = 100;
    c.height = 200;
    c.styleSheet = "pineapple";
    c.menuVisibilityMap["File"] = false;
    c.pluginsFromPaths = false;
    c.showPlugins.push_back("watermelon");
    c.ignoredProps.insert("position");

    mainWindow->ApplyConfig(c);
  }

  // Check applied config
  {
    auto c = mainWindow->CurrentWindowConfig();

    // ignored
    EXPECT_NE(c.posX, 1000);
    EXPECT_NE(c.posY, 2000);

    EXPECT_EQ(c.width, 100);
    EXPECT_EQ(c.height, 200);
    EXPECT_EQ(c.styleSheet, "pineapple");
    EXPECT_FALSE(c.menuVisibilityMap["File"]);
    EXPECT_FALSE(c.pluginsFromPaths);
    EXPECT_EQ(c.showPlugins.size(), 1u);
    EXPECT_EQ(c.ignoredProps.size(), 1u);
  }

  delete mainWindow;
  EXPECT_TRUE(stop());
}

