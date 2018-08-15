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

#include <sstream>
#include <string>

#include <ignition/common/Console.hh>
#include <ignition/common/PluginMacros.hh>
#include <ignition/math/Color.hh>
#include <ignition/math/Pose3.hh>
#include <ignition/rendering.hh>

#include "ignition/gui/ColorWidget.hh"
#include "ignition/gui/NumberWidget.hh"
#include "ignition/gui/Pose3dWidget.hh"
#include "ignition/gui/QtMetatypes.hh"
#include "ignition/gui/display_plugins/GridDisplay.hh"

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
namespace display_plugins
{
  /// \brief Holds configuration for a grid
  struct GridDisplayInfo
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

  class GridDisplayPrivate
  {
    /// \brief Keep track of this grid.
    public: rendering::GridPtr grid = nullptr;
  };
}
}
}

using namespace ignition;
using namespace gui;
using namespace display_plugins;

/////////////////////////////////////////////////
GridDisplay::GridDisplay()
  : DisplayPlugin(), dataPtr(new GridDisplayPrivate)
{
  this->title = "Grid";
}

/////////////////////////////////////////////////
GridDisplay::~GridDisplay()
{
}

/////////////////////////////////////////////////
void GridDisplay::Initialize(const tinyxml2::XMLElement *_pluginElem)
{
  // Configuration
  GridDisplayInfo gridInfo;
  if (_pluginElem)
  {
    if (auto elem = _pluginElem->FirstChildElement("cell_count"))
      elem->QueryIntText(&gridInfo.cellCount);

    if (auto elem = _pluginElem->FirstChildElement("vertical_cell_count"))
      elem->QueryIntText(&gridInfo.vertCellCount);

    if (auto elem = _pluginElem->FirstChildElement("cell_length"))
      elem->QueryDoubleText(&gridInfo.cellLength);

    if (auto elem = _pluginElem->FirstChildElement("pose"))
    {
      std::stringstream poseStr;
      poseStr << std::string(elem->GetText());
      poseStr >> gridInfo.pose;
    }

    if (auto elem = _pluginElem->FirstChildElement("color"))
    {
      std::stringstream colorStr;
      colorStr << std::string(elem->GetText());
      colorStr >> gridInfo.color;
    }
  }

  ignition::rendering::MaterialPtr mat;
  if (auto scenePtr = this->Scene().lock())
  {
    this->dataPtr->grid = scenePtr->CreateGrid();
    mat = scenePtr->CreateMaterial();
  }
  else
  {
    ignerr << "Scene invalid. Grid display not initialized." << std::endl;
    return;
  }
  this->dataPtr->grid->SetCellCount(gridInfo.cellCount);
  this->dataPtr->grid->SetVerticalCellCount(gridInfo.vertCellCount);
  this->dataPtr->grid->SetCellLength(gridInfo.cellLength);

  this->Visual()->SetLocalPose(gridInfo.pose);
  this->Visual()->AddGeometry(this->dataPtr->grid);

  mat->SetAmbient(gridInfo.color);
  this->Visual()->SetMaterial(mat);
}

