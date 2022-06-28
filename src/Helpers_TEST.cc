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

#include <gz/math/Helpers.hh>
#include <gz/utils/ExtraTestMacros.hh>

#include "test_config.hh"  // NOLINT(build/include)
#include "gz/gui/Application.hh"
#include "gz/gui/MainWindow.hh"
#include "gz/gui/Helpers.hh"

int gg_argc = 1;
char* gg_argv[] =
{
  reinterpret_cast<char*>(const_cast<char*>("./Helpers_TEST")),
};

using namespace gz;
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
// See https://github.com/gazebosim/gz-gui/issues/75
TEST(HelpersTest, GZ_UTILS_TEST_DISABLED_ON_WIN32(findFirstByProperty))
{
  Application app(gg_argc, gg_argv);

  // Construct a list
  auto o0 = new QObject();
  ASSERT_NE(nullptr, o0);
  o0->setProperty("banana", 1.0);

  auto o1 = new QObject();
  ASSERT_NE(nullptr, o1);
  o1->setProperty("banana", 2.0);

  auto o2 = new QObject();
  ASSERT_NE(nullptr, o2);
  o2->setProperty("banana", 1.0);

  QList<QObject *> list;
  list.append(o0);
  list.append(o1);
  list.append(o2);

  EXPECT_EQ(findFirstByProperty(list, "banana", 1.0), o0);
  EXPECT_EQ(findFirstByProperty(list, "banana", 2.0), o1);
  EXPECT_EQ(findFirstByProperty(list, "banana", 3.0), nullptr);
  EXPECT_EQ(findFirstByProperty(list, "acerola", 1.0), nullptr);
}

/////////////////////////////////////////////////
// See https://github.com/gazebosim/gz-gui/issues/75
TEST(HelpersTest, GZ_UTILS_TEST_DISABLED_ON_WIN32(worldNames))
{
  // No app, no window, no names
  EXPECT_TRUE(worldNames().empty());

  Application app(gg_argc, gg_argv);
  auto mainWindow = app.findChild<MainWindow *>();
  ASSERT_NE(nullptr, mainWindow);

  // No names by default
  EXPECT_TRUE(worldNames().empty());

  QStringList names{"banana", "grape"};
  mainWindow->setProperty("worldNames", names);

  // Has names
  EXPECT_FALSE(worldNames().empty());
  ASSERT_EQ(2, worldNames().size());
  EXPECT_EQ("banana", worldNames()[0]);
  EXPECT_EQ("grape", worldNames()[1]);

  mainWindow->setProperty("worldNames", QStringList());

  // No more names
  EXPECT_TRUE(worldNames().empty());
}

/////////////////////////////////////////////////
TEST(HelpersTest, GZ_UTILS_TEST_DISABLED_ON_WIN32(renderEngine))
{
  // No app, no window, no render engine
  EXPECT_TRUE(renderEngineName().empty());

  Application app(gg_argc, gg_argv);
  auto mainWindow = app.findChild<MainWindow *>();
  ASSERT_NE(nullptr, mainWindow);

  // No render engine by default
  EXPECT_TRUE(renderEngineName().empty());

  // Set the render engine GUI name
  mainWindow->SetRenderEngine("ogre");

  // Has render engine
  EXPECT_FALSE(renderEngineName().empty());
  EXPECT_EQ("ogre", renderEngineName());

  // Set no render engine
  mainWindow->SetRenderEngine({});

  // No render engine set
  EXPECT_TRUE(renderEngineName().empty());
}
