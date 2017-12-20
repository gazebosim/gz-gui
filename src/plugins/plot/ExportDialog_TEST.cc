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
#include "ignition/gui/plugins/plot/ExportDialog.hh"
#include "ignition/gui/Iface.hh"

using namespace ignition;
using namespace gui;
using namespace plugins;
using namespace plot;

/////////////////////////////////////////////////
TEST(ExportDialogTest, VerifyButtons)
{
  setVerbosity(4);
  ASSERT_TRUE(initApp());
  ASSERT_TRUE(loadPlugin("Plot"));

  Canvas *plotCanvas = new Canvas(nullptr);
  ASSERT_NE(nullptr, plotCanvas);

  plotCanvas->show();

  std::list<Canvas *> plots;
  plots.push_back(plotCanvas);

  ExportDialog *exportDialog = new ExportDialog(nullptr, plots);
  ASSERT_NE(nullptr, exportDialog);

  exportDialog->show();

  QList<QPushButton *> buttons = exportDialog->findChildren<QPushButton *>(
      "materialFlat");
  EXPECT_EQ(2, buttons.count());

  exportDialog->hide();
  delete exportDialog;

  plotCanvas->hide();
  delete plotCanvas;

  EXPECT_TRUE(stop());
}
