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
#include "ignition/gui/MainWindow.hh"

using namespace ignition;
using namespace gui;
using namespace plugins;
using namespace plot;

/////////////////////////////////////////////////
TEST(PlotTest, Load)
{
  setVerbosity(4);
  ASSERT_TRUE(initApp());

  // Load plugin
  EXPECT_TRUE(loadPlugin("Plot"));

  // Create main window
  EXPECT_TRUE(createMainWindow());
  auto win = mainWindow();
  ASSERT_NE(nullptr, win);

  // Show, but don't exec, so we don't block
  win->show();

  // Get plot plugin
  auto plotWidget = win->findChild<Plot *>();
  ASSERT_NE(nullptr, plotWidget);

/*
  // there should be an empty canvas
  EXPECT_EQ(1u, plotWidget->CanvasCount());

  // add canvas
  Canvas *canvas01 = plotWidget->AddCanvas();
  ASSERT_NE(nullptr, canvas01);
  EXPECT_EQ(2u, plotWidget->CanvasCount());

  Canvas *canvas02 = plotWidget->AddCanvas();
  ASSERT_NE(nullptr, canvas02);
  EXPECT_EQ(3u, plotWidget->CanvasCount());

  Canvas *canvas03 = plotWidget->AddCanvas();
  ASSERT_NE(nullptr, canvas03);
  EXPECT_EQ(4u, plotWidget->CanvasCount());

  // remove canvas
  plotWidget->RemoveCanvas(canvas01);
  EXPECT_EQ(3u, plotWidget->CanvasCount());

  plotWidget->RemoveCanvas(canvas02);
  EXPECT_EQ(2u, plotWidget->CanvasCount());

  // remove already removed canvas
  plotWidget->RemoveCanvas(canvas02);
  EXPECT_EQ(2u, plotWidget->CanvasCount());

  // remove last canvas
  plotWidget->RemoveCanvas(canvas03);
  EXPECT_EQ(1u, plotWidget->CanvasCount());

  // check we can add more canvases

  Canvas *canvas04 = plotWidget->AddCanvas();
  ASSERT_NE(nullptr, canvas04);
  EXPECT_EQ(2u, plotWidget->CanvasCount());
  // clear canvases
  plotWidget->Clear();
  EXPECT_EQ(0u, plotWidget->CanvasCount());
*/

  EXPECT_TRUE(stop());
}
