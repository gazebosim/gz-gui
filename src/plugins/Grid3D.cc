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
  class Grid3DPrivate
  {
    /// \brief Pointer to grid
    public: rendering::GridPtr grid;
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
  int cellCount{20};
  int vertCellCount{0};
  double cellLength{1};
  math::Pose3d pose;
  math::Color color(0.7, 0.7, 0.7);
  bool autoClose = false;
  if (_pluginElem)
  {
    if (_pluginElem->Attribute("auto_close"))
      _pluginElem->QueryBoolAttribute("auto_close", &autoClose);

    if (auto elem = _pluginElem->FirstChildElement("engine"))
      engineName = elem->GetText();

    if (auto elem = _pluginElem->FirstChildElement("scene"))
      sceneName = elem->GetText();

    if (auto elem = _pluginElem->FirstChildElement("cell_count"))
      elem->QueryIntText(&cellCount);

    if (auto elem = _pluginElem->FirstChildElement("vertical_cell_count"))
      elem->QueryIntText(&vertCellCount);

    if (auto elem = _pluginElem->FirstChildElement("cell_length"))
      elem->QueryDoubleText(&cellLength);

    if (auto elem = _pluginElem->FirstChildElement("pose"))
    {
      std::stringstream poseStr;
      poseStr << std::string(elem->GetText());
      poseStr >> pose;
    }

    if (auto elem = _pluginElem->FirstChildElement("color"))
    {
      std::stringstream colorStr;
      colorStr << std::string(elem->GetText());
      colorStr >> color;
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

  // Grid
  this->dataPtr->grid = scene->CreateGrid();
  this->dataPtr->grid->SetCellCount(cellCount);
  this->dataPtr->grid->SetVerticalCellCount(vertCellCount);
  this->dataPtr->grid->SetCellLength(cellLength);
  this->dataPtr->grid->SetVerticalCellCount(0);

  auto gridVis = scene->CreateVisual();
  root->AddChild(gridVis);
  gridVis->SetLocalPose(pose);
  gridVis->AddGeometry(this->dataPtr->grid);

  auto gray = scene->CreateMaterial();
  gray->SetAmbient(color);
  gridVis->SetMaterial(gray);

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

  // Widgets
  auto cellCountWidget = new NumberWidget("Horizontal cell count", NumberType::INT);
  cellCountWidget->SetValue(QVariant::fromValue(cellCount));
  cellCountWidget->setObjectName("cellCountWidget");
  this->connect(cellCountWidget, SIGNAL(ValueChanged(QVariant)), this,
      SLOT(OnChange(QVariant)));

  auto vertCellCountWidget = new NumberWidget("Vertical cell count", NumberType::INT);
  vertCellCountWidget->SetValue(QVariant::fromValue(vertCellCount));
  vertCellCountWidget->setObjectName("vertCellCountWidget");
  this->connect(vertCellCountWidget, SIGNAL(ValueChanged(QVariant)), this,
      SLOT(OnChange(QVariant)));

  auto cellLengthWidget = new NumberWidget("Cell length", NumberType::DOUBLE);
  cellLengthWidget->SetValue(QVariant::fromValue(cellLength));
  cellLengthWidget->setObjectName("cellLengthWidget");
  this->connect(cellLengthWidget, SIGNAL(ValueChanged(QVariant)), this,
      SLOT(OnChange(QVariant)));

  auto poseWidget = new Pose3dWidget();
  poseWidget->SetValue(QVariant::fromValue(pose));
  poseWidget->setObjectName("poseWidget");
  this->connect(poseWidget, SIGNAL(ValueChanged(QVariant)), this,
      SLOT(OnChange(QVariant)));

  auto colorWidget = new ColorWidget();
  colorWidget->SetValue(QVariant::fromValue(color));
  colorWidget->setObjectName("colorWidget");
  this->connect(colorWidget, SIGNAL(ValueChanged(QVariant)), this,
      SLOT(OnChange(QVariant)));

  auto spacer = new QWidget();
  spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  // Layout
  auto mainLayout = new QVBoxLayout();
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->setSpacing(0);
  mainLayout->addWidget(cellCountWidget);
  mainLayout->addWidget(vertCellCountWidget);
  mainLayout->addWidget(cellLengthWidget);
  mainLayout->addWidget(poseWidget);
  mainLayout->addWidget(colorWidget);
  mainLayout->addWidget(spacer);
  this->setLayout(mainLayout);
}

/////////////////////////////////////////////////
void Grid3D::OnChange(const QVariant &_value)
{
  if (this->sender()->objectName() == "cellCountWidget")
    this->dataPtr->grid->SetCellCount(_value.toInt());
  else if (this->sender()->objectName() == "vertCellCountWidget")
    this->dataPtr->grid->SetVerticalCellCount(_value.toInt());
  else if (this->sender()->objectName() == "cellLengthWidget")
    this->dataPtr->grid->SetCellLength(_value.toInt());
  else if (this->sender()->objectName() == "poseWidget")
    this->dataPtr->grid->Parent()->SetWorldPose(_value.value<math::Pose3d>());
  else if (this->sender()->objectName() == "colorWidget")
    this->dataPtr->grid->Material()->SetAmbient(_value.value<math::Color>());
}

// Register this plugin
IGN_COMMON_REGISTER_SINGLE_PLUGIN(ignition::gui::plugins::Grid3D,
                                  ignition::gui::Plugin)

