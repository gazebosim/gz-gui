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
#include <set>
#include <vector>
#include <ignition/math/Vector2.hh>

#include "ignition/gui/plugins/plot/Curve.hh"
#include "ignition/gui/plugins/plot/Types.hh"
#include "ignition/gui/Iface.hh"

using namespace ignition;
using namespace gui;
using namespace plugins;
using namespace plot;

/////////////////////////////////////////////////
TEST(CurveTest, Curve)
{
  ASSERT_TRUE(initApp());

  // Create a new plot curve.
  Curve *plotCurve = new Curve("curve01");

  ASSERT_NE(nullptr, plotCurve);

  // Age.
  EXPECT_EQ(0u, plotCurve->Age());
  plotCurve->SetAge(1u);
  EXPECT_EQ(1u, plotCurve->Age());

  // Active.
  EXPECT_TRUE(plotCurve->Active());
  plotCurve->SetActive(false);
  EXPECT_FALSE(plotCurve->Active());

  // Label.
  EXPECT_EQ("curve01", plotCurve->Label());
  plotCurve->SetLabel("new_curve01");
  EXPECT_EQ("new_curve01", plotCurve->Label());

  delete plotCurve;

  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(CurveTest, CurveId)
{
  ASSERT_TRUE(initApp());

  // A set of unique plot curve ids.
  std::set<unsigned int> ids;

  // Create new variable pills and verify they all have unique ids.
  Curve *curve01 = new Curve("curve01");
  ASSERT_NE(nullptr, curve01);
  unsigned int id = curve01->Id();
  ASSERT_EQ(0u, ids.count(id));
  ids.insert(id);

  Curve *curve02 = new Curve("curve02");
  ASSERT_NE(nullptr, curve02);
  id = curve02->Id();
  EXPECT_EQ(0u, ids.count(id));
  ids.insert(id);

  Curve *curve03 = new Curve("curve03");
  ASSERT_NE(nullptr, curve03);
  curve03->SetId(999u);
  id = curve03->Id();
  EXPECT_EQ(999u, id);
  EXPECT_EQ(0u, ids.count(id));
  ids.insert(id);

  delete curve01;
  delete curve02;
  delete curve03;

  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(CurveTest, AddPoint)
{
  ASSERT_TRUE(initApp());

  // Create a new plot curve.
  Curve *plotCurve = new Curve("curve01");

  ASSERT_NE(nullptr, plotCurve);

  // Verify the curve has no data.
  EXPECT_EQ(0u, plotCurve->Size());

  // Add points.
  math::Vector2d point01(12.3, -39.4);
  // If it's inactive, the point shouldn't be added.
  plotCurve->SetActive(false);
  plotCurve->AddPoint(point01);
  EXPECT_EQ(0u, plotCurve->Size());

  // Active again.
  plotCurve->SetActive(true);
  plotCurve->AddPoint(point01);
  EXPECT_EQ(1u, plotCurve->Size());
  EXPECT_EQ(point01, plotCurve->Point(0));

  math::Vector2d point02(3.3, -3.4);
  plotCurve->AddPoint(point02);
  EXPECT_EQ(2u, plotCurve->Size());
  EXPECT_EQ(point02, plotCurve->Point(1));

  EXPECT_EQ(math::Vector2d(3.3, -39.4), plotCurve->Min());
  EXPECT_EQ(math::Vector2d(12.3, -3.4), plotCurve->Max());

  plotCurve->Clear();

  // Create a list of points and add them to the curve.
  std::vector<math::Vector2d> points;
  unsigned int ptSize = 11000u;
  for (unsigned int i = 0u; i < ptSize; ++i)
    points.push_back(math::Vector2d(i, ptSize - i));

  // If it's inactive, the points shouldn't be added.
  plotCurve->SetActive(false);
  plotCurve->AddPoints(points);
  EXPECT_EQ(0u, plotCurve->Size());

  // Active again.
  plotCurve->SetActive(true);
  plotCurve->AddPoints(points);

  // Verify the points are appended to the curve.
  EXPECT_EQ(ptSize, plotCurve->Size());
  for (unsigned int i = 0; i < ptSize; ++i)
    EXPECT_EQ(points[i], plotCurve->Point(i));

  // Wrong index point.
  auto point = plotCurve->Point(99999);
  EXPECT_TRUE(math::isnan(point.X()));
  EXPECT_TRUE(math::isnan(point.Y()));

  ASSERT_NE(nullptr, plotCurve->QwtCurve());

  // Verify that some of the oldest points are removed when reaching
  // "maxSampleSize" points.
  plotCurve->AddPoint(point02);
  EXPECT_EQ(10001u, plotCurve->Size());
  EXPECT_EQ(points[1000], plotCurve->Point(0));
  EXPECT_EQ(point02, plotCurve->Point(ptSize - 1000));

  delete plotCurve;

  EXPECT_TRUE(stop());
}
