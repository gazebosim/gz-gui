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
#include <ignition/common/Console.hh>
#include <ignition/rendering.hh>

#include "ignition/gui/CollapsibleWidget.hh"
#include "ignition/gui/ColorWidget.hh"
#include "ignition/gui/Helpers.hh"
#include "ignition/gui/Iface.hh"
#include "ignition/gui/MainWindow.hh"
#include "ignition/gui/NumberWidget.hh"
#include "ignition/gui/Pose3dWidget.hh"
#include "ignition/gui/Plugin.hh"
#include "ignition/gui/QtMetatypes.hh"

using namespace ignition;
using namespace gui;

/////////////////////////////////////////////////
TEST(Grid3DTest, WithoutScene)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // Load Grid3D without Scene3D
  EXPECT_TRUE(loadPlugin("Grid3D"));

  // Create main window
  EXPECT_TRUE(createMainWindow());

  // Check there is no scene
  auto engine = rendering::engine("ogre");
  ASSERT_NE(nullptr, engine);

  EXPECT_EQ(0u, engine->SceneCount());

  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(Grid3DTest, CRUD)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // Load Scene3D before Grid3D
  EXPECT_TRUE(loadPlugin("Scene3D"));

  // Load plugin
  const char *pluginStr =
    "<plugin filename=\"Grid3D\">"
      "<engine>ogre</engine>"
      "<scene>scene</scene>"
      "<insert>"
        "<cell_count>5</cell_count>"
        "<cell_length>3.5</cell_length>"
        "<vertical_cell_count>3</vertical_cell_count>"
        "<pose>1 0 0 0 0 0</pose>"
        "<color>0 0 1 1</color>"
      "</insert>"
    "</plugin>";

  tinyxml2::XMLDocument pluginDoc;
  pluginDoc.Parse(pluginStr);
  EXPECT_TRUE(ignition::gui::loadPlugin("Grid3D",
      pluginDoc.FirstChildElement("plugin")));

  // Create main window
  EXPECT_TRUE(createMainWindow());

  // Check scene
  auto engine = rendering::engine("ogre");
  ASSERT_NE(nullptr, engine);

  auto scene = engine->SceneByName("scene");
  ASSERT_NE(nullptr, scene);

  // 2 children: camera and grid
  EXPECT_EQ(1u, scene->VisualCount());
  EXPECT_EQ(1u, scene->VisualByIndex(0)->GeometryCount());

  // Check grid
  auto grid = std::dynamic_pointer_cast<rendering::Grid>(
      scene->VisualByIndex(0)->GeometryByIndex(0));
  ASSERT_NE(nullptr, grid);

  EXPECT_EQ(5u, grid->CellCount());
  EXPECT_EQ(3u, grid->VerticalCellCount());
  EXPECT_DOUBLE_EQ(3.5, grid->CellLength());
  EXPECT_EQ(math::Pose3d(1, 0, 0, 0, 0, 0), grid->Parent()->WorldPose());
  EXPECT_EQ(math::Color::Blue, grid->Material()->Ambient());

  // Edit grid
  auto win = mainWindow();
  ASSERT_NE(nullptr, win);

  // Cell count
  {
    auto name = QString::fromStdString("cellCountWidget");
    auto w = win->findChild<NumberWidget *>(name);
    ASSERT_NE(nullptr, w);
    EXPECT_EQ(5, w->Value().toInt());
    w->ValueChanged(10);
    QCoreApplication::processEvents();
    EXPECT_EQ(10u, grid->CellCount());
  }

  // Vertical cell count
  {
    auto name = QString::fromStdString("vertCellCountWidget");
    auto w = win->findChild<NumberWidget *>(name);
    ASSERT_NE(nullptr, w);
    EXPECT_EQ(3, w->Value().toInt());
    w->ValueChanged(8);
    QCoreApplication::processEvents();
    EXPECT_EQ(8u, grid->VerticalCellCount());
  }

  // Cell length
  {
    auto name = QString::fromStdString("cellLengthWidget");
    auto w = win->findChild<NumberWidget *>(name);
    ASSERT_NE(nullptr, w);
    EXPECT_DOUBLE_EQ(3.5, w->Value().toDouble());
    w->ValueChanged(0.2);
    QCoreApplication::processEvents();
    EXPECT_DOUBLE_EQ(0.2, grid->CellLength());
  }

  // Pose
  {
    auto name = QString::fromStdString("poseWidget");
    auto w = win->findChild<Pose3dWidget *>(name);
    ASSERT_NE(nullptr, w);
    EXPECT_EQ(math::Pose3d(1, 0, 0, 0, 0, 0), w->Value().value<math::Pose3d>());
    w->ValueChanged(QVariant::fromValue(math::Pose3d(0, 0, 1, 0, 0, 0)));
    QCoreApplication::processEvents();
    EXPECT_EQ(math::Pose3d(0, 0, 1, 0, 0, 0), grid->Parent()->WorldPose());
  }

  // Color
  {
    auto name = QString::fromStdString("colorWidget");
    auto w = win->findChild<ColorWidget *>(name);
    ASSERT_NE(nullptr, w);
    EXPECT_EQ(math::Color::Blue, w->Value().value<math::Color>());
    w->ValueChanged(QVariant::fromValue(math::Color::Cyan));
    QCoreApplication::processEvents();
    EXPECT_EQ(math::Color::Cyan, grid->Material()->Ambient());
  }

  // Add a new grid via GUI
  {
    // Click button
    auto addButton = win->findChild<QPushButton *>("addGridButton");
    ASSERT_NE(nullptr, addButton);
    addButton->click();

    // Check there is a new grid
    EXPECT_EQ(2u, scene->VisualCount());

    // Check the old grid was moved to index 1
    EXPECT_EQ(grid, scene->VisualByIndex(1)->GeometryByIndex(0));

    // Check new grid has the default values
    EXPECT_EQ(1u, scene->VisualByIndex(0)->GeometryCount());
    auto grid2 = std::dynamic_pointer_cast<rendering::Grid>(
        scene->VisualByIndex(0)->GeometryByIndex(0));
    ASSERT_NE(nullptr, grid2);

    EXPECT_EQ(20u, grid2->CellCount());
    EXPECT_EQ(0u, grid2->VerticalCellCount());
    EXPECT_DOUBLE_EQ(1.0, grid2->CellLength());
    EXPECT_EQ(math::Pose3d::Zero, grid2->Parent()->WorldPose());
    EXPECT_EQ(math::Color(0.7, 0.7, 0.7, 1.0), grid2->Material()->Ambient());
  }

  // Add a new grid programmatically and refresh
  {
    // Add grid
    auto grid3 = scene->CreateGrid();
    grid3->SetCellCount(66);
    grid3->SetVerticalCellCount(8);
    grid3->SetCellLength(2.6);

    auto grid3Vis = scene->CreateVisual();
    scene->RootVisual()->AddChild(grid3Vis);
    grid3Vis->SetLocalPose(math::Pose3d(1, 2, 3, 0, 0, 0));
    grid3Vis->AddGeometry(grid3);

    auto mat = scene->CreateMaterial();
    mat->SetAmbient(math::Color(0.1, 0.2, 0.3));
    grid3Vis->SetMaterial(mat);

    EXPECT_EQ(3u, scene->VisualCount());

    // Click button
    auto refreshButton = win->findChild<QPushButton *>("refreshGridButton");
    ASSERT_NE(nullptr, refreshButton);
    refreshButton->click();

    // Check we now have 3 collapsibles
    auto collapsibles = win->findChildren<CollapsibleWidget *>();
    EXPECT_EQ(3, collapsibles.size());

    auto gridName = QVariant(QString::fromStdString(grid3->Name()));

    // Cell count
    {
      auto name = QString::fromStdString("cellCountWidget");
      auto w = findFirstByProperty(win->findChildren<NumberWidget *>(name),
          "gridName", gridName);
      ASSERT_NE(nullptr, w);
      EXPECT_EQ(66, w->Value().toInt());
    }

    // Vertical cell count
    {
      auto name = QString::fromStdString(
          "vertCellCountWidget");
      auto w = findFirstByProperty(win->findChildren<NumberWidget *>(name),
          "gridName", gridName);
      ASSERT_NE(nullptr, w);
      EXPECT_EQ(8, w->Value().toInt());
    }

    // Cell length
    {
      auto name = QString::fromStdString("cellLengthWidget");
      auto w = findFirstByProperty(win->findChildren<NumberWidget *>(name),
          "gridName", gridName);
      ASSERT_NE(nullptr, w);
      EXPECT_DOUBLE_EQ(2.6, w->Value().toDouble());
    }

    // Pose
    {
      auto name = QString::fromStdString("poseWidget");
      auto w = findFirstByProperty(win->findChildren<Pose3dWidget *>(name),
          "gridName", gridName);
      ASSERT_NE(nullptr, w);
      EXPECT_EQ(math::Pose3d(1, 2, 3, 0, 0, 0),
                w->Value().value<math::Pose3d>());
    }

    // Color
    {
      auto name = QString::fromStdString("colorWidget");
      auto w = findFirstByProperty(win->findChildren<ColorWidget *>(name),
          "gridName", gridName);
      ASSERT_NE(nullptr, w);
      EXPECT_EQ(math::Color(0.1, 0.2, 0.3), w->Value().value<math::Color>());
    }
  }

  // Delete a grid
  {
    auto gridName = QVariant(QString::fromStdString(grid->Name()));
    auto name = QString::fromStdString("deleteButton");
    auto w = findFirstByProperty(win->findChildren<QPushButton *>(name),
        "gridName", gridName);
    ASSERT_NE(nullptr, w);

    w->click();

    // Check scene
    EXPECT_EQ(2u, scene->VisualCount());

    // Check widgets
    EXPECT_EQ(2, win->findChildren<CollapsibleWidget *>().size());
  }

  EXPECT_TRUE(stop());
}

