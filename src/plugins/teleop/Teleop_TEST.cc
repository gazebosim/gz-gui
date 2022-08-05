/*
 * Copyright (C) 2021 Open Source Robotics Foundation
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
#ifdef _MSC_VER
#pragma warning(push, 0)
#endif
#include <gz/msgs/stringmsg.pb.h>
#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include <gz/utils/ExtraTestMacros.hh>

#include "test_config.hh"  // NOLINT(build/include)
#include "gz/gui/Application.hh"
#include "gz/gui/Plugin.hh"
#include "gz/gui/MainWindow.hh"
#include "gz/gui/qt.h"

#include "Teleop.hh"

int g_argc = 1;
char* g_argv[] =
{
  reinterpret_cast<char*>(const_cast<char*>("./Teleop_TEST")),
};

using namespace gz;
using namespace gui;

class TeleopTest : public ::testing::Test
{
  // Set up function.
  protected: void SetUp() override
    {
      common::Console::SetVerbosity(4);

      this->app.AddPluginPath(std::string(PROJECT_BINARY_PATH) + "/lib");

      // Load plugin
      const std::string kTopic{"/test/cmd_vel"};
      std::string pluginStr =
        "<plugin filename=\"Teleop\">"
          "<gz-gui>"
            "<title>Teleop!</title>"
          "</gz-gui>"
          "<topic>" + kTopic + "</topic>"
        "</plugin>";

      tinyxml2::XMLDocument pluginDoc;
      EXPECT_EQ(tinyxml2::XML_SUCCESS, pluginDoc.Parse(pluginStr.c_str()));
      EXPECT_TRUE(this->app.LoadPlugin("Teleop",
          pluginDoc.FirstChildElement("plugin")));

      // Get main window
      win = this->app.findChild<MainWindow *>();
      ASSERT_NE(nullptr, win);

      // Show, but don't exec, so we don't block
      win->QuickWindow()->show();

      // Get plugin
      plugins = win->findChildren<plugins::Teleop *>();
      EXPECT_EQ(plugins.size(), 1);

      plugin = plugins[0];
      EXPECT_EQ(plugin->Title(), "Teleop!");

      // Subscribes to the command velocity topic.
      node.Subscribe(kTopic, &TeleopTest::VerifyTwistMsgCb, this);

      // Sets topic. This must be the same as the
      // one the node is subscribed to.
      plugin->SetTopic(QString::fromStdString(kTopic));

      // Set velocity value and movement direction.
      plugin->SetMaxForwardVel(this->kMaxForwardVel);
      plugin->SetMaxVerticalVel(this->kMaxVerticalVel);
      plugin->SetMaxYawVel(this->kMaxYawVel);
    }

  // Set up function.
  protected: void TearDown() override
    {
      // Cleanup
      plugins.clear();
    }

  // Subscriber call back function. Verifies if the Twist message is
  // sent correctly.
  protected: void VerifyTwistMsgCb(const msgs::Twist &_msg)
    {
      EXPECT_DOUBLE_EQ(_msg.linear().x(), this->forwardVel);
      EXPECT_DOUBLE_EQ(_msg.linear().z(), this->verticalVel);
      EXPECT_DOUBLE_EQ(_msg.angular().z(), this->yawVel);
      received = true;
    }

  // Subscriber call back function. Verifies if the Twist message is
  // sent correctly.
  protected: void KeyEvent(bool _press, int _key)
    {
      this->received = false;

      auto type = _press ? QKeyEvent::KeyPress : QKeyEvent::KeyRelease;
      auto event = new QKeyEvent(type, _key, Qt::NoModifier);
      app.sendEvent(win->QuickWindow(), event);

      int sleep = 0;
      const int maxSleep = 30;
      while (!this->received && sleep < maxSleep)
      {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        QCoreApplication::processEvents();
        sleep++;
      }

      EXPECT_LT(sleep, maxSleep);
      EXPECT_TRUE(this->received);
    }

  // Provides an API to load plugins and configuration files.
  protected: Application app{g_argc, g_argv};

  // List of plugins.
  protected: QList<plugins::Teleop *> plugins;
  protected: plugins::Teleop * plugin;

  // Instance of the main window.
  protected: MainWindow * win;

  // Checks if a new command has been received.
  protected: bool received = false;
  protected: transport::Node node;

  // Maximum velocities
  protected: const double kMaxForwardVel = 1.0;
  protected: const double kMaxVerticalVel = 1.0;
  protected: const double kMaxYawVel = 0.5;

  // Current vel
  protected: double forwardVel = 0.0;
  protected: double verticalVel = 0.0;
  protected: double yawVel = 0.0;
};

/////////////////////////////////////////////////
TEST_F(TeleopTest, GZ_UTILS_TEST_DISABLED_ON_WIN32(ButtonCommand))
{
  this->forwardVel = 0.1;
  this->verticalVel = 0.2;
  this->yawVel = 0.3;
  plugin->OnTeleopTwist(this->forwardVel, this->verticalVel, this->yawVel);

  int sleep = 0;
  const int maxSleep = 30;
  while (!received && sleep < maxSleep)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    QCoreApplication::processEvents();
    sleep++;
  }

  EXPECT_TRUE(received);
  received = false;
}

/////////////////////////////////////////////////
TEST_F(TeleopTest, GZ_UTILS_TEST_DISABLED_ON_WIN32(KeyboardCommand))
{
  QKeyEvent *keypress_W = new QKeyEvent(QKeyEvent::KeyPress,
    Qt::Key_W, Qt::NoModifier);
  app.sendEvent(win->QuickWindow(), keypress_W);
  QCoreApplication::processEvents();

  EXPECT_FALSE(received);

  // Enables key input.
  plugin->OnKeySwitch(true);

  gzdbg << "Press W" << std::endl;
  this->forwardVel = this->kMaxForwardVel;
  this->KeyEvent(true, Qt::Key_W);

  gzdbg << "Press D" << std::endl;
  this->yawVel = -this->kMaxYawVel;
  this->KeyEvent(true, Qt::Key_D);

  gzdbg << "Release D" << std::endl;
  this->yawVel = 0.0;
  this->KeyEvent(false, Qt::Key_D);

  gzdbg << "Press A" << std::endl;
  this->yawVel = this->kMaxYawVel;
  this->KeyEvent(true, Qt::Key_A);

  gzdbg << "Release A" << std::endl;
  this->yawVel = 0.0;
  this->KeyEvent(false, Qt::Key_A);

  gzdbg << "Release W" << std::endl;
  this->forwardVel = 0.0;
  this->KeyEvent(false, Qt::Key_W);

  gzdbg << "Press S" << std::endl;
  this->forwardVel = -this->kMaxForwardVel;
  this->KeyEvent(true, Qt::Key_S);

  gzdbg << "Release S" << std::endl;
  this->forwardVel = 0.0;
  this->KeyEvent(false, Qt::Key_S);

  gzdbg << "Press X" << std::endl;
  this->KeyEvent(true, Qt::Key_X);

  gzdbg << "Release X" << std::endl;
  this->KeyEvent(false, Qt::Key_X);

  gzdbg << "Press Q" << std::endl;
  this->verticalVel = this->kMaxVerticalVel;
  this->KeyEvent(true, Qt::Key_Q);

  gzdbg << "Press E" << std::endl;
  this->verticalVel = -this->kMaxVerticalVel;
  this->KeyEvent(true, Qt::Key_E);

  gzdbg << "Release E" << std::endl;
  this->verticalVel = 0.0;
  this->KeyEvent(false, Qt::Key_E);
}
