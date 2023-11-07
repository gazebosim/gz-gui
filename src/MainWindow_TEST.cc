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
#include <chrono>
#include <thread>

#include <QQmlProperty>

#include <gz/common/Console.hh>
#include <gz/msgs/boolean.pb.h>
#include <gz/msgs/server_control.pb.h>
#include <gz/transport/Node.hh>
#include <gz/utils/ExtraTestMacros.hh>

#include "test_config.hh"  // NOLINT(build/include)
#include "gz/gui/Application.hh"
#include "gz/gui/MainWindow.hh"
#include "gz/gui/Plugin.hh"

std::string kTestConfigFile = "/tmp/gz-gui-test.config"; // NOLINT(*)
int g_argc = 1;
char* g_argv[] =
{
  reinterpret_cast<char*>(const_cast<char*>("./MainWindow_TEST")),
};

using namespace gz;
using namespace gui;
using namespace std::chrono_literals;

/////////////////////////////////////////////////
// See https://github.com/gazebosim/gz-gui/issues/75
TEST(MainWindowTest, GZ_UTILS_TEST_ENABLED_ONLY_ON_LINUX(Constructor))
{
  common::Console::SetVerbosity(4);
  Application app(g_argc, g_argv);

  // Constructor
  auto mainWindow = new MainWindow;
  ASSERT_NE(nullptr, mainWindow);

  mainWindow->deleteLater();
}

/////////////////////////////////////////////////
TEST(MainWindowTest, GZ_UTILS_TEST_ENABLED_ONLY_ON_LINUX(OnSaveConfig))
{
  common::Console::SetVerbosity(4);
  Application app(g_argc, g_argv);

  // Change default config path
  App()->SetDefaultConfigPath(kTestConfigFile);

  // Create window
  auto mainWindow = new MainWindow;
  ASSERT_NE(nullptr, mainWindow);

  // Save to default location
  {
    // Trigger save
    mainWindow->OnSaveConfig();

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

    // Delete file
    std::remove(kTestConfigFile.c_str());
  }

  mainWindow->deleteLater();
}

/////////////////////////////////////////////////
TEST(MainWindowTest, GZ_UTILS_TEST_ENABLED_ONLY_ON_LINUX(SaveConfigAs))
{
  common::Console::SetVerbosity(4);
  Application app(g_argc, g_argv);

  auto mainWindow = new MainWindow;
  ASSERT_NE(nullptr, mainWindow);

  // Save to file
  {
    // Trigger save
    mainWindow->OnSaveConfigAs(QString::fromStdString(kTestConfigFile));

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
    EXPECT_TRUE(savedStr.contains("<menus>"));
    EXPECT_TRUE(savedStr.contains("<drawer"));
    EXPECT_TRUE(savedStr.contains("<plugins"));

    // Delete file
    std::remove(kTestConfigFile.c_str());
  }

  mainWindow->deleteLater();
}

/////////////////////////////////////////////////
TEST(MainWindowTest, GZ_UTILS_TEST_ENABLED_ONLY_ON_LINUX(OnLoadConfig))
{
  common::Console::SetVerbosity(4);
  Application app(g_argc, g_argv);

  // Add test plugins to path
  App()->AddPluginPath(std::string(PROJECT_BINARY_PATH) + "/lib");

  // Get main window
  auto mainWindow = App()->findChild<MainWindow *>();
  ASSERT_NE(nullptr, mainWindow);

  // Check window doesn't have any plugins
  auto plugins = mainWindow->findChildren<Plugin *>();
  EXPECT_EQ(plugins.size(), 0);

  // Load file with single plugin
  {
    // Trigger load
    auto path = QString::fromStdString(
          std::string(PROJECT_SOURCE_PATH) + "/test/config/test.config");
    mainWindow->OnLoadConfig(path);

    // Check window has 1 plugin
    plugins = mainWindow->findChildren<Plugin *>();
    EXPECT_EQ(1, plugins.size());
  }

  // Load file with 2 plugins and window state
  {
    // Trigger load
    auto path = QString::fromStdString(
          std::string(PROJECT_SOURCE_PATH) + "/test/config/state.config");
    mainWindow->OnLoadConfig(path);

    // Check window has 2 plugins
    plugins = mainWindow->findChildren<Plugin *>();
    EXPECT_EQ(2, plugins.size());
  }
}

