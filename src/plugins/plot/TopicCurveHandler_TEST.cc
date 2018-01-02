/*
 * Copyright (C) 2018 Open Source Robotics Foundation
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

#include "ignition/gui/plugins/plot/Curve.hh"
#include "ignition/gui/plugins/plot/TopicCurveHandler.hh"
#include "ignition/gui/plugins/plot/Types.hh"
#include "ignition/gui/Iface.hh"

using namespace ignition;
using namespace gui;
using namespace plugins;
using namespace plot;

/////////////////////////////////////////////////
TEST(TopicCurveHandlerTest, AddRemoveCurve)
{
  setVerbosity(4);
  ASSERT_TRUE(initApp());

  // create a TopicCurveHandler and verify initial state
  TopicCurveHandler handler;
  EXPECT_EQ(0u, handler.CurveCount());

  // Create a new plot curve and add it to the handler
  CurvePtr plotCurve01(new Curve("curve01"));
  handler.AddCurve("/world_stats?p=sim_time", plotCurve01);
  EXPECT_EQ(1u, handler.CurveCount());

  // verify we can have two different curves associated to the same topic
  CurvePtr plotCurve02(new Curve("curve02"));
  handler.AddCurve("/world_stats?p=iterations", plotCurve02);
  EXPECT_EQ(2u, handler.CurveCount());

  // add another curve associated to a different topic
  CurvePtr plotCurve03(new Curve("curve03"));
  handler.AddCurve("/pose/local/info?p=time", plotCurve03);
  EXPECT_EQ(3u, handler.CurveCount());

  // test removing curves
  handler.RemoveCurve(plotCurve01);
  EXPECT_EQ(2u, handler.CurveCount());
  handler.RemoveCurve(plotCurve02);
  EXPECT_EQ(1u, handler.CurveCount());
  handler.RemoveCurve(plotCurve03);
  EXPECT_EQ(0u, handler.CurveCount());

  EXPECT_TRUE(stop());
}
