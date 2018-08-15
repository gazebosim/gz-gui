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
#include <ignition/common/Console.hh>
#include <ignition/rendering.hh>

#include "ignition/gui/CollapsibleWidget.hh"
#include "ignition/gui/ColorWidget.hh"
#include "ignition/gui/DisplayPlugin.hh"
#include "ignition/gui/Helpers.hh"
#include "ignition/gui/Iface.hh"
#include "ignition/gui/MainWindow.hh"
#include "ignition/gui/NumberWidget.hh"
#include "ignition/gui/Pose3dWidget.hh"
#include "ignition/gui/QtMetatypes.hh"

using namespace ignition;
using namespace gui;

/////////////////////////////////////////////////
TEST(GridDisplayTest, CRUD)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // Load Scene3D before Displays
  EXPECT_TRUE(loadPlugin("Scene3D"));

  // Load Displays plugin with a GridDisplay
  const char *pluginStr =
    "<plugin filename=\"Displays\">"
      "<engine>ogre</engine>"
      "<scene>scene</scene>"
      "<displays>"
        "<display type=\"GridDisplay\">"
          "<cell_count>5</cell_count>"
          "<cell_length>3.5</cell_length>"
          "<vertical_cell_count>3</vertical_cell_count>"
          "<pose>1 0 0 0 0 0</pose>"
          "<color>0 0 1 1</color>"
        "</display>"
      "</displays>"
    "</plugin>";

  tinyxml2::XMLDocument pluginDoc;
  pluginDoc.Parse(pluginStr);
  EXPECT_TRUE(ignition::gui::loadPlugin("Displays",
      pluginDoc.FirstChildElement("plugin")));

  // Create main window
  EXPECT_TRUE(createMainWindow());

  // Check scene
  auto engine = rendering::engine("ogre");
  ASSERT_NE(nullptr, engine);

  auto scene = engine->SceneByName("scene");
  ASSERT_NE(nullptr, scene);

  // Visual created by the base class' constructor.
  ASSERT_EQ(1u, scene->VisualCount());
  // Grid added to the visual in the GridDisplay's initialization.
  ASSERT_EQ(1u, scene->VisualByIndex(0)->GeometryCount());

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

  // Check the same state can be loaded after saving.
  auto currentConfigStr = win->CurrentWindowConfig().XMLString();
  ASSERT_FALSE(currentConfigStr.empty());
  ASSERT_NE(currentConfigStr.find("<display type=\"GridDisplay\">"),
    std::string::npos) << currentConfigStr;

  // Clear the loaded plugins.
  EXPECT_TRUE(stop());

  // Re-initialize the app.
  EXPECT_TRUE(initApp());

  tinyxml2::XMLDocument configDoc;
  configDoc.Parse(currentConfigStr.c_str());
  EXPECT_TRUE(ignition::gui::loadPlugin("Displays",
      configDoc.FirstChildElement("plugin")));

  // Recreate the main window.
  EXPECT_TRUE(createMainWindow());

  // Check the state of the re-loaded display plugin.
  {
    // Check scene
    engine = rendering::engine("ogre");
    ASSERT_NE(nullptr, engine);

    scene = engine->SceneByName("scene");
    ASSERT_NE(nullptr, scene);

    // Visual created by the base class' constructor.
    ASSERT_EQ(1u, scene->VisualCount());
    // Grid added to the visual in the GridDisplay's initialization.
    ASSERT_EQ(1u, scene->VisualByIndex(0)->GeometryCount());

    // Check grid
    grid = std::dynamic_pointer_cast<rendering::Grid>(
        scene->VisualByIndex(0)->GeometryByIndex(0));
    ASSERT_NE(nullptr, grid);

    EXPECT_EQ(10u, grid->CellCount());
    EXPECT_EQ(8u, grid->VerticalCellCount());
    EXPECT_DOUBLE_EQ(0.2, grid->CellLength());
    EXPECT_EQ(math::Pose3d(0, 0, 1, 0, 0, 0), grid->Parent()->WorldPose());
    EXPECT_EQ(math::Color::Cyan, grid->Material()->Ambient());
  }

  EXPECT_TRUE(stop());
}

