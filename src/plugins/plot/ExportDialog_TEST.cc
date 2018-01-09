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

#include <list>
#include <thread>

#include <ignition/common/Filesystem.hh>
#include "ignition/gui/plugins/plot/Canvas.hh"
#include "ignition/gui/plugins/plot/ExportDialog.hh"
#include "ignition/gui/Iface.hh"

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
void verifyButtons(ExportDialog *_exportDialog, const bool _enabled)
{
  QList<QPushButton *> buttons = _exportDialog->findChildren<QPushButton *>(
      "materialFlat");

  EXPECT_EQ(2, buttons.count());
  if (buttons.at(0)->text() == QString("&Export to"))
  {
    EXPECT_EQ(_enabled, buttons.at(0)->isEnabled());
    EXPECT_TRUE(buttons.at(1)->isEnabled());
  }
  else if (buttons.at(1)->text() == QString("&Export to"))
  {
    EXPECT_EQ(_enabled, buttons.at(1)->isEnabled());
    EXPECT_TRUE(buttons.at(0)->isEnabled());
  }
  else
  {
    FAIL();
  }
}

/////////////////////////////////////////////////
void select(ExportDialog *_exportDialog, const bool _all)
{
  QList<QAction *> actions = _exportDialog->findChildren<QAction *>();

  QAction *selectAll = nullptr;
  QAction *selectNone = nullptr;
  for (int i = 0; i < actions.count(); ++i)
  {
    std::string txt = actions.at(i)->text().toStdString();
    if (txt == "Select all")
      selectAll = actions.at(i);
    if (txt == "Clear selection")
      selectNone = actions.at(i);
  }
  ASSERT_NE(nullptr, selectAll);
  ASSERT_NE(nullptr, selectNone);

  if (_all)
    selectAll->trigger();
  else
    selectNone->trigger();
}

/////////////////////////////////////////////////
TEST(ExportDialogTest, Empty)
{
  setVerbosity(4);
  ASSERT_TRUE(initApp());

  std::list<Canvas *> plots;

  // Create a new plot canvas widget
  ExportDialog *exportDialog = new ExportDialog(nullptr, plots);

  ASSERT_NE(nullptr, exportDialog);

  exportDialog->show();

  auto listView = exportDialog->findChild<QListView *>();
  ASSERT_NE(nullptr, listView);
  EXPECT_EQ(0, listView->model()->columnCount());
  EXPECT_EQ(0, listView->model()->rowCount());

  verifyButtons(exportDialog, false);

  exportDialog->hide();
  delete exportDialog;

  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(ExportDialogTest, OnePlot)
{
  setVerbosity(4);
  ASSERT_TRUE(initApp());

  std::list<Canvas *> plots;

  // Create a new plot canvas widget
  Canvas *plotCanvas = new Canvas(nullptr);
  ASSERT_NE(nullptr, plotCanvas);
  plotCanvas->show();

  // there should be an empty plot
  EXPECT_EQ(1u, plotCanvas->PlotCount());

  plots.push_back(plotCanvas);

  // Create a new plot canvas widget
  ExportDialog *exportDialog = new ExportDialog(nullptr, plots);

  ASSERT_NE(nullptr, exportDialog);

  exportDialog->show();

  // Get the list view, which holds the plotcanvas items.
  auto listView = exportDialog->findChild<QListView *>();
  ASSERT_NE(nullptr, listView);
  EXPECT_EQ(1, listView->model()->columnCount());
  EXPECT_EQ(1, listView->model()->rowCount());

  // Verify that the export button is disabled.
  verifyButtons(exportDialog, false);

  // Get the rectangle for the plot item in the list view
  QRect rect = listView->visualRect(listView->model()->index(0, 0));

  // Click the list item
  auto mouseEvent = new QMouseEvent(QEvent::MouseButtonPress,
    rect.center(), Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
  QCoreApplication::postEvent(listView->viewport(), mouseEvent);
  QCoreApplication::processEvents();

  // ToDo: Fix this test.
  // The export button should now be enabled.
  verifyButtons(exportDialog, true);

  // Click the list item again
  mouseEvent = new QMouseEvent(QEvent::MouseButtonPress,
    rect.center(), Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
  QCoreApplication::postEvent(listView->viewport(), mouseEvent);
  QCoreApplication::processEvents();

  // The export button should now be disabled.
  verifyButtons(exportDialog, false);

  // Select all
  select(exportDialog, true);
  verifyButtons(exportDialog, true);

  // Select none
  select(exportDialog, false);
  verifyButtons(exportDialog, false);

  exportDialog->hide();
  delete exportDialog;

  plotCanvas->hide();
  delete plotCanvas;

  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(ExportDialogTest, exportPDF)
{
  setVerbosity(4);
  ASSERT_TRUE(initApp());

  std::string newTempDir;
  ASSERT_TRUE(createAndSwitchToTempDir(newTempDir));

  // Create a new plot canvas widget
  Canvas *plotCanvas = new Canvas(nullptr);
  ASSERT_NE(nullptr, plotCanvas);
  plotCanvas->show();

  // Add a plot to the canvas
  int index = plotCanvas->AddPlot();
  EXPECT_EQ(0, index);

  // Export the plot to pdf
  plotCanvas->Export(newTempDir, FileType::PDFFile);

  ignition::common::DirIter endIter;
  bool foundFile = false;
  for (ignition::common::DirIter dirIter(newTempDir);
       dirIter != endIter; ++dirIter)
  {
    if (ignition::common::basename(*dirIter) == plotCanvas->Title() + ".pdf")
      foundFile = true;
  }

  EXPECT_TRUE(foundFile);

  // Clean.
  ignition::common::removeAll(newTempDir);

  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(ExportDialogTest, exportCSV)
{
  setVerbosity(4);
  ASSERT_TRUE(initApp());

  std::string newTempDir;
  ASSERT_TRUE(createAndSwitchToTempDir(newTempDir));

  // Create a new plot canvas widget
  Canvas *plotCanvas = new Canvas(nullptr);
  ASSERT_NE(nullptr, plotCanvas);
  plotCanvas->show();

  // Add a plot to the canvas
  int index = plotCanvas->AddPlot();
  EXPECT_EQ(1, index);

  // Add a variable to the plot
  std::string var = std::string("var01");
  plotCanvas->AddVariable(var, index);
  plotCanvas->SetVariableLabel(index, "test");

  // Export the plot to pdf
  plotCanvas->Export(newTempDir, FileType::CSVFile);

  ignition::common::DirIter endIter;
  bool foundFile = false;
  for (ignition::common::DirIter dirIter(newTempDir);
       dirIter != endIter; ++dirIter)
  {
    if (ignition::common::basename(*dirIter) ==
        plotCanvas->Title() + "-var01.csv")
    {
      foundFile = true;
    }
  }

  EXPECT_TRUE(foundFile);

  // Clean.
  ignition::common::removeAll(newTempDir);

  EXPECT_TRUE(stop());
}
