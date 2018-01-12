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

#include "ignition/gui/Iface.hh"
#include "ignition/gui/MainWindow.hh"
#include "ignition/gui/VariablePillContainer.hh"
#include "ignition/gui/plugins/plot/IncrementalPlot.hh"
#include "ignition/gui/plugins/plot/Plot.hh"

using namespace ignition;
using namespace gui;
using namespace plugins;
using namespace plot;

/////////////////////////////////////////////////
TEST(PlotTest, AddRemoveVariables)
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
  auto plotPlugin = win->findChild<Plot *>();
  ASSERT_NE(nullptr, plotPlugin);

  // There should be an empty plot
  auto plots = plotPlugin->findChildren<IncrementalPlot *>();
  EXPECT_EQ(1, plots.size());
  EXPECT_TRUE(plots[0]->isVisible());

  // There are two variable containers
  auto containers = plotPlugin->findChildren<VariablePillContainer *>();
  EXPECT_EQ(2, containers.size());
  EXPECT_EQ(1u, containers[0]->VariablePillCount());
  EXPECT_EQ(0u, containers[1]->VariablePillCount());

  // Simulate dropping a variable on a plot
  plots[0]->VariableAdded("banana");

  // Check the empty plot is hidden and a new one is created
  plots = plotPlugin->findChildren<IncrementalPlot *>();
  EXPECT_EQ(2, plots.size());
  EXPECT_FALSE(plots[0]->isVisible());
  EXPECT_TRUE(plots[1]->isVisible());

  // Simulate dropping a variable on the container
  containers[1]->VariableAdded(1, "coconut", VariablePill::EmptyVariable);

  // Check another plot is created
  plots = plotPlugin->findChildren<IncrementalPlot *>();
  EXPECT_EQ(3, plots.size());
  EXPECT_FALSE(plots[0]->isVisible());
  EXPECT_TRUE(plots[1]->isVisible());
  EXPECT_TRUE(plots[2]->isVisible());

  // Simulate dropping a variable on an existing pill
  containers[1]->VariableAdded(2, "acerola", 1);

  // Check we still have 3 plots
  plots = plotPlugin->findChildren<IncrementalPlot *>();
  EXPECT_EQ(3, plots.size());
  EXPECT_FALSE(plots[0]->isVisible());
  EXPECT_TRUE(plots[1]->isVisible());
  EXPECT_TRUE(plots[2]->isVisible());

  // Simulate moving a variable on the container
  containers[1]->VariableMoved(2, VariablePill::EmptyVariable);

  // Check now we have 4 plots
  plots = plotPlugin->findChildren<IncrementalPlot *>();
  EXPECT_EQ(4, plots.size());
  EXPECT_FALSE(plots[0]->isVisible());
  EXPECT_TRUE(plots[1]->isVisible());
  EXPECT_TRUE(plots[2]->isVisible());
  EXPECT_TRUE(plots[3]->isVisible());

  // Simulate deleting a variable from the container
  containers[1]->VariableRemoved(2, VariablePill::EmptyVariable);

  // Check now we have 3 plots again
  plots = plotPlugin->findChildren<IncrementalPlot *>();
  EXPECT_EQ(3, plots.size());
  EXPECT_FALSE(plots[0]->isVisible());
  EXPECT_TRUE(plots[1]->isVisible());
  EXPECT_TRUE(plots[2]->isVisible());

  EXPECT_TRUE(stop());
}

