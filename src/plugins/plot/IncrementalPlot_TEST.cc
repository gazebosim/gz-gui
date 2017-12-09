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
#include <vector>
#include <ignition/common/Time.hh>
#include <ignition/math/Vector2.hh>

#include "ignition/gui/plugins/plot/IncrementalPlot.hh"
#include "ignition/gui/plugins/plot/Curve.hh"
#include "ignition/gui/plugins/plot/Types.hh"
#include "ignition/gui/Iface.hh"

using namespace ignition;
using namespace gui;
using namespace plugins;
using namespace plot;

/////////////////////////////////////////////////
TEST(IncrementalPlotTest, AddRemoveCurve)
{
  setVerbosity(4);
  ASSERT_TRUE(initApp());
  ASSERT_TRUE(loadPlugin("Plot"));

  CurveWeakPtr curve01;
  {
    // Create a new plot.
    IncrementalPlot *plot = new IncrementalPlot(nullptr);
    ASSERT_NE(nullptr, plot);

    // Add a curve and verify that it is in the plot.
    curve01 = plot->AddCurve("curve01");
    EXPECT_FALSE(curve01.expired());
    auto c01 = curve01.lock();
    ASSERT_NE(nullptr, c01);
    EXPECT_EQ(c01, plot->Curve(c01->Id()).lock());
    EXPECT_EQ(c01, plot->Curve(c01->Label()).lock());

    // Try to add a curve with an existing label.
    plot->AddCurve("curve01");
    EXPECT_EQ(1u, plot->Curves().size());

    // Add another curve and verify that both curves are in the plot.
    CurveWeakPtr curve02 = plot->AddCurve("curve02");
    EXPECT_FALSE(curve02.expired());
    auto c02 = curve02.lock();
    ASSERT_NE(nullptr, c02);
    EXPECT_EQ(c02, plot->Curve(c02->Id()).lock());
    EXPECT_EQ(c02, plot->Curve(c02->Label()).lock());

    // Remove first curve.
    plot->RemoveCurve(c01->Id());
    EXPECT_TRUE(plot->Curve(c01->Id()).expired());
    EXPECT_TRUE(plot->Curve(c01->Label()).expired());

    // Remove second curve.
    plot->RemoveCurve(c02->Id());
    EXPECT_TRUE(plot->Curve(c02->Id()).expired());
    EXPECT_TRUE(plot->Curve(c02->Label()).expired());

    // Remove already removed curve.
    plot->RemoveCurve(c02->Id());
    EXPECT_TRUE(plot->Curve(c02->Id()).expired());
    EXPECT_TRUE(plot->Curve(c02->Label()).expired());

    // Check we can add more curves.
    CurveWeakPtr curve03 = plot->AddCurve("curve03");
    auto c03 = curve03.lock();

    ASSERT_NE(nullptr, c03);
    EXPECT_EQ(c03, plot->Curve(c03->Id()).lock());
    EXPECT_EQ(c03, plot->Curve(c03->Label()).lock());

    // Clear the plot - all curves should be removed.
    plot->Clear();
    EXPECT_TRUE(plot->Curve(c03->Id()).expired());
    EXPECT_TRUE(plot->Curve(c03->Label()).expired());

    delete plot;
  }

  // Create a new plot.
  IncrementalPlot *plot = new IncrementalPlot(nullptr);
  ASSERT_NE(nullptr, plot);

  // Try to attach a curve that has been removed by another plot.
  EXPECT_EQ(0u, plot->Curves().size());
  plot->AttachCurve(curve01);
  EXPECT_EQ(0u, plot->Curves().size());

  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(IncrementalPlotTest,  AttachDetachCurve)
{
  setVerbosity(4);
  ASSERT_TRUE(initApp());
  ASSERT_TRUE(loadPlugin("Plot"));

  // Create new plots
  IncrementalPlot *plot01 = new IncrementalPlot(nullptr);
  ASSERT_NE(nullptr, plot01);

  IncrementalPlot *plot02 = new IncrementalPlot(nullptr);
  ASSERT_NE(nullptr, plot02);

  // Add a curve to plot01 and verify that it is in the right plot.
  CurveWeakPtr curve01 = plot01->AddCurve("curve01");
  EXPECT_FALSE(curve01.expired());
  auto c01 = curve01.lock();
  ASSERT_NE(nullptr, c01);
  EXPECT_EQ(c01, plot01->Curve(c01->Id()).lock());
  EXPECT_TRUE(plot02->Curve(c01->Id()).expired());

  // Add another curve to plot01 and verify it is in the right plot.
  CurveWeakPtr curve02 = plot01->AddCurve("curve02");
  EXPECT_FALSE(curve02.expired());
  auto c02 = curve02.lock();
  ASSERT_NE(nullptr, c02);
  EXPECT_EQ(c02, plot01->Curve(c02->Id()).lock());
  EXPECT_TRUE(plot02->Curve(c02->Id()).expired());

  // Detach curve01 from plot01 and verify it is no longer in the plot.
  CurvePtr pc01 = plot01->DetachCurve(c01->Id());
  EXPECT_EQ(pc01, c01);
  EXPECT_TRUE(plot01->Curve(c01->Id()).expired());
  EXPECT_TRUE(plot01->Curve(c01->Label()).expired());

  // Attach curve01 to plot02 and verify.
  plot02->AttachCurve(pc01);
  EXPECT_EQ(c01, plot02->Curve(pc01->Id()).lock());
  EXPECT_EQ(c01, plot02->Curve(pc01->Label()).lock());

  // Detach curve02 from plot01.
  CurvePtr pc02 = plot01->DetachCurve(c02->Id());
  EXPECT_EQ(pc02, c02);
  EXPECT_TRUE(plot02->Curve(c02->Id()).expired());
  EXPECT_TRUE(plot02->Curve(c02->Label()).expired());

  // Detach already datched curve02 from plot01.
  CurvePtr nullptrPc02 = plot01->DetachCurve(c02->Id());
  EXPECT_EQ(nullptrPc02, nullptr);

  // Attach curve02 to plot02 and verify.
  plot02->AttachCurve(pc02);
  EXPECT_EQ(c02, plot02->Curve(pc02->Id()).lock());
  EXPECT_EQ(c02, plot02->Curve(pc02->Label()).lock());

  // Verify we can still add a curves to plot01 and plot02.
  CurveWeakPtr curve03 = plot01->AddCurve("curve03");
  EXPECT_FALSE(curve03.expired());
  auto c03 = curve03.lock();
  ASSERT_NE(nullptr, c03);
  EXPECT_EQ(c03, plot01->Curve(c03->Id()).lock());

  CurveWeakPtr curve04 = plot02->AddCurve("curve04");
  EXPECT_FALSE(curve04.expired());
  auto c04 = curve04.lock();
  ASSERT_NE(nullptr, c04);
  EXPECT_EQ(c04, plot02->Curve(c04->Id()).lock());

  delete plot01;
  delete plot02;

  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(IncrementalPlotTest, AddPoint)
{
  setVerbosity(4);
  ASSERT_TRUE(initApp());
  ASSERT_TRUE(loadPlugin("Plot"));

  // Create new plots
  IncrementalPlot *plot = new IncrementalPlot(nullptr);
  ASSERT_NE(nullptr, plot);

  // Add two curves.
  CurveWeakPtr curve01 = plot->AddCurve("curve01");
  EXPECT_FALSE(curve01.expired());
  auto c01 = curve01.lock();
  ASSERT_NE(nullptr, c01);
  EXPECT_EQ(c01, plot->Curve(c01->Id()).lock());

  CurveWeakPtr curve02 = plot->AddCurve("curve02");
  EXPECT_FALSE(curve02.expired());
  auto c02 = curve02.lock();
  ASSERT_NE(nullptr, c02);
  EXPECT_EQ(c02, plot->Curve(c02->Id()).lock());

  // Add point to curve01 and verify.
  ignition::math::Vector2d point01(12.3, 99);
  EXPECT_EQ(0u, c01->Size());

  // Try to add a point with an incorrect Id.
  plot->AddPoint(99999, point01);
  EXPECT_EQ(0u, c01->Size());

  plot->AddPoint(c01->Id(), point01);
  EXPECT_EQ(1u, c01->Size());
  EXPECT_EQ(point01, c01->Point(0u));

  // Add another point to curve01 and verify.
  ignition::math::Vector2d point02(-1.3, -9.9);
  plot->AddPoint(c01->Id(), point02);
  EXPECT_EQ(2u, c01->Size());
  EXPECT_EQ(point02, c01->Point(1u));

  // Add a list of points to curve02 and verify.
  std::vector<ignition::math::Vector2d> points;
  unsigned int ptSize = 10;
  for (unsigned int i = 0; i < ptSize; ++i)
    points.push_back(ignition::math::Vector2d(i * 2, i * 0.5));

  // Try to add points with an incorrect Id.
  plot->AddPoints(99999, points);
  EXPECT_EQ(0u, c02->Size());

  // Now, the correct Id.
  plot->AddPoints(c02->Id(), points);
  EXPECT_EQ(ptSize, c02->Size());
  for (unsigned int i = 0; i < ptSize; ++i)
    EXPECT_EQ(points[i], c02->Point(i));

  delete plot;

  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(ncrementalPlotTest, SetCurveLabel)
{
  setVerbosity(4);
  ASSERT_TRUE(initApp());
  ASSERT_TRUE(loadPlugin("Plot"));

  CurveWeakPtr curve01;
  unsigned int id;
  {
    // Create a new plot.
    IncrementalPlot *plot = new IncrementalPlot(nullptr);
    ASSERT_NE(nullptr, plot);

    // Add a curve and verify that it is in the plot.
    curve01 = plot->AddCurve("curve01");
    EXPECT_FALSE(curve01.expired());
    auto c01 = curve01.lock();
    ASSERT_NE(nullptr, c01);
    EXPECT_EQ("curve01", c01->Label());
    EXPECT_EQ(c01, plot->Curve(c01->Label()).lock());

    // We don't accept empty labels.
    id = c01->Id();
    plot->SetCurveLabel(id, "");
    EXPECT_EQ("curve01", c01->Label());

    plot->SetCurveLabel(c01->Id(), "new_curve01");
    EXPECT_EQ("new_curve01", c01->Label());
    EXPECT_EQ(c01, plot->Curve(c01->Label()).lock());

    delete plot;
  }

  // Create a new plot.
  IncrementalPlot *plot = new IncrementalPlot(nullptr);
  ASSERT_NE(nullptr, plot);

  // Try to use an Id of a previously removed curve.
  plot->SetCurveLabel(id, "a_new_label");
  EXPECT_EQ(0u, plot->Curves().size());
  EXPECT_TRUE(curve01.expired());

  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(IncrementalPlotTest, Period)
{
  setVerbosity(4);
  ASSERT_TRUE(initApp());
  ASSERT_TRUE(loadPlugin("Plot"));

  // Create a new plot
  IncrementalPlot *plot = new IncrementalPlot(nullptr);
  ASSERT_NE(nullptr, plot);

  // Check the default period.
  EXPECT_EQ(common::Time(10.0), plot->Period());

  // Modify the period.
  plot->SetPeriod(common::Time(20.0));
  EXPECT_EQ(common::Time(20.0), plot->Period());

  delete plot;

  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(IncrementalPlotTest, Grid)
{
  setVerbosity(4);
  ASSERT_TRUE(initApp());
  ASSERT_TRUE(loadPlugin("Plot"));

  // Create a new plot
  IncrementalPlot *plot = new IncrementalPlot(nullptr);
  ASSERT_NE(nullptr, plot);

  EXPECT_TRUE(plot->IsShowGrid());
  plot->ShowGrid(false);
  EXPECT_FALSE(plot->IsShowGrid());
  plot->ShowGrid(true);
  EXPECT_TRUE(plot->IsShowGrid());

  delete plot;

  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(IncrementalPlotTest, HoverLine)
{
  setVerbosity(4);
  ASSERT_TRUE(initApp());
  ASSERT_TRUE(loadPlugin("Plot"));

  // Create a new plot
  IncrementalPlot *plot = new IncrementalPlot(nullptr);
  ASSERT_NE(nullptr, plot);

  EXPECT_FALSE(plot->IsShowHoverLine());
  plot->ShowHoverLine(true);
  EXPECT_TRUE(plot->IsShowHoverLine());
  plot->ShowHoverLine(false);
  EXPECT_FALSE(plot->IsShowHoverLine());

  delete plot;

  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(IncrementalPlotTest, Update)
{
  setVerbosity(4);
  ASSERT_TRUE(initApp());
  ASSERT_TRUE(loadPlugin("Plot"));

  // Create plot
  auto plot = new IncrementalPlot(nullptr);
  ASSERT_NE(nullptr, plot);

  // Update with no curves doesn't cause errors
  plot->Update();

  // Add a curve
  auto curve01 = plot->AddCurve("curve01");
  auto c01 = curve01.lock();
  ASSERT_NE(nullptr, c01);

  // Update with a curve that has no points
  plot->Update();

  // Add point to curve
  plot->AddPoint(c01->Id(), math::Vector2d(0.2, 0.4));

  // Update with a curve that has points
  plot->Update();

  // Add points to curve so it goes beyond the period and the window needs to
  // move
  for (double i = 0.5; i < plot->Period().Double() + 2; i += 0.5)
  {
    plot->AddPoint(c01->Id(), math::Vector2d(i, 0.4));
    plot->Update();
  }

  EXPECT_TRUE(stop());
}
