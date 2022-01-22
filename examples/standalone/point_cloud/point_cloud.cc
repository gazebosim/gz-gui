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

#include <ignition/msgs/float_v.pb.h>
#include <ignition/msgs/pointcloud_packed.pb.h>

#include <ignition/transport/Node.hh>

#include <chrono>
#include <csignal>
#include <iostream>
#include <thread>

using namespace std::chrono_literals;

static std::atomic<bool> g_terminatePub(false);

/////////////////////////////////////////////////
int main(int _argc, char **_argv)
{
  // Install a signal handler for SIGINT and SIGTERM.
  auto signal_handler = [](int _signal) -> void
  {
    if (_signal == SIGINT || _signal == SIGTERM)
      g_terminatePub = true;
  };
  std::signal(SIGINT,  signal_handler);
  std::signal(SIGTERM, signal_handler);

  // Create messages
  ignition::msgs::PointCloudPacked pcMsg;
  ignition::msgs::InitPointCloudPacked(pcMsg, "some_frame", true,
      {{"xyz", ignition::msgs::PointCloudPacked::Field::FLOAT32}});

  int numberOfPoints{1000};
  unsigned int dataSize{numberOfPoints * pcMsg.point_step()};
  pcMsg.mutable_data()->resize(dataSize);
  pcMsg.set_height(1);
  pcMsg.set_width(1000);

  ignition::msgs::Float_V flatMsg;
  ignition::msgs::Float_V sumMsg;
  ignition::msgs::Float_V productMsg;

  // Populate messages
  ignition::msgs::PointCloudPackedIterator<float> xIter(pcMsg, "x");
  ignition::msgs::PointCloudPackedIterator<float> yIter(pcMsg, "y");
  ignition::msgs::PointCloudPackedIterator<float> zIter(pcMsg, "z");

  for (float x = 0.0, y = 0.0, z = 0.0;
       xIter != xIter.End();
       ++xIter, ++yIter, ++zIter)
  {
    *xIter = x;
    *yIter = y;
    *zIter = z;
    flatMsg.add_data(1);
    sumMsg.add_data(x + y + z);
    productMsg.add_data(x * y * z);

    x += 1.0;
    if (x > 9)
    {
      x = 0.0;
      y += 1.0;
    }
    if (y > 9)
    {
      y = 0.0;
      z += 1.0;
    }
  }

  // Publish messages at 1Hz until interrupted.
  ignition::transport::Node node;
  auto flatPub = node.Advertise<ignition::msgs::Float_V>("/flat");
  auto sumPub = node.Advertise<ignition::msgs::Float_V>("/sum");
  auto productPub = node.Advertise<ignition::msgs::Float_V>("/product");
  auto pcPub = node.Advertise<ignition::msgs::PointCloudPacked>("/point_cloud");

  while (!g_terminatePub)
  {
    std::cout << "Publishing" << std::endl;
    flatPub.Publish(flatMsg);
    sumPub.Publish(sumMsg);
    productPub.Publish(productMsg);
    pcPub.Publish(pcMsg);
    std::this_thread::sleep_for(1s);
  }
}
