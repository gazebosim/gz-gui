/*
 * Copyright (C) 2020 Open Source Robotics Foundation
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
#include <ignition/msgs.hh>
#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include <ignition/transport.hh>
#include <ignition/common/Console.hh>
#include <ignition/utilities/ExtraTestMacros.hh>
#include "test_config.h"  // NOLINT(build/include)
#include "ignition/gui/Enums.hh"
#include "ignition/gui/PlottingInterface.hh"

using namespace ignition;
using namespace gui;

//////////////////////////////////////////////////
// Disable test on windows until we fix "LNK2001 unresolved external symbol"
// error
TEST(PlottingInterfaceTest, IGN_UTILS_TEST_DISABLED_ON_WIN32(Topic))
{
  common::Console::SetVerbosity(4);

  // ============== Register & UnRegister Test =============

  // prepare the msg
  msgs::Collision msg;
  auto pose = new ignition::msgs::Pose();
  auto vector3d = new ignition::msgs::Vector3d();
  vector3d->set_x(10);
  vector3d->set_z(15);
  pose->set_allocated_position(vector3d);
  msg.set_allocated_pose(pose);

  auto topic = Topic("");

  topic.Register("pose-position-x", 1);
  topic.Register("pose-position-x", 2);
  topic.Register("pose-position-y", 1);
  topic.Register("pose-position-y", 2);
  topic.UnRegister("pose-position-y", 2);

  auto fields = topic.Fields();
  ASSERT_EQ(topic.FieldCount(), 2);

  // size test
  EXPECT_EQ(fields["pose-position-x"]->ChartCount(), 2);
  EXPECT_EQ(fields["pose-position-y"]->ChartCount(), 1);

  // charts test
  EXPECT_TRUE(fields["pose-position-x"]->Charts().find(1) !=
          fields["pose-position-x"]->Charts().end());
  EXPECT_TRUE(fields["pose-position-x"]->Charts().find(2) !=
          fields["pose-position-x"]->Charts().end());
  EXPECT_TRUE(fields["pose-position-y"]->Charts().find(1) !=
          fields["pose-position-x"]->Charts().end());

  // test the removing of the field if it has not attatched charts
  topic.UnRegister("pose-position-y", 1);
  EXPECT_EQ(topic.FieldCount(), 1);


  // =========== Callback Test ============
  topic.Register("pose-position-z", 1);

  // update the fields
  topic.Callback(msg);

  fields = topic.Fields();

  EXPECT_EQ(static_cast<int>(fields["pose-position-x"]->Value()), 10);
  EXPECT_EQ(static_cast<int>(fields["pose-position-z"]->Value()), 15);
}

//////////////////////////////////////////////////
// Disable test on windows until we fix "LNK2001 unresolved external symbol"
// error
TEST(PlottingInterfaceTest, IGN_UTILS_TEST_DISABLED_ON_WIN32(Transport))
{
  // =========== Publish Test =================
  transport::Node node;

  auto pub = node.Advertise<msgs::Collision> ("/collision_topic");
  std::this_thread::sleep_for(std::chrono::milliseconds(200));

  auto transport = Transport();
  transport.Subscribe("/collision_topic", "pose-position-x", 1);
  transport.Subscribe("/collision_topic", "pose-position-z", 1);

  // prepare the msg
  msgs::Collision msg;
  auto pose = new ignition::msgs::Pose();
  auto vector3d = new ignition::msgs::Vector3d();
  vector3d->set_x(10);
  vector3d->set_z(15);
  pose->set_allocated_position(vector3d);
  msg.set_allocated_pose(pose);

  bool received = false;
  std::function<void(const msgs::Collision &)> cb =
      [&](const msgs::Collision &_msg)
  {
      EXPECT_NEAR(_msg.pose().position().x(), 10, 1e-6);
      received = true;
  };

  node.Subscribe("collision_topic", cb);

  // publish to call the topic::Callback
  pub.Publish(msg);

  int sleep = 0;
  int maxSleep = 30;
  while (!received && sleep < maxSleep)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    QCoreApplication::processEvents();
    sleep++;
  }

  EXPECT_TRUE(received);

  auto topics = transport.Topics();

  EXPECT_EQ(topics["/collision_topic"]->FieldCount(), 2);

  auto fields = topics["/collision_topic"]->Fields();

  EXPECT_EQ(static_cast<int>(fields["pose-position-x"]->Value()), 10);
  EXPECT_EQ(static_cast<int>(fields["pose-position-z"]->Value()), 15);


  // =========== Many Topics Test =================
  // add another topic to the transport and subscribe to it
  node.Advertise<msgs::Int32> ("/test_topic");
  transport.Subscribe("/test_topic", "data", 2);

  topics = transport.Topics();

  ASSERT_EQ(static_cast<int>(topics.size()), 2);
  EXPECT_EQ(topics["/test_topic"]->FieldCount(), 1);


  // =========== UnSubscribe Test =================

  // test the deletion of the topic if it has no fields
  transport.Unsubscribe("/collision_topic", "pose-position-z", 1);
  transport.Unsubscribe("/collision_topic", "pose-position-x", 1);

  topics = transport.Topics();
  EXPECT_EQ(static_cast<int>(topics.size()), 1);
}