/////////////////////////////////////////////////
TEST(MainWindowTest, GZ_UTILS_TEST_ENABLED_ONLY_ON_LINUX(OnAddPlugin))
{
  common::Console::SetVerbosity(4);
  Application app(g_argc, g_argv);

  // Add test plugins to path
  App()->AddPluginPath(std::string(PROJECT_BINARY_PATH) + "/lib");

  // Get window
  auto mainWindow = App()->findChild<MainWindow *>();
  ASSERT_NE(nullptr, mainWindow);

  // Check window doesn't have any plugins
  auto plugins = mainWindow->findChildren<Plugin *>();
  EXPECT_EQ(plugins.size(), 0);

  // Add plugin
  mainWindow->OnAddPlugin("TestPlugin");

  // Check window has 1 plugin
  plugins = mainWindow->findChildren<Plugin *>();
  EXPECT_EQ(plugins.size(), 1);

  // Add another plugin
  mainWindow->OnAddPlugin("TestPlugin");

  // Check window has 2 plugins
  plugins = mainWindow->findChildren<Plugin *>();
  EXPECT_EQ(plugins.size(), 2);
}

/////////////////////////////////////////////////
TEST(WindowConfigTest, GZ_UTILS_TEST_DISABLED_ON_WIN32(defaultValues))
{
  common::Console::SetVerbosity(4);

  WindowConfig c;

  EXPECT_EQ(c.posX, -1);
  EXPECT_EQ(c.posY, -1);
  EXPECT_EQ(c.width, -1);
  EXPECT_EQ(c.height, -1);
  EXPECT_TRUE(c.state.isEmpty());
  EXPECT_TRUE(c.materialTheme.empty());
  EXPECT_TRUE(c.materialPrimary.empty());
  EXPECT_TRUE(c.materialAccent.empty());
  EXPECT_TRUE(c.showDrawer);
  EXPECT_TRUE(c.showDefaultDrawerOpts);
  EXPECT_TRUE(c.showPluginMenu);
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
  EXPECT_NE(xml.find("<drawer"), std::string::npos);
  EXPECT_NE(xml.find("<plugins"), std::string::npos);
  EXPECT_EQ(xml.find("<ignore>"), std::string::npos);
}

/////////////////////////////////////////////////
TEST(WindowConfigTest, GZ_UTILS_TEST_DISABLED_ON_WIN32(mergeFromXML))
{
  common::Console::SetVerbosity(4);

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
  EXPECT_TRUE(c.materialTheme.empty());
  EXPECT_TRUE(c.materialPrimary.empty());
  EXPECT_TRUE(c.materialAccent.empty());
  EXPECT_TRUE(c.showDrawer);
  EXPECT_TRUE(c.showDefaultDrawerOpts);
  EXPECT_TRUE(c.showPluginMenu);
  EXPECT_FALSE(c.pluginsFromPaths);
  EXPECT_TRUE(c.showPlugins.empty());
  EXPECT_EQ(c.ignoredProps.size(), 2u);
  EXPECT_TRUE(c.IsIgnoring("state"));
  EXPECT_TRUE(c.IsIgnoring("size"));
}

/////////////////////////////////////////////////
TEST(WindowConfigTest, GZ_UTILS_TEST_DISABLED_ON_WIN32(MenusToString))
{
  common::Console::SetVerbosity(4);

  WindowConfig c;

  // Set some menu-related properties
  c.showDrawer = false;
  c.pluginsFromPaths = false;

  c.showPlugins.push_back("PluginA");
  c.showPlugins.push_back("PluginB");

  // Check generated string
  auto str = c.XMLString();
  EXPECT_FALSE(str.empty());

  EXPECT_TRUE(str.find("<drawer visible=\"0\"") != std::string::npos ||
              str.find("<drawer visible=\"false\"") != std::string::npos);
  EXPECT_TRUE(str.find("<plugins visible=\"1\" from_paths=\"0\">") !=
      std::string::npos ||
      str.find("<plugins visible=\"true\" from_paths=\"false\">") !=
      std::string::npos);

  EXPECT_NE(str.find("<show>PluginA</show>"), std::string::npos) << str;
  EXPECT_NE(str.find("<show>PluginB</show>"), std::string::npos) << str;
  EXPECT_EQ(str.find("<show>PluginC</show>"), std::string::npos) << str;
}

