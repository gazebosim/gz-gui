/*
 * Copyright (C) 2019 Open Source Robotics Foundation
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

#include <ignition/transport.hh>
#include <ignition/math.hh>
#include <ignition/msgs.hh>

/////////////////////////////////////////////////
int main(int _argc, char **_argv)
{
  ignition::transport::Node node;

  // Create the marker message
  ignition::msgs::Marker markerMsg;
  ignition::msgs::Material matMsg;
  markerMsg.set_ns("default");
  markerMsg.set_id(0);
  markerMsg.set_action(ignition::msgs::Marker::ADD_MODIFY);
  markerMsg.set_type(ignition::msgs::Marker::SPHERE);
  markerMsg.set_visibility(ignition::msgs::Marker::GUI);

  // Set color to Blue
  markerMsg.mutable_material()->mutable_ambient()->set_r(0);
  markerMsg.mutable_material()->mutable_ambient()->set_g(0);
  markerMsg.mutable_material()->mutable_ambient()->set_b(1);
  markerMsg.mutable_material()->mutable_ambient()->set_a(1);
  markerMsg.mutable_material()->mutable_diffuse()->set_r(0);
  markerMsg.mutable_material()->mutable_diffuse()->set_g(0);
  markerMsg.mutable_material()->mutable_diffuse()->set_b(1);
  markerMsg.mutable_material()->mutable_diffuse()->set_a(1);
  markerMsg.mutable_lifetime()->set_sec(2);
  markerMsg.mutable_lifetime()->set_nsec(0);
  ignition::msgs::Set(markerMsg.mutable_scale(),
                    ignition::math::Vector3d(1.0, 1.0, 1.0));

  ignition::msgs::Set(markerMsg.mutable_pose(),
                      ignition::math::Pose3d(2, 2, 0, 0, 0, 0));

  for (int i = 0; i < 1000000; i++)
  {
    node.Request("/marker", markerMsg);
  }
}