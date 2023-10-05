/*
 * Copyright (C) 2020 Open Source Robotics Foundation
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

#include <string>
#include <vector>

#include <gz/common/Console.hh>
#include <gz/gui/Application.hh>
#include <gz/gui/Conversions.hh>
#include <gz/gui/GuiEvents.hh>
#include <gz/gui/MainWindow.hh>
#include <gz/plugin/Register.hh>
#include <gz/math/Color.hh>
#include <gz/math/Pose3.hh>
#include <gz/rendering/Grid.hh>
#include <gz/rendering/RenderingIface.hh>
#include <gz/rendering/Scene.hh>

#include "GridConfig.hh"

namespace gz::gui::plugins
{
struct GridParam
{
  /// \brief Horizontal cell count
  int hCellCount{20};

  /// \brief Vertical cell count
  int vCellCount{0};

  /// \brief Cell length
  double cellLength{1.0};

  /// \brief 3D pose
  math::Pose3d pose{math::Pose3d::Zero};

  /// \brief Grid color
  math::Color color{math::Color(0.7f, 0.7f, 0.7f, 1.0f)};
};

class GridConfigPrivate
{
  /// \brief List of grid names.
  public: QStringList nameList;

  /// \brief
  std::string name;

  /// \brief Grid parameters
  public: GridParam gridParam;

  /// \brief Grids to add at startup
  public: std::vector<GridParam> startupGrids;

  /// \brief Pointer to selected grid
  rendering::GridPtr grid{nullptr};

  /// \brief Pointer to scene
  rendering::ScenePtr scene{nullptr};

  /// \brief Flag that indicates whether there are new updates to be rendered.
  public: bool dirty{false};

  /// \brief True if name list needs to be refreshed.
  public: bool refreshList{true};

  /// \brief Visible state
  bool visible{true};
};

/////////////////////////////////////////////////
GridConfig::GridConfig()
  : gz::gui::Plugin(), dataPtr(std::make_unique<GridConfigPrivate>())
{
}

/////////////////////////////////////////////////
GridConfig::~GridConfig() = default;

/////////////////////////////////////////////////
void GridConfig::LoadConfig(const tinyxml2::XMLElement *_pluginElem)
{
  if (this->title.empty())
    this->title = "Grid config";

  // Configuration
  if (_pluginElem)
  {
    // For grids to be inserted at startup
    for (auto insertElem = _pluginElem->FirstChildElement("insert");
         insertElem != nullptr;
         insertElem = insertElem->NextSiblingElement("insert"))
    {
      GridParam gridParam;

      // Both cell_count and horizontal_cell_count apply to horizontal for
      // backwards compatibility
      if (auto cellCountElem = insertElem->FirstChildElement("cell_count"))
        cellCountElem->QueryIntText(&gridParam.hCellCount);

      if (auto cellCountElem = insertElem->FirstChildElement(
          "horizontal_cell_count"))
      {
        cellCountElem->QueryIntText(&gridParam.hCellCount);
      }

      if (auto vElem = insertElem->FirstChildElement("vertical_cell_count"))
        vElem->QueryIntText(&gridParam.vCellCount);

      if (auto lengthElem = insertElem->FirstChildElement("cell_length"))
        lengthElem->QueryDoubleText(&gridParam.cellLength);

      auto elem = insertElem->FirstChildElement("pose");
      if (nullptr != elem && nullptr != elem->GetText())
      {
        std::stringstream poseStr;
        poseStr << std::string(elem->GetText());
        poseStr >> gridParam.pose;
      }

      elem = insertElem->FirstChildElement("color");
      if (nullptr != elem && nullptr != elem->GetText())
      {
        std::stringstream colorStr;
        colorStr << std::string(elem->GetText());
        colorStr >> gridParam.color;
      }

      this->dataPtr->startupGrids.push_back(gridParam);
    }
  }

  gui::App()->findChild<
      MainWindow *>()->installEventFilter(this);
}

/////////////////////////////////////////////////
bool GridConfig::eventFilter(QObject *_obj, QEvent *_event)
{
  if (_event->type() == events::Render::kType)
  {
    if (nullptr == this->dataPtr->scene)
      this->dataPtr->scene = rendering::sceneFromFirstRenderEngine();

    if (nullptr != this->dataPtr->scene)
    {
      // Create grid setup at startup
      this->CreateGrids();

      // Update combo box
      this->RefreshList();

      // Update selected grid
      this->UpdateGrid();
    }
  }

  // Standard event processing
  return QObject::eventFilter(_obj, _event);
}

/////////////////////////////////////////////////
void GridConfig::CreateGrids()
{
  if (this->dataPtr->startupGrids.empty())
    return;

  for (const auto &gridParam : this->dataPtr->startupGrids)
  {
    auto grid = this->dataPtr->scene->CreateGrid();
    grid->SetCellCount(gridParam.hCellCount);
    grid->SetVerticalCellCount(gridParam.vCellCount);
    grid->SetCellLength(gridParam.cellLength);

    auto gridVis = this->dataPtr->scene->CreateVisual();
    this->dataPtr->scene->RootVisual()->AddChild(gridVis);
    gridVis->SetLocalPose(gridParam.pose);
    gridVis->AddGeometry(grid);

    auto mat = this->dataPtr->scene->CreateMaterial();
    mat->SetAmbient(gridParam.color);
    mat->SetDiffuse(gridParam.color);
    mat->SetSpecular(gridParam.color);
    gridVis->SetMaterial(mat);

    this->dataPtr->dirty = true;

    gzdbg << "Created grid [" << grid->Name() << "]" << std::endl;
  }
  this->dataPtr->startupGrids.clear();
}

/////////////////////////////////////////////////
void GridConfig::UpdateGrid()
{
  // Connect to a grid
  if (!this->dataPtr->grid)
    this->ConnectToGrid();

  // If not connected, don't update
  if (!this->dataPtr->grid)
    return;

  if (!this->dataPtr->dirty)
    return;

  this->dataPtr->grid->SetVerticalCellCount(
    this->dataPtr->gridParam.vCellCount);
  this->dataPtr->grid->SetCellCount(
    this->dataPtr->gridParam.hCellCount);
  this->dataPtr->grid->SetCellLength(
    this->dataPtr->gridParam.cellLength);

  auto visual = this->dataPtr->grid->Parent();
  if (visual)
  {
    visual->SetLocalPose(this->dataPtr->gridParam.pose);

    auto mat = visual->Material();
    if (mat)
    {
      mat->SetAmbient(this->dataPtr->gridParam.color);
      mat->SetDiffuse(this->dataPtr->gridParam.color);
      mat->SetSpecular(this->dataPtr->gridParam.color);
    }
    else
    {
      gzerr << "Grid visual missing material" << std::endl;
    }

    visual->SetVisible(this->dataPtr->visible);
  }
  else
  {
    gzerr << "Grid missing parent visual" << std::endl;
  }

  this->dataPtr->dirty = false;
}

/////////////////////////////////////////////////
void GridConfig::ConnectToGrid()
{
  if (this->dataPtr->name.empty())
    return;

  if (this->dataPtr->grid)
    return;

  for (unsigned int i = 0; i < this->dataPtr->scene->VisualCount(); ++i)
  {
    auto vis = this->dataPtr->scene->VisualByIndex(i);
    if (!vis || vis->GeometryCount() == 0)
      continue;
    for (unsigned int j = 0; j < vis->GeometryCount(); ++j)
    {
      auto grid = std::dynamic_pointer_cast<rendering::Grid>(
            vis->GeometryByIndex(j));
      if (grid && grid->Name() == this->dataPtr->name)
      {
        this->dataPtr->grid = grid;

        gzdbg << "Connected to grid [" << grid->Name() << "]" << std::endl;

        // TODO(chapulina) Set to the grid's visible state when that's available
        // through gz-rendering's API
        this->dataPtr->visible = true;
        grid->Parent()->SetVisible(true);

        this->dataPtr->gridParam.hCellCount = grid->CellCount();
        this->dataPtr->gridParam.vCellCount = grid->VerticalCellCount();
        this->dataPtr->gridParam.cellLength = grid->CellLength();
        this->dataPtr->gridParam.pose = grid->Parent()->LocalPose();
        this->dataPtr->gridParam.color = grid->Parent()->Material()->Ambient();
        this->newParams(
            grid->CellCount(),
            grid->VerticalCellCount(),
            grid->CellLength(),
            convert(grid->Parent()->LocalPose().Pos()),
            convert(grid->Parent()->LocalPose().Rot().Euler()),
            convert(grid->Parent()->Material()->Ambient()));
      }
    }
  }
}

/////////////////////////////////////////////////
void GridConfig::OnName(const QString &_name)
{
  this->dataPtr->name = _name.toStdString();

  // Set it to null so we load the new grid
  this->dataPtr->grid = nullptr;

  // Don't change the grid we're about to connected to
  this->dataPtr->dirty = false;
}

/////////////////////////////////////////////////
QStringList GridConfig::NameList() const
{
  return this->dataPtr->nameList;
}

/////////////////////////////////////////////////
void GridConfig::SetNameList(const QStringList &_nameList)
{
  this->dataPtr->nameList = _nameList;
  this->NameListChanged();
}

/////////////////////////////////////////////////
void GridConfig::UpdateVCellCount(int _cellCount)
{
  this->dataPtr->gridParam.vCellCount = _cellCount;
  this->dataPtr->dirty = true;
}

/////////////////////////////////////////////////
void GridConfig::UpdateHCellCount(int _cellCount)
{
  this->dataPtr->gridParam.hCellCount = _cellCount;
  this->dataPtr->dirty = true;
}

/////////////////////////////////////////////////
void GridConfig::UpdateCellLength(double _length)
{
  this->dataPtr->gridParam.cellLength = _length;
  this->dataPtr->dirty = true;
}

/////////////////////////////////////////////////
void GridConfig::SetPose(
  double _x, double _y, double _z,
  double _roll, double _pitch, double _yaw)
{
  this->dataPtr->gridParam.pose = math::Pose3d(_x, _y, _z, _roll, _pitch, _yaw);
  this->dataPtr->dirty = true;
}

/////////////////////////////////////////////////
void GridConfig::SetColor(double _r, double _g, double _b, double _a)
{
  this->dataPtr->gridParam.color = math::Color(_r, _g, _b, _a);
  this->dataPtr->dirty = true;
}

/////////////////////////////////////////////////
void GridConfig::OnShow(bool _checked)
{
  this->dataPtr->visible = _checked;
  this->dataPtr->dirty = true;
}

/////////////////////////////////////////////////
void GridConfig::OnRefresh()
{
  this->dataPtr->refreshList = true;
}

/////////////////////////////////////////////////
void GridConfig::RefreshList()
{
  if (!this->dataPtr->refreshList)
    return;
  this->dataPtr->refreshList = false;

  // Clear
  this->dataPtr->nameList.clear();

  // Get updated list
  for (unsigned int i = 0; i < this->dataPtr->scene->VisualCount(); ++i)
  {
    auto vis = this->dataPtr->scene->VisualByIndex(i);
    if (!vis || vis->GeometryCount() == 0)
      continue;
    for (unsigned int j = 0; j < vis->GeometryCount(); ++j)
    {
      auto grid = std::dynamic_pointer_cast<rendering::Grid>(
            vis->GeometryByIndex(j));
      if (grid)
      {
        this->dataPtr->nameList.push_back(QString::fromStdString(grid->Name()));
      }
    }
  }

  // Select first one
  if (this->dataPtr->nameList.count() > 0)
    this->OnName(this->dataPtr->nameList.at(0));
  this->NameListChanged();
}
}  // namespace gz::gui::plugins

// Register this plugin
GZ_ADD_PLUGIN(gz::gui::plugins::GridConfig,
              gz::gui::Plugin)
