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

#include <ignition/common/Console.hh>
#include <ignition/common/PluginMacros.hh>
#include <ignition/rendering.hh>

#include "ignition/gui/CollapsibleWidget.hh"
#include "ignition/gui/ColorWidget.hh"
#include "ignition/gui/NumberWidget.hh"
#include "ignition/gui/Pose3dWidget.hh"
#include "ignition/gui/QtMetatypes.hh"
#include "ignition/gui/plugins/Grid3D.hh"

// Default cell count
static const int kDefaultCellCount{20};

// Default vertical cell count
static const int kDefaultVertCellCount{0};

// Default cell length
static const double kDefaultCellLength{1.0};

// Default pose
static const ignition::math::Pose3d kDefaultPose{ignition::math::Pose3d::Zero};

// Default color
static const ignition::math::Color kDefaultColor{
    ignition::math::Color(0.7, 0.7, 0.7, 1.0)};

namespace ignition
{
namespace gui
{
namespace plugins
{
  /// \brief Holds configuration for a grid
  struct GridInfo
  {
    /// \brief Number of cells in the horizontal
    int cellCount{kDefaultCellCount};

    /// \brief Number of cells in the vertical
    int vertCellCount{kDefaultVertCellCount};

    /// \brief Cell length, both horizontal and vertical
    double cellLength{kDefaultCellLength};

    /// \brief Grid pose in the world
    math::Pose3d pose{kDefaultPose};

    /// \brief Grid ambient color
    math::Color color{kDefaultColor};
  };

  class Grid3DPrivate
  {
    /// \brief Pointer to scene
    public: rendering::ScenePtr scene;

    /// \brief Keep track of grids we currently found on the scene
    public: std::vector<rendering::GridPtr> grids;
  };
}
}
}

using namespace ignition;
using namespace gui;
using namespace plugins;

/////////////////////////////////////////////////
Grid3D::Grid3D()
  : Plugin(), dataPtr(new Grid3DPrivate)
{
}

/////////////////////////////////////////////////
Grid3D::~Grid3D()
{
}

/////////////////////////////////////////////////
void Grid3D::LoadConfig(const tinyxml2::XMLElement *_pluginElem)
{
  if (this->title.empty())
    this->title = "3D Grid";

  // Configuration
  std::string engineName{"ogre"};
  std::string sceneName{"scene"};
  std::vector<GridInfo> grids;
  if (_pluginElem)
  {
    // All grids managed belong to the same engine and scene
    if (auto elem = _pluginElem->FirstChildElement("engine"))
      engineName = elem->GetText();

    if (auto elem = _pluginElem->FirstChildElement("scene"))
      sceneName = elem->GetText();

    // For grids to be inserted at startup
    for (auto insertElem = _pluginElem->FirstChildElement("insert");
         insertElem != nullptr;
        insertElem = insertElem->NextSiblingElement("insert"))
    {
      GridInfo gridInfo;

      if (auto elem = insertElem->FirstChildElement("cell_count"))
        elem->QueryIntText(&gridInfo.cellCount);

      if (auto elem = insertElem->FirstChildElement("vertical_cell_count"))
        elem->QueryIntText(&gridInfo.vertCellCount);

      if (auto elem = insertElem->FirstChildElement("cell_length"))
        elem->QueryDoubleText(&gridInfo.cellLength);

      if (auto elem = insertElem->FirstChildElement("pose"))
      {
        std::stringstream poseStr;
        poseStr << std::string(elem->GetText());
        poseStr >> gridInfo.pose;
      }

      if (auto elem = insertElem->FirstChildElement("color"))
      {
        std::stringstream colorStr;
        colorStr << std::string(elem->GetText());
        colorStr >> gridInfo.color;
      }

      grids.push_back(gridInfo);
    }
  }

  // Render engine
  auto engine = rendering::engine(engineName);
  if (!engine)
  {
    ignerr << "Engine [" << engineName
           << "] is not supported, grid plugin won't work." << std::endl;
    return;
  }

  // Scene
  this->dataPtr->scene = engine->SceneByName(sceneName);
  if (!this->dataPtr->scene)
  {
    ignerr << "Scene [" << sceneName << "] not found, grid plugin won't work."
           << std::endl;
    return;
  }
  auto root = this->dataPtr->scene->RootVisual();

  // Initial grids
  for (const auto &g : grids)
  {
    auto grid = this->dataPtr->scene->CreateGrid();
    grid->SetCellCount(g.cellCount);
    grid->SetVerticalCellCount(g.vertCellCount);
    grid->SetCellLength(g.cellLength);

    auto gridVis = this->dataPtr->scene->CreateVisual();
    root->AddChild(gridVis);
    gridVis->SetLocalPose(g.pose);
    gridVis->AddGeometry(grid);

    auto mat = this->dataPtr->scene->CreateMaterial();
    mat->SetAmbient(g.color);
    gridVis->SetMaterial(mat);
  }

  // Don't waste time loading widgets if this will be deleted anyway
  if (this->deleteLaterRequested)
    return;

  this->Refresh();
}