/////////////////////////////////////////////////
QWidget *GridDisplay::CreateCustomProperties() const
{
  if (nullptr == this->dataPtr->grid)
  {
    return nullptr;
  }
  auto cellCountWidget = new NumberWidget("Horizontal cell count",
      NumberType::UINT);
  cellCountWidget->SetValue(
    QVariant::fromValue(this->dataPtr->grid->CellCount()));
  cellCountWidget->setObjectName("cellCountWidget");
  this->connect(cellCountWidget, SIGNAL(ValueChanged(QVariant)), this,
      SLOT(OnChange(QVariant)));

  auto vertCellCountWidget = new NumberWidget("Vertical cell count",
      NumberType::UINT);
  vertCellCountWidget->SetValue(
      QVariant::fromValue(this->dataPtr->grid->VerticalCellCount()));
  vertCellCountWidget->setObjectName("vertCellCountWidget");
  this->connect(vertCellCountWidget, SIGNAL(ValueChanged(QVariant)), this,
      SLOT(OnChange(QVariant)));

  auto cellLengthWidget = new NumberWidget("Cell length", NumberType::DOUBLE);
  cellLengthWidget->SetValue(
    QVariant::fromValue(this->dataPtr->grid->CellLength()));
  cellLengthWidget->setObjectName("cellLengthWidget");
  this->connect(cellLengthWidget, SIGNAL(ValueChanged(QVariant)), this,
      SLOT(OnChange(QVariant)));

  auto poseWidget = new Pose3dWidget();
  poseWidget->SetValue(
    QVariant::fromValue(this->dataPtr->grid->Parent()->WorldPose()));
  poseWidget->setObjectName("poseWidget");
  this->connect(poseWidget, SIGNAL(ValueChanged(QVariant)), this,
      SLOT(OnChange(QVariant)));

  auto colorWidget = new ColorWidget();
  colorWidget->SetValue(
    QVariant::fromValue(this->dataPtr->grid->Material()->Ambient()));
  colorWidget->setObjectName("colorWidget");
  this->connect(colorWidget, SIGNAL(ValueChanged(QVariant)), this,
      SLOT(OnChange(QVariant)));

  auto layout = new QVBoxLayout();
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);
  layout->addWidget(cellCountWidget);
  layout->addWidget(vertCellCountWidget);
  layout->addWidget(cellLengthWidget);
  layout->addWidget(colorWidget);
  layout->addWidget(poseWidget);
  auto widget = new QWidget();
  widget->setLayout(layout);

  return widget;
}

/////////////////////////////////////////////////
void GridDisplay::OnChange(const QVariant &_value)
{
  if (nullptr == this->dataPtr->grid)
  {
    return;
  }
  auto type = this->sender()->objectName().toStdString();

  if (type == "cellCountWidget")
    this->dataPtr->grid->SetCellCount(_value.toInt());
  else if (type == "vertCellCountWidget")
    this->dataPtr->grid->SetVerticalCellCount(_value.toInt());
  else if (type == "cellLengthWidget")
    this->dataPtr->grid->SetCellLength(_value.toDouble());
  else if (type == "poseWidget")
    this->dataPtr->grid->Parent()->SetWorldPose(_value.value<math::Pose3d>());
  else if (type == "colorWidget")
    this->dataPtr->grid->Material()->SetAmbient(_value.value<math::Color>());
}

/////////////////////////////////////////////////
tinyxml2::XMLElement * GridDisplay::CustomConfig(tinyxml2::XMLDocument *_doc)
  const
{
  auto customConfigElem = _doc->NewElement("config");
  if (nullptr == this->dataPtr->grid)
  {
    // The properties can't be retirieved from the grid.
    // TODO(dhood): return the initial properties specified in this case.
    return customConfigElem;
  }

  // Cell count
  auto cellCountElem = _doc->NewElement("cell_count");
  cellCountElem->SetText(
    std::to_string(this->dataPtr->grid->CellCount()).c_str());
  customConfigElem->InsertEndChild(cellCountElem);

  // Vertical cell count
  auto verticalCellCountElem = _doc->NewElement("vertical_cell_count");
  verticalCellCountElem->SetText(
    std::to_string(this->dataPtr->grid->VerticalCellCount()).c_str());
  customConfigElem->InsertEndChild(verticalCellCountElem);

  // Cell length
  auto cellLengthElem = _doc->NewElement("cell_length");
  cellLengthElem->SetText(
    std::to_string(this->dataPtr->grid->CellLength()).c_str());
  customConfigElem->InsertEndChild(cellLengthElem);

  // Pose
  auto poseElem = _doc->NewElement("pose");
  std::stringstream poseStr;
  poseStr << this->Visual()->LocalPose();
  poseElem->SetText(poseStr.str().c_str());
  customConfigElem->InsertEndChild(poseElem);

  // Color
  auto colorElem = _doc->NewElement("color");
  std::stringstream colorStr;
  colorStr << this->dataPtr->grid->Material()->Ambient();
  colorElem->SetText(colorStr.str().c_str());
  customConfigElem->InsertEndChild(colorElem);

  return customConfigElem;
}

// Register this plugin
IGN_COMMON_REGISTER_SINGLE_PLUGIN(ignition::gui::display_plugins::GridDisplay,
                                  ignition::gui::DisplayPlugin)

