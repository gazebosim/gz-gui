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

namespace ignition
{
namespace gui
{
namespace plugins
{
  struct GridInfo
  {
    int cellCount{20};
    int vertCellCount{0};
    double cellLength{1};
    math::Pose3d pose;
    math::Color color = math::Color(0.7, 0.7, 0.7);
  };

  class Grid3DPrivate
  {
    /// \brief Pointer to scene
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
  bool autoClose = false;
  std::vector<GridInfo> grids;
  if (_pluginElem)
  {
    // All grids managed belong to the same engine and scene
    if (auto elem = _pluginElem->FirstChildElement("engine"))
      engineName = elem->GetText();

    if (auto elem = _pluginElem->FirstChildElement("scene"))
      sceneName = elem->GetText();

    if (_pluginElem->Attribute("auto_close"))
      _pluginElem->QueryBoolAttribute("auto_close", &autoClose);

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
    ignerr << "Engine [" << engineName << "] is not supported" << std::endl;
    return;
  }

  // Scene
  auto scene = engine->SceneByName(sceneName);
  if (!scene)
  {
    ignerr << "Scene [" << sceneName << "] not found" << std::endl;
    return;
  }
  auto root = scene->RootVisual();

  // Initial grids
  for (const auto &g : grids)
  {
    auto grid = scene->CreateGrid();
    grid->SetCellCount(g.cellCount);
    grid->SetVerticalCellCount(g.vertCellCount);
    grid->SetCellLength(g.cellLength);
    grid->SetVerticalCellCount(g.vertCellCount);

    auto gridVis = scene->CreateVisual();
    root->AddChild(gridVis);
    gridVis->SetLocalPose(g.pose);
    gridVis->AddGeometry(grid);

    auto gray = scene->CreateMaterial();
    gray->SetAmbient(g.color);
    gridVis->SetMaterial(gray);
  }

  // Auto-close
  if (autoClose)
  {
    QTimer::singleShot(100, [this]()
        {
          if (!this->parent())
          {
            ignerr << "Failed to autoclose plugin" << std::endl;
            return;
          }

          qobject_cast<QWidget *>(this->parent())->close();
        });

    return;
  }

  // Populate widgets
  auto mainLayout = new QVBoxLayout();
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->setSpacing(0);
  this->setLayout(mainLayout);

  for (unsigned int i = 0; i < scene->VisualCount(); ++i)
  {
    auto vis = scene->VisualByIndex(i);
    if (!vis || vis->GeometryCount() == 0)
      continue;

    rendering::GridPtr grid;
    for (unsigned int j = 0; j < vis->GeometryCount(); ++j)
    {
      grid = std::dynamic_pointer_cast<rendering::Grid>(vis->GeometryByIndex(i));
      if (grid)
        break;
    }
    if (!grid)
      continue;

    this->dataPtr->grids.push_back(grid);

    auto cellCountWidget = new NumberWidget("Horizontal cell count", NumberType::INT);
    cellCountWidget->SetValue(QVariant::fromValue(grid->CellCount()));
    cellCountWidget->setObjectName(QString::fromStdString(grid->Name() + "::cellCountWidget"));
    this->connect(cellCountWidget, SIGNAL(ValueChanged(QVariant)), this,
        SLOT(OnChange(QVariant)));

    auto vertCellCountWidget = new NumberWidget("Vertical cell count", NumberType::INT);
    vertCellCountWidget->SetValue(QVariant::fromValue(grid->VerticalCellCount()));
    vertCellCountWidget->setObjectName(QString::fromStdString(grid->Name() + "::vertCellCountWidget"));
    this->connect(vertCellCountWidget, SIGNAL(ValueChanged(QVariant)), this,
        SLOT(OnChange(QVariant)));

    auto cellLengthWidget = new NumberWidget("Cell length", NumberType::DOUBLE);
    cellLengthWidget->SetValue(QVariant::fromValue(grid->CellLength()));
    cellLengthWidget->setObjectName(QString::fromStdString(grid->Name() + "::cellLengthWidget"));
    this->connect(cellLengthWidget, SIGNAL(ValueChanged(QVariant)), this,
        SLOT(OnChange(QVariant)));

    auto poseWidget = new Pose3dWidget();
    poseWidget->SetValue(QVariant::fromValue(grid->Parent()->WorldPose()));
    poseWidget->setObjectName(QString::fromStdString(grid->Name() + "::poseWidget"));
    this->connect(poseWidget, SIGNAL(ValueChanged(QVariant)), this,
        SLOT(OnChange(QVariant)));

    auto colorWidget = new ColorWidget();
    colorWidget->SetValue(QVariant::fromValue(grid->Material()->Ambient()));
    colorWidget->setObjectName(QString::fromStdString(grid->Name() + "::colorWidget"));
    this->connect(colorWidget, SIGNAL(ValueChanged(QVariant)), this,
        SLOT(OnChange(QVariant)));

    auto collapsible = new CollapsibleWidget(grid->Name());
    collapsible->layout()->addWidget(cellCountWidget);
    collapsible->layout()->addWidget(vertCellCountWidget);
    collapsible->layout()->addWidget(cellLengthWidget);
    collapsible->layout()->addWidget(poseWidget);
    collapsible->layout()->addWidget(colorWidget);

    mainLayout->addWidget(collapsible);
  }

  auto spacer = new QWidget();
  spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  mainLayout->addWidget(spacer);
}

/////////////////////////////////////////////////
void Grid3D::OnChange(const QVariant &_value)
{
  auto parts = this->sender()->objectName().split("::");
  if (parts.size() != 3)
    return;

  std::string gridName{(parts[0] + "::" + parts[1]).toStdString()};
  for (auto grid : this->dataPtr->grids)
  {
    if (grid->Name() != gridName)
      continue;

    if (parts[2] == "cellCountWidget")
      grid->SetCellCount(_value.toInt());
    else if (parts[2] == "vertCellCountWidget")
      grid->SetVerticalCellCount(_value.toInt());
    else if (parts[2] == "cellLengthWidget")
      grid->SetCellLength(_value.toInt());
    else if (parts[2] == "poseWidget")
      grid->Parent()->SetWorldPose(_value.value<math::Pose3d>());
    else if (parts[2] == "colorWidget")
      grid->Material()->SetAmbient(_value.value<math::Color>());

    break;
  }
}

// Register this plugin
IGN_COMMON_REGISTER_SINGLE_PLUGIN(ignition::gui::plugins::Grid3D,
                                  ignition::gui::Plugin)

