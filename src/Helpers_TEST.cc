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

#include <stdlib.h>
#include <gtest/gtest.h>

#include <ignition/math/Helpers.hh>

#include "test_config.h"  // NOLINT(build/include)
#include "ignition/gui/Helpers.hh"
#include "ignition/gui/Iface.hh"

using namespace ignition;
using namespace gui;

/////////////////////////////////////////////////
TEST(HelpersTest, humanReadable)
{
  EXPECT_EQ(humanReadable("mass"), "Mass");
  EXPECT_EQ(humanReadable("cast_shadows"), "Cast shadows");
}

/////////////////////////////////////////////////
TEST(HelpersTest, unitFromKey)
{
  EXPECT_EQ(unitFromKey("pos"), "m");
  EXPECT_EQ(unitFromKey("length"), "m");
  EXPECT_EQ(unitFromKey("min_depth"), "m");

  EXPECT_EQ(unitFromKey("rot"), "rad");

  EXPECT_EQ(unitFromKey("kp"), "N/m");
  EXPECT_EQ(unitFromKey("kd"), "N/m");

  EXPECT_EQ(unitFromKey("max_vel"), "m/s");

  EXPECT_EQ(unitFromKey("mass"), "kg");

  EXPECT_EQ(unitFromKey("density"), "kg/m<sup>3</sup>");

  EXPECT_EQ(unitFromKey("ixx"), "kg&middot;m<sup>2</sup>");
  EXPECT_EQ(unitFromKey("ixy"), "kg&middot;m<sup>2</sup>");
  EXPECT_EQ(unitFromKey("iyy"), "kg&middot;m<sup>2</sup>");
  EXPECT_EQ(unitFromKey("ixz"), "kg&middot;m<sup>2</sup>");
  EXPECT_EQ(unitFromKey("iyz"), "kg&middot;m<sup>2</sup>");
  EXPECT_EQ(unitFromKey("izz"), "kg&middot;m<sup>2</sup>");

  EXPECT_EQ(unitFromKey("limit_lower"), "");
  EXPECT_EQ(unitFromKey("limit_lower", "PRISMATIC"), "m");
  EXPECT_EQ(unitFromKey("limit_lower", "REVOLUTE"), "rad");

  EXPECT_EQ(unitFromKey("limit_upper"), "");
  EXPECT_EQ(unitFromKey("limit_upper", "PRISMATIC"), "m");
  EXPECT_EQ(unitFromKey("limit_upper", "BALL"), "rad");

  EXPECT_EQ(unitFromKey("limit_effort"), "");
  EXPECT_EQ(unitFromKey("limit_effort", "PRISMATIC"), "N");
  EXPECT_EQ(unitFromKey("limit_effort", "REVOLUTE2"), "Nm");

  EXPECT_EQ(unitFromKey("limit_velocity"), "");
  EXPECT_EQ(unitFromKey("limit_velocity", "PRISMATIC"), "m/s");
  EXPECT_EQ(unitFromKey("limit_velocity", "REVOLUTE"), "rad/s");

  EXPECT_EQ(unitFromKey("damping"), "");
  EXPECT_EQ(unitFromKey("damping", "PRISMATIC"), "Ns/m");
  EXPECT_EQ(unitFromKey("damping", "REVOLUTE"), "Ns");

  EXPECT_EQ(unitFromKey("friction"), "");
  EXPECT_EQ(unitFromKey("friction", "PRISMATIC"), "N");
  EXPECT_EQ(unitFromKey("friction", "REVOLUTE"), "Nm");
}

/////////////////////////////////////////////////
TEST(HelpersTest, rangeFromKey)
{
  double min, max;

  rangeFromKey("pos", min, max);
  EXPECT_DOUBLE_EQ(min, -math::MAX_D);
  EXPECT_DOUBLE_EQ(max, math::MAX_D);

  rangeFromKey("mass", min, max);
  EXPECT_DOUBLE_EQ(min, 0);
  EXPECT_DOUBLE_EQ(max, math::MAX_D);

  rangeFromKey("transparency", min, max);
  EXPECT_DOUBLE_EQ(min, 0);
  EXPECT_DOUBLE_EQ(max, 1);

  rangeFromKey("xyz", min, max);
  EXPECT_DOUBLE_EQ(min, -1);
  EXPECT_DOUBLE_EQ(max, 1);
}

/////////////////////////////////////////////////
TEST(HelpersTest, stringTypeFromKey)
{
  EXPECT_EQ(stringTypeFromKey("name"), StringType::LINE);
  EXPECT_EQ(stringTypeFromKey("innerxml"), StringType::PLAIN_TEXT);
}

/////////////////////////////////////////////////
TEST(HelpersTest, findFirstByProperty)
{
  ASSERT_TRUE(initApp());

  // Construct a list
  auto w0 = new QWidget();
  w0->setProperty("banana", true);

  auto w1 = new QWidget();
  w1->setProperty("banana", false);

  auto w2 = new QWidget();
  w2->setProperty("banana", true);

  QList<QWidget *> list;
  list.append(w0);
  list.append(w1);
  list.append(w2);

  EXPECT_EQ(findFirstByProperty(list, "banana", true), w0);
  EXPECT_EQ(findFirstByProperty(list, "banana", false), w1);
  EXPECT_EQ(findFirstByProperty(list, "acerola", true), nullptr);

  EXPECT_TRUE(stop());
}

