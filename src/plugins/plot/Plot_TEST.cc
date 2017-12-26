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

#include "ignition/gui/plugins/plot/Plot.hh"
#include "ignition/gui/plugins/plot/Types.hh"
#include "ignition/gui/Iface.hh"

using namespace ignition;
using namespace gui;
using namespace plugins;
using namespace plot;

// ToDo: Fix segfault
/////////////////////////////////////////////////
TEST(PlotTest, Load)
{
  EXPECT_TRUE(initApp());

  EXPECT_TRUE(loadPlugin("Plot"));

  // Create a new plot window widget
  Plot *plotWindow = new Plot();
  ASSERT_NE(nullptr, plotWindow);

  plotWindow->show();

  // there should be an empty canvas
  EXPECT_EQ(1u, plotWindow->CanvasCount());

  // add canvas
  Canvas *canvas01 = plotWindow->AddCanvas();
  ASSERT_NE(nullptr, canvas01);
  EXPECT_EQ(2u, plotWindow->CanvasCount());

  Canvas *canvas02 = plotWindow->AddCanvas();
  ASSERT_NE(nullptr, canvas02);
  EXPECT_EQ(3u, plotWindow->CanvasCount());

  Canvas *canvas03 = plotWindow->AddCanvas();
  ASSERT_NE(nullptr, canvas03);
  EXPECT_EQ(4u, plotWindow->CanvasCount());

  // remove canvas
  plotWindow->RemoveCanvas(canvas01);
  EXPECT_EQ(3u, plotWindow->CanvasCount());

  plotWindow->RemoveCanvas(canvas02);
  EXPECT_EQ(2u, plotWindow->CanvasCount());

  // remove already removed canvas
  plotWindow->RemoveCanvas(canvas02);
  EXPECT_EQ(2u, plotWindow->CanvasCount());

  // remove last canvas
  plotWindow->RemoveCanvas(canvas03);
  EXPECT_EQ(1u, plotWindow->CanvasCount());

  // check we can add more canvases
  Canvas *canvas04 = plotWindow->AddCanvas();
  ASSERT_NE(nullptr, canvas04);
  EXPECT_EQ(2u, plotWindow->CanvasCount());

  // clear canvases
  plotWindow->Clear();
  EXPECT_EQ(0u, plotWindow->CanvasCount());

  plotWindow->hide();
  delete plotWindow;

  EXPECT_TRUE(stop());
}
