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

#include "ignition/gui/plugins/plot/Canvas.hh"
#include "ignition/gui/plugins/plot/Curve.hh"
#include "ignition/gui/plugins/plot/Types.hh"
#include "ignition/gui/Iface.hh"

using namespace ignition;
using namespace gui;
using namespace plugins;
using namespace plot;

/////////////////////////////////////////////////
TEST(CanvasTest, AddRemovePlot)
{
  setVerbosity(4);
  ASSERT_TRUE(initApp());

  // Create a new plot canvas widget
  Canvas *plotCanvas = new Canvas(nullptr);

  ASSERT_NE(nullptr, plotCanvas);

  plotCanvas->show();

  // there should be an empty plot
  EXPECT_EQ(1u, plotCanvas->PlotCount());

  // add plot
  unsigned int plot01 = plotCanvas->AddPlot();
  EXPECT_EQ(2u, plotCanvas->PlotCount());

  unsigned int plot02 = plotCanvas->AddPlot();
  EXPECT_EQ(3u, plotCanvas->PlotCount());

  unsigned int plot03 = plotCanvas->AddPlot();
  EXPECT_EQ(4u, plotCanvas->PlotCount());

  // remove plot
  plotCanvas->RemovePlot(plot01);
  EXPECT_EQ(3u, plotCanvas->PlotCount());

  plotCanvas->RemovePlot(plot02);
  EXPECT_EQ(2u, plotCanvas->PlotCount());

  // remove already removed plot
  plotCanvas->RemovePlot(plot02);
  EXPECT_EQ(2u, plotCanvas->PlotCount());

  // remove last plot
  plotCanvas->RemovePlot(plot03);
  EXPECT_EQ(1u, plotCanvas->PlotCount());

  // check we can add more plots
  plotCanvas->AddPlot();
  EXPECT_EQ(2u, plotCanvas->PlotCount());

  plotCanvas->hide();
  delete plotCanvas;

  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(CanvasTest, AddRemoveVariable)
{
  setVerbosity(4);
  ASSERT_TRUE(initApp());

  // Create a new plot canvas widget
  Canvas *plotCanvas = new Canvas(nullptr);
  ASSERT_NE(nullptr, plotCanvas);

  plotCanvas->show();

  // there should be an empty plot
  EXPECT_EQ(1u, plotCanvas->PlotCount());

  // add variable to first plot
  unsigned int var01 = plotCanvas->AddVariable("var01");
  EXPECT_EQ(1u, plotCanvas->PlotCount());
  EXPECT_NE(Canvas::EmptyPlot, plotCanvas->PlotByVariable(var01));

  // add another variable - this creates a new plot
  unsigned int var02 = plotCanvas->AddVariable("var02");
  EXPECT_EQ(2u, plotCanvas->PlotCount());
  EXPECT_NE(Canvas::EmptyPlot, plotCanvas->PlotByVariable(var02));

  // add one more variable
  unsigned int var03 = plotCanvas->AddVariable("var03");
  EXPECT_EQ(3u, plotCanvas->PlotCount());
  EXPECT_NE(Canvas::EmptyPlot, plotCanvas->PlotByVariable(var03));

  // remove variable
  plotCanvas->RemoveVariable(var01);
  EXPECT_EQ(2u, plotCanvas->PlotCount());
  EXPECT_EQ(Canvas::EmptyPlot, plotCanvas->PlotByVariable(var01));

  plotCanvas->RemoveVariable(var02);
  EXPECT_EQ(1u, plotCanvas->PlotCount());
  EXPECT_EQ(Canvas::EmptyPlot, plotCanvas->PlotByVariable(var02));

  // remove already removed plot
  plotCanvas->RemoveVariable(var02);
  EXPECT_EQ(1u, plotCanvas->PlotCount());
  EXPECT_EQ(Canvas::EmptyPlot, plotCanvas->PlotByVariable(var02));

  // remove last variable - this should leave an empty plot in the canvas
  plotCanvas->RemoveVariable(var03);
  EXPECT_EQ(1u, plotCanvas->PlotCount());
  EXPECT_EQ(Canvas::EmptyPlot, plotCanvas->PlotByVariable(var03));

  // check we can add more variables - should now have one plot with variable
  unsigned int var04 = plotCanvas->AddVariable("var04");
  EXPECT_EQ(1u, plotCanvas->PlotCount());
  EXPECT_NE(Canvas::EmptyPlot, plotCanvas->PlotByVariable(var04));

  plotCanvas->hide();
  delete plotCanvas;

  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(CanvasTest, VariableLabel)
{
  setVerbosity(4);
  ASSERT_TRUE(initApp());

  // Create a new plot canvas widget
  Canvas *plotCanvas = new Canvas(nullptr);
  EXPECT_NE(nullptr, plotCanvas);

  plotCanvas->show();

  // add a variable to plot
  unsigned int var01 = plotCanvas->AddVariable("var01");
  EXPECT_EQ(1u, plotCanvas->PlotCount());
  EXPECT_NE(Canvas::EmptyPlot, plotCanvas->PlotByVariable(var01));

  // find the curve associated with the variable
  CurveWeakPtr curve = plotCanvas->PlotCurve(var01);
  auto c = curve.lock();
  EXPECT_EQ(std::string("var01"), c->Label());

  // set new label and verify.
  // ToDo: Fix it.
  // plotCanvas->SetVariableLabel(var01, "new_var01");
  // EXPECT_EQ(std::string("new_var01"), c->Label());

  plotCanvas->hide();
  delete plotCanvas;

  EXPECT_TRUE(stop());
}
