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
#include <ignition/common/Filesystem.hh>

#include "ignition/gui/Iface.hh"
#include "ignition/gui/MainWindow.hh"
#include "ignition/gui/VariablePillContainer.hh"
#include "ignition/gui/plugins/plot/Curve.hh"
#include "ignition/gui/plugins/plot/IncrementalPlot.hh"
#include "ignition/gui/plugins/plot/Plot.hh"

using namespace ignition;
using namespace gui;
using namespace plugins;
using namespace plot;

/////////////////////////////////////////////////
/// \brief Copy from IgnitionCommon::FileSystem_TEST.cc
bool createAndSwitchToTempDir(std::string &_newTempPath)
{
  std::string tmppath;
  const char *tmp = getenv("TMPDIR");
  if (tmp)
  {
    tmppath = std::string(tmp);
  }
  else
  {
    tmppath = std::string("/tmp");
  }

  tmppath += "/XXXXXX";

  char *dtemp = mkdtemp(const_cast<char *>(tmppath.c_str()));
  if (dtemp == nullptr)
  {
    return false;
  }
  if (chdir(dtemp) < 0)
  {
    return false;
  }

  char resolved[PATH_MAX];
  if (realpath(dtemp, resolved) == nullptr)
  {
    return false;
  }

  _newTempPath = std::string(resolved);

  return true;
}

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

  // See issue #24
#if !defined(__APPLE__)
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

  // Simulate dropping a variable on an existing plot
  plots[1]->VariableAdded("papaya");

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

  // Clear all plots
  auto toolButtons = plotPlugin->findChildren<QToolButton *>();
  EXPECT_EQ(1, toolButtons.size());

  auto menu = toolButtons[0]->menu();
  ASSERT_NE(nullptr, menu);
  ASSERT_EQ(4, menu->actions().size());
  EXPECT_EQ("Clear all fields", menu->actions()[0]->text().toStdString());

  bool closed{false};
  QTimer::singleShot(300, [&]
  {
    // Select file
    auto box = win->findChild<QMessageBox *>();
    ASSERT_NE(nullptr, box);

    // Accept
    auto buttons = box->findChildren<QPushButton *>();
    EXPECT_EQ(2, buttons.size());
    buttons[1]->click();
    closed = true;
  });

  // Trigger clear
  menu->actions()[0]->trigger();

  EXPECT_TRUE(closed);

  // Check we're back to the empty plot
  plots = plotPlugin->findChildren<IncrementalPlot *>();
  EXPECT_EQ(1, plots.size());
  EXPECT_TRUE(plots[0]->isVisible());
#endif

  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(PlotTest, Export)
{
  setVerbosity(4);
  ASSERT_TRUE(initApp());

  std::string newTempDir;
  ASSERT_TRUE(createAndSwitchToTempDir(newTempDir));

  // Load plugin
  EXPECT_TRUE(loadPlugin("Plot"));

  // Create main window
  EXPECT_TRUE(createMainWindow());
  auto win = mainWindow();
  ASSERT_NE(nullptr, win);

  // Show, but don't exec, so we don't block
  win->show();

  // See issue #24
#if !defined(__APPLE__)
  // Get plot plugin
  auto plotPlugin = win->findChild<Plot *>();
  ASSERT_NE(nullptr, plotPlugin);

  // Get empty plot
  auto plots = plotPlugin->findChildren<IncrementalPlot *>();
  EXPECT_EQ(1, plots.size());

  // Simulate dropping a variable onto it
  plots[0]->VariableAdded("banana");

  // Get newly created plot
  plots = plotPlugin->findChildren<IncrementalPlot *>();
  EXPECT_EQ(2, plots.size());

  // Add data
  auto curve = plots[1]->AddCurve("banana");
  auto c = curve.lock();
  ASSERT_NE(nullptr, c);

  plots[1]->AddPoint(c->Id(), math::Vector2d(-1, 0.2));

  // Get export menu
  auto toolButtons = plotPlugin->findChildren<QToolButton *>();
  EXPECT_EQ(1, toolButtons.size());

  auto menu = toolButtons[0]->menu();
  ASSERT_NE(nullptr, menu);

  auto exportMenu = menu->findChild<QMenu *>();
  ASSERT_NE(nullptr, exportMenu);
  ASSERT_EQ(2, exportMenu->actions().size());

  // CSV Export
  {
    // Get action
    auto csvAct = exportMenu->actions()[0];
    EXPECT_EQ("CSV (.csv)", csvAct->text().toStdString());

    // Choose file after a while
    bool closed{false};
    QTimer::singleShot(300, [&]
    {
      auto fileDialogs = win->findChildren<QFileDialog *>();
      ASSERT_EQ(fileDialogs.size(), 1);

      // Select file
      auto edits = fileDialogs[0]->findChildren<QLineEdit *>();
      ASSERT_GT(edits.size(), 0);
      edits[0]->setText(QString::fromStdString(newTempDir));

      // Accept
      auto buttons = fileDialogs[0]->findChildren<QPushButton *>();
      EXPECT_GT(buttons.size(), 0);
      buttons[0]->click();
      closed = true;
    });

    // Trigger export
    csvAct->trigger();

    EXPECT_TRUE(closed);

    ignition::common::DirIter endIter;
    bool foundFile = false;
    for (ignition::common::DirIter dirIter(newTempDir);
         dirIter != endIter; ++dirIter)
    {
      if (ignition::common::basename(*dirIter) == "Plot Name-banana.csv")
        foundFile = true;
    }

    EXPECT_TRUE(foundFile);
  }

  // PDF Export
  {
    // Get action
    auto pdfAct = exportMenu->actions()[1];
    EXPECT_EQ("PDF (.pdf)", pdfAct->text().toStdString());

    // Choose file after a while
    bool closed{false};
    QTimer::singleShot(300, [&]
    {
      auto fileDialogs = win->findChildren<QFileDialog *>();
      ASSERT_EQ(fileDialogs.size(), 1);

      // Select file
      auto edits = fileDialogs[0]->findChildren<QLineEdit *>();
      ASSERT_GT(edits.size(), 0);
      edits[0]->setText(QString::fromStdString(newTempDir));

      // Accept
      auto buttons = fileDialogs[0]->findChildren<QPushButton *>();
      EXPECT_GT(buttons.size(), 0);
      buttons[0]->click();
      closed = true;
    });

    // Trigger export
    pdfAct->trigger();

    EXPECT_TRUE(closed);

    ignition::common::DirIter endIter;
    bool foundFile = false;
    for (ignition::common::DirIter dirIter(newTempDir);
         dirIter != endIter; ++dirIter)
    {
      if (ignition::common::basename(*dirIter) == "Plot Name.pdf")
        foundFile = true;
    }

    EXPECT_TRUE(foundFile);
  }

  // Clean.
  ignition::common::removeAll(newTempDir);
#endif

  EXPECT_TRUE(stop());
}

