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
#include <ignition/msgs.hh>
#include <ignition/transport/Node.hh>

#include "ignition/gui/DragDropModel.hh"
#include "ignition/gui/Iface.hh"
#include "ignition/gui/Plugin.hh"
#include "ignition/gui/MainWindow.hh"

using namespace std::chrono_literals;
using namespace ignition;
using namespace gui;

/////////////////////////////////////////////////
TEST(TopicViewerTest, Load)
{
  EXPECT_TRUE(initApp());

  EXPECT_TRUE(loadPlugin("TopicViewer"));

  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(TopicViewerTest, OnMessage)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // Load plugin.
  EXPECT_TRUE(ignition::gui::loadPlugin("TopicViewer"));

  // Create main window.
  EXPECT_TRUE(createMainWindow());
  auto win = mainWindow();
  EXPECT_TRUE(win != nullptr);
  win->show();

  // Get plugin.
  auto plugins = win->findChildren<Plugin *>();
  EXPECT_EQ(1, plugins.size());
  auto plugin = plugins[0];
  EXPECT_EQ("Topic viewer", plugin->Title());

  // Check topicsModel was created.
  auto topicsModel = plugin->findChildren<DragDropModel *>();
  ASSERT_EQ(1, topicsModel.size());

  // Check that there are no topics displayed.
  EXPECT_EQ(0, topicsModel[0]->rowCount());

  // Publish a message.
  transport::Node node;
  auto pub = node.Advertise<msgs::StringMsg>("/test_topic_str");
  msgs::StringMsg msg;
  msg.set_data("test_content");
  pub.Publish(msg);

  // Check that eventually we show the topic name on the widget.
  unsigned int counter = 0u;
  bool found = false;
  while (counter < 20u && !found)
  {
    // We need this line to trigger the timeout event connected to the QT slot
    // function that will update the topic list.
    QCoreApplication::processEvents();
    found = topicsModel[0]->rowCount() > 0;
    std::this_thread::sleep_for(100ms);
    ++counter;
  }

  EXPECT_TRUE(found);

  // Cleanup.
  EXPECT_TRUE(stop());
}