/////////////////////////////////////////////////
void Grid3D::Refresh()
{
  auto mainLayout = this->layout();
  // Clear previous layout
  if (mainLayout)
  {
    while (mainLayout->count() != 1)
    {
      auto item = mainLayout->takeAt(1);
      if (qobject_cast<CollapsibleWidget *>(item->widget()))
      {
        delete item->widget();
        delete item;
      }
    }
  }
  // Creating layout for the first time
  else
  {
    mainLayout = new QVBoxLayout();
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    this->setLayout(mainLayout);

    auto addButton = new QPushButton("New grid");
    addButton->setObjectName("addGridButton");
    addButton->setToolTip("Add a new grid with default values");
    this->connect(addButton, SIGNAL(clicked()), this, SLOT(OnAdd()));

    auto refreshButton = new QPushButton("Refresh");
    refreshButton->setObjectName("refreshGridButton");
    refreshButton->setToolTip("Refresh the list of grids");
    this->connect(refreshButton, SIGNAL(clicked()), this, SLOT(Refresh()));

    auto buttonsLayout = new QHBoxLayout();
    buttonsLayout->addWidget(addButton);
    buttonsLayout->addWidget(refreshButton);

    auto buttonsWidget = new QWidget();
    buttonsWidget->setLayout(buttonsLayout);

    mainLayout->addWidget(buttonsWidget);
  }

  // Search for all grids currently in the scene
  for (unsigned int i = 0; i < this->dataPtr->scene->VisualCount(); ++i)
  {
    auto vis = this->dataPtr->scene->VisualByIndex(i);
    if (!vis || vis->GeometryCount() == 0)
      continue;

    rendering::GridPtr grid;
    for (unsigned int j = 0; j < vis->GeometryCount(); ++j)
    {
      grid = std::dynamic_pointer_cast<rendering::Grid>(
          vis->GeometryByIndex(j));
      if (grid)
        break;
    }
    if (!grid)
      continue;

    this->dataPtr->grids.push_back(grid);
    auto gridName = QString::fromStdString(grid->Name());

    auto cellCountWidget = new NumberWidget("Horizontal cell count",
        NumberType::UINT);
    cellCountWidget->SetValue(QVariant::fromValue(grid->CellCount()));
    cellCountWidget->setObjectName(gridName + "---cellCountWidget");
    this->connect(cellCountWidget, SIGNAL(ValueChanged(QVariant)), this,
        SLOT(OnChange(QVariant)));

    auto vertCellCountWidget = new NumberWidget("Vertical cell count",
        NumberType::UINT);
    vertCellCountWidget->SetValue(
        QVariant::fromValue(grid->VerticalCellCount()));
    vertCellCountWidget->setObjectName(gridName + "---vertCellCountWidget");
    this->connect(vertCellCountWidget, SIGNAL(ValueChanged(QVariant)), this,
        SLOT(OnChange(QVariant)));

    auto cellLengthWidget = new NumberWidget("Cell length", NumberType::DOUBLE);
    cellLengthWidget->SetValue(QVariant::fromValue(grid->CellLength()));
    cellLengthWidget->setObjectName(gridName + "---cellLengthWidget");
    this->connect(cellLengthWidget, SIGNAL(ValueChanged(QVariant)), this,
        SLOT(OnChange(QVariant)));

    auto poseWidget = new Pose3dWidget();
    poseWidget->SetValue(QVariant::fromValue(grid->Parent()->WorldPose()));
    poseWidget->setObjectName(gridName + "---poseWidget");
    this->connect(poseWidget, SIGNAL(ValueChanged(QVariant)), this,
        SLOT(OnChange(QVariant)));

    auto colorWidget = new ColorWidget();
    colorWidget->SetValue(QVariant::fromValue(grid->Material()->Ambient()));
    colorWidget->setObjectName(gridName + "---colorWidget");
    this->connect(colorWidget, SIGNAL(ValueChanged(QVariant)), this,
        SLOT(OnChange(QVariant)));

    auto deleteButton = new QPushButton("Delete grid");
    deleteButton->setToolTip("Delete grid " + gridName);
    deleteButton->setObjectName(gridName + "---deleteButton");
    this->connect(deleteButton, SIGNAL(clicked()), this, SLOT(OnDelete()));

    auto collapsible = new CollapsibleWidget(grid->Name());
    collapsible->AppendContent(cellCountWidget);
    collapsible->AppendContent(vertCellCountWidget);
    collapsible->AppendContent(cellLengthWidget);
    collapsible->AppendContent(poseWidget);
    collapsible->AppendContent(colorWidget);
    collapsible->AppendContent(deleteButton);

    mainLayout->addWidget(collapsible);
  }

  auto spacer = new QWidget();
  spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  mainLayout->addWidget(spacer);
}

