/*
 * Copyright (C) 2022 Open Source Robotics Foundation
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

#include <gz/msgs/int32.pb.h>

#include <gz/common/Console.hh>
#include <gz/common/Filesystem.hh>
#include <gz/utils/ExtraTestMacros.hh>

#include "gz/gui/Application.hh"
#include "gz/gui/MainWindow.hh"
#include "gz/gui/Plugin.hh"
#include "gz/gui/qt.h"
#include "test_config.hh"  // NOLINT(build/include)

#include "KeyPublisher.hh"

using KeyPublisher = gz::gui::plugins::KeyPublisher;
using MainWindow = gz::gui::MainWindow;

int g_argc = 1;
char* g_argv[] =
{
  reinterpret_cast<char*>(const_cast<char*>("./KeyPublisher_TEST")),
};

class KeyPublisherTest : public ::testing::Test
{
  // Set up function.
  protected: void SetUp() override
    {
      gz::common::Console::SetVerbosity(4);

      this->app.AddPluginPath(
        gz::common::joinPaths(std::string(PROJECT_BINARY_PATH), "lib"));

      // Load plugin
      EXPECT_TRUE(this->app.LoadPlugin("KeyPublisher"));

      // Get main window
      this->win = this->app.findChild<MainWindow *>();
      ASSERT_NE(win, nullptr);

      // Get plugin
      this->plugins = win->findChildren<KeyPublisher *>();
      ASSERT_EQ(plugins.size(), 1);
      this->plugin = plugins[0];
      EXPECT_EQ(this->plugin->Title(), "Key publisher");

      // Subscribes to keyboard/keypress topic
      const std::string kTopic{"keyboard/keypress"};
      node.Subscribe(kTopic, &KeyPublisherTest::VerifyKeypressCb, this);
    }

  // Tear down function
  protected: void TearDown() override
    {
      // Cleanup
      plugins.clear();
    }

  // Callback function to verify key message was sent correctly
  protected: void VerifyKeypressCb(const gz::msgs::Int32 &_msg)
    {
      this->received = true;
      EXPECT_EQ(_msg.data(), this->currentKey);
    }

  protected: void VerifyKeyEvent(int _key)
    {
      this->received = false;
      this->currentKey = _key;
      auto event = new QKeyEvent(QKeyEvent::KeyPress, _key, Qt::NoModifier);
      this->app.sendEvent(this->win->QuickWindow(), event);

      int sleep = 0;
      int maxSleep = 30;
      while (!this->received && sleep < maxSleep)
      {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        QCoreApplication::processEvents();
        ++sleep;
      }

      EXPECT_LT(sleep, maxSleep);
      EXPECT_TRUE(this->received);
    }

  // Provides an API to load plugins and configuration files.
  protected: gz::gui::Application app{g_argc, g_argv};

  // Instance of the main window.
  protected: gz::gui::MainWindow *win;

  // List of plugins.
  protected: QList<KeyPublisher *> plugins;
  protected: gz::gui::plugins::KeyPublisher *plugin;

  // Checks if a new key has been received.
  protected: bool received = false;
  protected: gz::transport::Node node;

  // Current key
  protected: int currentKey = 0;
};

/////////////////////////////////////////////////
TEST_F(KeyPublisherTest, GZ_UTILS_TEST_DISABLED_ON_WIN32(KeyPublisher))
{
  this->VerifyKeyEvent(Qt::Key_W);
  this->VerifyKeyEvent(Qt::Key_A);
  this->VerifyKeyEvent(Qt::Key_D);
}
