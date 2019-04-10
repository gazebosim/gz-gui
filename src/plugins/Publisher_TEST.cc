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
#include <ignition/msgs.hh>
#include <ignition/transport/Node.hh>

#include "ignition/gui/Iface.hh"
#include "ignition/gui/Plugin.hh"
#include "ignition/gui/MainWindow.hh"

using namespace ignition;
using namespace gui;

/////////////////////////////////////////////////
TEST(PublisherTest, Load)
{
  EXPECT_TRUE(initApp());

  EXPECT_TRUE(loadPlugin("Publisher"));

  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(PublisherTest, Publish)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // Load plugin
  EXPECT_TRUE(loadPlugin("Publisher"));

  // Create main window
  EXPECT_TRUE(createMainWindow());
  auto win = mainWindow();
  EXPECT_TRUE(win != nullptr);

  // Get plugin
  auto plugins = win->findChildren<Plugin *>();
  EXPECT_EQ(plugins.size(), 1);
  auto plugin = plugins[0];
  EXPECT_EQ(plugin->Title(), "Publisher");

  // Message type
  auto msgTypeEdit = plugin->findChild<QLineEdit *>("msgTypeEdit");
  ASSERT_TRUE(msgTypeEdit != nullptr);
  EXPECT_EQ(msgTypeEdit->text(), "ignition.msgs.StringMsg");

  // Message
  auto msgEdit = plugin->findChild<QTextEdit *>("msgEdit");
  ASSERT_TRUE(msgEdit != nullptr);
  EXPECT_EQ(msgEdit->toPlainText(), "data: \"Hello\"");

  // Topic
  auto topicEdit = plugin->findChild<QLineEdit *>("topicEdit");
  ASSERT_TRUE(topicEdit != nullptr);
  EXPECT_EQ(topicEdit->text(), "/echo");

  // Frequency
  auto freqSpin = plugin->findChild<QDoubleSpinBox *>("frequencySpinBox");
  ASSERT_TRUE(freqSpin != nullptr);
  EXPECT_DOUBLE_EQ(freqSpin->value(), 1.0);

  // Button
  auto pubButton = plugin->findChild<QPushButton *>("publishButton");
  ASSERT_TRUE(pubButton != nullptr);
  EXPECT_EQ(pubButton->text(), "Publish");

  // Subscribe
  bool received = false;
  std::function<void(const msgs::StringMsg &)> cb =
      [&](const msgs::StringMsg &_msg)
  {
    EXPECT_EQ(_msg.data(), "Hello");
    received = true;
  };
  transport::Node node;
  node.Subscribe("/echo", cb);

  EXPECT_FALSE(received);

  // Publish
  pubButton->click();

  int sleep = 0;
  int maxSleep = 30;
  while (!received && sleep < maxSleep)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    QCoreApplication::processEvents();
    sleep++;
  }

  EXPECT_EQ(pubButton->text(), "Stop publishing");
  EXPECT_TRUE(received);
  received = false;

  // Stop publishing
  pubButton->click();

  sleep = 0;
  // cppcheck-suppress knownConditionTrueFalse
  while (!received && sleep < maxSleep)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    QCoreApplication::processEvents();
    sleep++;
  }

  EXPECT_EQ(pubButton->text(), "Publish");
  EXPECT_FALSE(received);

  // Publish once
  freqSpin->setValue(0.0);
  pubButton->click();

  sleep = 0;
  while (!received && sleep < maxSleep)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    QCoreApplication::processEvents();
    sleep++;
  }

  EXPECT_EQ(pubButton->text(), "Publish");
  EXPECT_TRUE(received);
  received = false;

  // Bad message type
  freqSpin->setValue(1.0);
  msgTypeEdit->setText("banana.message");
  pubButton->click();

  sleep = 0;
  // cppcheck-suppress knownConditionTrueFalse
  while (!received && sleep < maxSleep)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    QCoreApplication::processEvents();
    sleep++;
  }
  EXPECT_EQ(pubButton->text(), "Publish");
  EXPECT_FALSE(received);

  // Bad message type - msg combination
  msgTypeEdit->setText("ignition.msgs.StringMsg");
  msgEdit->setPlainText("banana: apple");
  pubButton->click();

  sleep = 0;
  while (!received && sleep < maxSleep)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    QCoreApplication::processEvents();
    sleep++;
  }
  EXPECT_EQ(pubButton->text(), "Publish");
  EXPECT_FALSE(received);

  // Cleanup
  plugins.clear();
  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(PublisherTest, ParamsFromSDF)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // Load plugin
  const char *pluginStr =
    "<plugin filename=\"Publisher\">"
      "<topic>/fruit</topic>"
      "<message>number: 1 fruit {name:\"banana\"}</message>"
      "<message_type>ignition.msgs.Fruits</message_type>"
      "<frequency>0.1</frequency>"
    "</plugin>";

  tinyxml2::XMLDocument pluginDoc;
  pluginDoc.Parse(pluginStr);
  EXPECT_TRUE(ignition::gui::loadPlugin("Publisher",
      pluginDoc.FirstChildElement("plugin")));

  // Create main window
  EXPECT_TRUE(createMainWindow());
  auto win = mainWindow();
  ASSERT_NE(nullptr, win);

  // Get plugin
  auto plugins = win->findChildren<Plugin *>();
  EXPECT_EQ(plugins.size(), 1);
  auto plugin = plugins[0];
  EXPECT_EQ(plugin->Title(), "Publisher");

  // Message type
  auto msgTypeEdit = plugin->findChild<QLineEdit *>("msgTypeEdit");
  ASSERT_NE(nullptr, msgTypeEdit);
  EXPECT_EQ(msgTypeEdit->text(), "ignition.msgs.Fruits");

  // Message
  auto msgEdit = plugin->findChild<QTextEdit *>("msgEdit");
  ASSERT_NE(nullptr, msgEdit);
  EXPECT_EQ(msgEdit->toPlainText(), "number: 1 fruit {name:\"banana\"}");

  // Topic
  auto topicEdit = plugin->findChild<QLineEdit *>("topicEdit");
  ASSERT_NE(nullptr, topicEdit);
  EXPECT_EQ(topicEdit->text(), "/fruit");

  // Frequency
  auto freqSpin = plugin->findChild<QDoubleSpinBox *>("frequencySpinBox");
  ASSERT_NE(nullptr, freqSpin);
  EXPECT_DOUBLE_EQ(freqSpin->value(), 0.1);
}