/////////////////////////////////////////////////
TEST(WindowConfigTest, GZ_UTILS_TEST_DISABLED_ON_WIN32(IgnoreToString))
{
  common::Console::SetVerbosity(4);

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
TEST(MainWindowTest,
    GZ_UTILS_TEST_ENABLED_ONLY_ON_LINUX(CloseWithoutSavingChanges))
{
  common::Console::SetVerbosity(4);
  Application app(g_argc, g_argv);

  // Access window after it's open
  bool closed{false};
  QTimer::singleShot(300, App(), [&closed]
  {
    auto win = App()->findChild<MainWindow *>();
    ASSERT_NE(nullptr, win);
    EXPECT_TRUE(win->QuickWindow()->isVisible());

    for (unsigned int i = 0; i < 100; ++i)
    {
//      win->resize(10+i, 10+2*i);
//      QCoreApplication::processEvents();
//      std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    // Access dialog after it's open
//    bool dialogClosed{false};
//    QTimer::singleShot(300, [&]
//    {
//      auto fileDialogs = win->findChildren<QDialog *>();
//      ASSERT_EQ(fileDialogs.size(), 1);
//
//      auto closeButton = fileDialogs[0]->findChild<QPushButton *>(
//          "closeConfirmationDialogCloseButton");
//
//      closeButton->click();
//      dialogClosed = true;
//    });
//
    win->QuickWindow()->close();
//    EXPECT_TRUE(dialogClosed);
//
    closed = true;
  });

  // Show window
  App()->exec();

  EXPECT_TRUE(closed);
}

/////////////////////////////////////////////////
TEST(MainWindowTest,
     GZ_UTILS_TEST_ENABLED_ONLY_ON_LINUX(DefaultExitActionAutoShutdown))
{
  common::Console::SetVerbosity(4);
  Application app(g_argc, g_argv);

  app.LoadConfig(common::joinPaths(
    PROJECT_SOURCE_PATH, "test", "config",
    "close_dialog_auto_shutdown.config"));
  // Get main window
  auto mainWindow = App()->findChild<MainWindow *>();
  ASSERT_NE(nullptr, mainWindow);

  bool shutdownCalled{false};
  transport::Node node;
  std::string serverControlService{"/server_control"};
  std::function<bool(const msgs::ServerControl &, msgs::Boolean &)>
  cb = [&](const msgs::ServerControl &_req, msgs::Boolean &_rep) {
    shutdownCalled = _req.stop();
    _rep.set_data(true);
    return true;
  };
  node.Advertise(serverControlService, cb);

  EXPECT_TRUE(mainWindow->QuickWindow()->isVisible());
  mainWindow->QuickWindow()->close();
  EXPECT_FALSE(mainWindow->QuickWindow()->isVisible());

  EXPECT_TRUE(shutdownCalled);
}

/////////////////////////////////////////////////
TEST(MainWindowTest,
     GZ_UTILS_TEST_ENABLED_ONLY_ON_LINUX(ExitActionCustomShutdownService))
{
  common::Console::SetVerbosity(4);
  Application app(g_argc, g_argv);

  app.LoadConfig(common::joinPaths(
    PROJECT_SOURCE_PATH, "test", "config",
    "close_dialog_custom_shutdown_service.config"));
  // Get main window
  auto mainWindow = App()->findChild<MainWindow *>();
  ASSERT_NE(nullptr, mainWindow);

  bool shutdownCalled{false};
  bool wrongShutdownCalled{false};

  transport::Node node;

  std::string serverControlService{"/test_service"};
  std::function<bool(const msgs::ServerControl &, msgs::Boolean &)>
  cb = [&](const msgs::ServerControl &_req, msgs::Boolean &_rep) {
    shutdownCalled = _req.stop();
    _rep.set_data(true);
    return true;
  };
  node.Advertise(serverControlService, cb);

  std::string wrongServerControlService{"/server_control"};
  std::function<bool(const msgs::ServerControl &, msgs::Boolean &)>
  cb2 = [&](const msgs::ServerControl &, msgs::Boolean &_rep) {
    wrongShutdownCalled = true;
    _rep.set_data(true);
    return true;
  };
  node.Advertise(wrongServerControlService, cb2);

  EXPECT_TRUE(mainWindow->QuickWindow()->isVisible());
  mainWindow->QuickWindow()->close();
  EXPECT_FALSE(mainWindow->QuickWindow()->isVisible());

  EXPECT_TRUE(shutdownCalled);
  EXPECT_FALSE(wrongShutdownCalled);
}

/////////////////////////////////////////////////
TEST(MainWindowTest,
     GZ_UTILS_TEST_ENABLED_ONLY_ON_LINUX(DefaultExitActionAutoCloseGui))
{
  common::Console::SetVerbosity(4);
  Application app(g_argc, g_argv);

  // Add test plugins to path
  app.AddPluginPath(common::joinPaths(PROJECT_BINARY_PATH, "lib"));
  app.LoadConfig(common::joinPaths(
    PROJECT_SOURCE_PATH, "test", "config",
    "close_dialog_auto_gui_only.config"));
  // Get main window
  auto mainWindow = App()->findChild<MainWindow *>();
  ASSERT_NE(nullptr, mainWindow);

  bool shutdownCalled{false};
  transport::Node node;
  std::string serverControlService{"/server_control"};
  std::function<bool(const msgs::ServerControl &, msgs::Boolean &)>
  cb = [&](const msgs::ServerControl &, msgs::Boolean &_rep) {
    shutdownCalled = true;
    _rep.set_data(true);
    return true;
  };
  node.Advertise(serverControlService, cb);

  EXPECT_TRUE(mainWindow->QuickWindow()->isVisible());
  mainWindow->QuickWindow()->close();
  EXPECT_FALSE(mainWindow->QuickWindow()->isVisible());

  EXPECT_FALSE(shutdownCalled);
}

/////////////////////////////////////////////////
// Copied from private QPlatformDialogHelper::ButtonRole
enum ButtonRole {
  InvalidRole = -1,
  AcceptRole,
  RejectRole,
  DestructiveRole,
  ActionRole,
  HelpRole,
  YesRole,
  NoRole,
  ResetRole,
  ApplyRole,
  NRoles
};

/////////////////////////////////////////////////
void FindExitDialogButtons(
  MainWindow *_mainWindow,
  std::unordered_set<ButtonRole> &_roles,
  std::unordered_map<ButtonRole, QQuickItem *> &_buttonRoles)
{
  auto dialog = _mainWindow->QuickWindow()->findChild<QObject *>(
    "confirmationDialogOnExit");
  ASSERT_NE(nullptr, dialog);

  QObject *buttonBox{nullptr};
  // Qt considers range-based for loops over temporary objects
  // potentially dangerous, so explicitly create a container.
  const auto children = dialog->findChildren<QObject *>();
  for (const auto& c : children)
  {
    if (std::string(c->metaObject()->className()).find("ButtonBox") !=
        std::string::npos)
    {
      const auto& p = c->property("standardButtons");
      if (p.isValid() && p.toInt() != 0)
      {
        buttonBox = c;
        break;
      }
    }
  }
  ASSERT_NE(nullptr, buttonBox);

  const auto buttonCount = buttonBox->property("count").toInt();

  std::vector<QQuickItem *> buttons;
  for (int index = 0; index < buttonCount; ++index)
  {
    QQuickItem *button {nullptr};
    QMetaObject::invokeMethod(buttonBox, "itemAt", Qt::DirectConnection,
                              Q_RETURN_ARG(QQuickItem*, button),
                              Q_ARG(int, index));

    ASSERT_NE(std::string::npos,
        std::string(button->metaObject()->className()).find("Button"));
    buttons.push_back(button);
  }

  EXPECT_EQ(static_cast<size_t>(buttonCount), buttons.size());

  for (const auto& button : buttons)
  {
    QQmlProperty prop(button, "DialogButtonBox.buttonRole", qmlContext(button));
    const auto role = static_cast<ButtonRole>(prop.read().toInt());
    _roles.insert(role);
    _buttonRoles[role] = button;
  }
}

/////////////////////////////////////////////////
TEST(MainWindowTest,
     GZ_UTILS_TEST_ENABLED_ONLY_ON_LINUX(ExitDialogShutdownButton))
{
  common::Console::SetVerbosity(4);
  Application app(g_argc, g_argv);

  app.LoadConfig(common::joinPaths(
    PROJECT_SOURCE_PATH, "test", "config",
    "close_dialog_buttons.config"));
  // Get main window
  auto mainWindow = App()->findChild<MainWindow *>();
  ASSERT_NE(nullptr, mainWindow);

  // Trigger the closing behavior
  EXPECT_TRUE(mainWindow->QuickWindow()->isVisible());
  mainWindow->QuickWindow()->close();
  EXPECT_TRUE(mainWindow->QuickWindow()->isVisible());

  QCoreApplication::processEvents();

  std::unordered_set<ButtonRole> roles;
  std::unordered_map<ButtonRole, QQuickItem *> buttonRoles;
  FindExitDialogButtons(mainWindow, roles, buttonRoles);

  auto expectedRoles =
    std::unordered_set<ButtonRole>({
      ButtonRole::AcceptRole,
      ButtonRole::DestructiveRole,
      ButtonRole::RejectRole
    });
  ASSERT_EQ(expectedRoles, roles);

  bool shutdownCalled{false};
  transport::Node node;
  std::string serverControlService{"/server_control"};
  std::function<bool(const msgs::ServerControl &, msgs::Boolean &)>
    cb = [&](const msgs::ServerControl &, msgs::Boolean &_rep) {
    shutdownCalled = true;
    _rep.set_data(true);
    return true;
  };
  node.Advertise(serverControlService, cb);

  EXPECT_TRUE(mainWindow->QuickWindow()->isVisible());
  QMetaObject::invokeMethod(
    buttonRoles[ButtonRole::DestructiveRole], "clicked");

  // Wait until the window closes (it may take some time, but not > 1 second)
  int sleep = 0;
  for (; mainWindow->QuickWindow()->isVisible() && sleep < 10; ++sleep)
  {
    std::this_thread::sleep_for(100ms);
    QCoreApplication::processEvents();
  }

  EXPECT_TRUE(shutdownCalled);
  EXPECT_FALSE(mainWindow->QuickWindow()->isVisible());
}

/////////////////////////////////////////////////
TEST(MainWindowTest,
     GZ_UTILS_TEST_ENABLED_ONLY_ON_LINUX(ExitDialogDefaultButtons))
{
  common::Console::SetVerbosity(4);
  Application app(g_argc, g_argv);

  app.LoadConfig(common::joinPaths(
    PROJECT_SOURCE_PATH, "test", "config",
    "close_dialog_default_buttons.config"));
  // Get main window
  auto mainWindow = App()->findChild<MainWindow *>();
  ASSERT_NE(nullptr, mainWindow);

  // Trigger the closing behavior
  EXPECT_TRUE(mainWindow->QuickWindow()->isVisible());
  mainWindow->QuickWindow()->close();
  EXPECT_TRUE(mainWindow->QuickWindow()->isVisible());

  QCoreApplication::processEvents();

  std::unordered_set<ButtonRole> roles;
  std::unordered_map<ButtonRole, QQuickItem *> buttonRoles;
  FindExitDialogButtons(mainWindow, roles, buttonRoles);

  auto expectedRoles =
    std::unordered_set<ButtonRole>({
      ButtonRole::AcceptRole,
      ButtonRole::RejectRole
    });
  ASSERT_EQ(expectedRoles, roles);

  bool shutdownCalled{false};
  transport::Node node;
  std::string serverControlService{"/server_control"};
  std::function<bool(const msgs::ServerControl &, msgs::Boolean &)>
    cb = [&](const msgs::ServerControl &, msgs::Boolean &_rep) {
    shutdownCalled = true;
    _rep.set_data(true);
    return true;
  };
  node.Advertise(serverControlService, cb);

  EXPECT_TRUE(mainWindow->QuickWindow()->isVisible());
  QMetaObject::invokeMethod(buttonRoles[ButtonRole::AcceptRole], "clicked");
  EXPECT_FALSE(mainWindow->QuickWindow()->isVisible());
}

/////////////////////////////////////////////////
TEST(MainWindowTest,
     GZ_UTILS_TEST_ENABLED_ONLY_ON_LINUX(ExitDialogButtonsText))
{
  common::Console::SetVerbosity(4);
  Application app(g_argc, g_argv);

  app.LoadConfig(common::joinPaths(
    PROJECT_SOURCE_PATH, "test", "config",
    "close_dialog_buttons_text.config"));
  // Get main window
  auto mainWindow = App()->findChild<MainWindow *>();
  ASSERT_NE(nullptr, mainWindow);

  // Trigger the closing behavior
  EXPECT_TRUE(mainWindow->QuickWindow()->isVisible());
  mainWindow->QuickWindow()->close();
  EXPECT_TRUE(mainWindow->QuickWindow()->isVisible());

  QCoreApplication::processEvents();

  std::unordered_set<ButtonRole> roles;
  std::unordered_map<ButtonRole, QQuickItem *> buttonRoles;
  FindExitDialogButtons(mainWindow, roles, buttonRoles);

  auto expectedRoles =
    std::unordered_set<ButtonRole>({
      ButtonRole::AcceptRole,
      ButtonRole::DestructiveRole,
      ButtonRole::RejectRole
    });
  ASSERT_EQ(expectedRoles, roles);

  auto closeGui = buttonRoles[ButtonRole::AcceptRole];
  EXPECT_EQ("close_gui",
    closeGui->property("text").toString().toStdString());

  auto shutdown = buttonRoles[ButtonRole::DestructiveRole];
  EXPECT_EQ("shutdown",
    shutdown->property("text").toString().toStdString());
}

/////////////////////////////////////////////////
TEST(MainWindowTest, GZ_UTILS_TEST_ENABLED_ONLY_ON_LINUX(ApplyConfig))
{
  common::Console::SetVerbosity(4);
  Application app(g_argc, g_argv);

  // Main window
  auto mainWindow = new MainWindow;
  ASSERT_NE(nullptr, mainWindow);

  app.processEvents(QEventLoop::ExcludeUserInputEvents);

  // Default config
  {
    auto c = mainWindow->CurrentWindowConfig();
    EXPECT_TRUE(c.showDrawer);
    EXPECT_TRUE(c.showDefaultDrawerOpts);
    EXPECT_TRUE(c.showPluginMenu);
    EXPECT_TRUE(c.pluginsFromPaths);
    EXPECT_TRUE(c.showPlugins.empty());
    EXPECT_TRUE(c.ignoredProps.empty());

  }

  // Apply a config
  {
    WindowConfig c;
//    c.posX = 1000;
//    c.posY = 2000;
    c.width = 100;
    c.height = 200;
    c.materialTheme = "Dark";
    c.materialPrimary = "#ff0000";
    c.materialAccent = "Indigo";
    c.showDrawer = false;
    c.pluginsFromPaths = false;
//    c.showPlugins.push_back("watermelon");
//    c.ignoredProps.insert("position");

    EXPECT_TRUE(mainWindow->ApplyConfig(c));
  }

  app.processEvents(QEventLoop::ExcludeUserInputEvents);

  // Check applied config
  {
    auto c = mainWindow->CurrentWindowConfig();

    // ignored
//    EXPECT_NE(c.posX, 1000);
//    EXPECT_NE(c.posY, 2000);

    EXPECT_EQ(c.width, 100);
    EXPECT_EQ(c.height, 200);
    EXPECT_EQ(c.materialTheme, "Dark");
    EXPECT_EQ(c.materialPrimary, "#ff0000");
    // Always save hex
    EXPECT_EQ(c.materialAccent, "#9fa8da");
    EXPECT_FALSE(c.showDrawer);
    EXPECT_FALSE(c.pluginsFromPaths);
//    EXPECT_EQ(c.showPlugins.size(), 1u);
//    EXPECT_EQ(c.ignoredProps.size(), 1u);
  }

  mainWindow->deleteLater();
}