/////////////////////////////////////////////////
void Grid3D::OnChange(const QVariant &_value)
{
  auto parts = this->sender()->objectName().split("---");
  if (parts.size() != 2)
    return;

  for (auto grid : this->dataPtr->grids)
  {
    if (grid->Name() != parts[0].toStdString())
      continue;

    if (parts[1] == "cellCountWidget")
      grid->SetCellCount(_value.toInt());
    else if (parts[1] == "vertCellCountWidget")
      grid->SetVerticalCellCount(_value.toInt());
    else if (parts[1] == "cellLengthWidget")
      grid->SetCellLength(_value.toDouble());
    else if (parts[1] == "poseWidget")
      grid->Parent()->SetWorldPose(_value.value<math::Pose3d>());
    else if (parts[1] == "colorWidget")
      grid->Material()->SetAmbient(_value.value<math::Color>());

    break;
  }
}

/////////////////////////////////////////////////
void Grid3D::OnDelete()
{
  auto parts = this->sender()->objectName().split("---");
  if (parts.size() != 2)
    return;

  for (auto grid : this->dataPtr->grids)
  {
    if (grid->Name() != parts[0].toStdString())
      continue;

    grid->Scene()->DestroyVisual(grid->Parent());
    this->dataPtr->grids.erase(std::remove(this->dataPtr->grids.begin(),
                                           this->dataPtr->grids.end(), grid),
                                           this->dataPtr->grids.end());

    this->Refresh();
    break;
  }
}

/////////////////////////////////////////////////
void Grid3D::OnAdd()
{
  auto root = this->dataPtr->scene->RootVisual();

  auto grid = this->dataPtr->scene->CreateGrid();
  grid->SetCellCount(kDefaultCellCount);
  grid->SetVerticalCellCount(kDefaultVertCellCount);
  grid->SetCellLength(kDefaultCellLength);

  auto gridVis = this->dataPtr->scene->CreateVisual();
  root->AddChild(gridVis);
  gridVis->SetLocalPose(kDefaultPose);
  gridVis->AddGeometry(grid);

  auto mat = this->dataPtr->scene->CreateMaterial();
  mat->SetAmbient(kDefaultColor);
  gridVis->SetMaterial(mat);

  this->Refresh();
}

// Register this plugin
IGN_COMMON_REGISTER_SINGLE_PLUGIN(ignition::gui::plugins::Grid3D,
                                  ignition::gui::Plugin)

