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
#ifndef IGNITION_GUI_QTMETATYPES_HH_
#define IGNITION_GUI_QTMETATYPES_HH_

#include <QMetaType>
#include <ignition/msgs/geometry.pb.h>

#include <string>

#include <ignition/math/Color.hh>
#include <ignition/math/Pose3.hh>
#include <ignition/math/Vector3.hh>

// Make it possible to use non-Qt types in QVariant
Q_DECLARE_METATYPE(std::string)
Q_DECLARE_METATYPE(ignition::math::Color)
Q_DECLARE_METATYPE(ignition::math::Pose3d)
Q_DECLARE_METATYPE(ignition::math::Vector3d)

Q_DECLARE_METATYPE(ignition::msgs::Geometry)

#endif
