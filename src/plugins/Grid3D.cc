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
  /// \brief Holds configuration for an object
  struct ObjInfo
  {
    /// \brief Number of cells in the horizontal
    int cellCount{kDefaultCellCount};

    /// \brief Number of cells in the vertical
    int vertCellCount{kDefaultVertCellCount};

    /// \brief Cell length, both horizontal and vertical
    double cellLength{kDefaultCellLength};

    /// \brief Pose in the world
    math::Pose3d pose{kDefaultPose};

    /// \brief Color
    math::Color color{kDefaultColor};
  };

  class Grid3DPrivate
  {
  };
}
}
}

using namespace ignition;
using namespace gui;
using namespace plugins;

/////////////////////////////////////////////////
Grid3D::Grid3D()
  : Object3DPlugin(), dataPtr(new Grid3DPrivate)
{
}

/////////////////////////////////////////////////
Grid3D::~Grid3D()
{
}

/////////////////////////////////////////////////
void Grid3D::LoadConfig(const tinyxml2::XMLElement *_pluginElem)
{
  this->typeSingular = "grid";

  if (this->title.empty())
    this->title = "3D " + this->typeSingular;

  // Configuration
  std::string engineName{"ogre"};
  std::vector<ObjInfo> objInfos;
  if (_pluginElem)
  {
    // All objs managed belong to the same engine and scene
    if (auto elem = _pluginElem->FirstChildElement("engine"))
      engineName = elem->GetText();

    if (auto elem = _pluginElem->FirstChildElement("scene"))
      this->sceneName = elem->GetText();

    // For objs to be inserted at startup
    for (auto insertElem = _pluginElem->FirstChildElement("insert");
         insertElem != nullptr;
         insertElem = insertElem->NextSiblingElement("insert"))
    {
      ObjInfo objInfo;

      if (auto elem = insertElem->FirstChildElement("cell_count"))
        elem->QueryIntText(&objInfo.cellCount);

      if (auto elem = insertElem->FirstChildElement("vertical_cell_count"))
        elem->QueryIntText(&objInfo.vertCellCount);

      if (auto elem = insertElem->FirstChildElement("cell_length"))
        elem->QueryDoubleText(&objInfo.cellLength);

      if (auto elem = insertElem->FirstChildElement("pose"))
      {
        std::stringstream poseStr;
        poseStr << std::string(elem->GetText());
        poseStr >> objInfo.pose;
      }

      if (auto elem = insertElem->FirstChildElement("color"))
      {
        std::stringstream colorStr;
        colorStr << std::string(elem->GetText());
        colorStr >> objInfo.color;
      }

      objInfos.push_back(objInfo);
    }
  }

  std::string error{""};
  rendering::ScenePtr scene;

  // Render engine
  this->engine = rendering::engine(engineName);
  if (!this->engine)
  {
    error = "Engine \"" + engineName
           + "\" not supported, plugin won't work.";
    ignwarn << error << std::endl;
  }
  else
  {
    // Scene
    scene = this->engine->SceneByName(this->sceneName);
    if (!scene)
    {
      error = "Scene \"" + this->sceneName
             + "\" not found, plugin won't work.";
      ignwarn << error << std::endl;
    }
    else
    {
      auto root = scene->RootVisual();

      // Initial objs
      for (const auto &g : objInfos)
      {
        auto grid = scene->CreateGrid();
        grid->SetCellCount(g.cellCount);
        grid->SetVerticalCellCount(g.vertCellCount);
        grid->SetCellLength(g.cellLength);

        auto gridVis = scene->CreateVisual();
        root->AddChild(gridVis);
        gridVis->SetLocalPose(g.pose);
        gridVis->AddGeometry(grid);

        auto mat = scene->CreateMaterial();
        mat->SetAmbient(g.color);
        gridVis->SetMaterial(mat);
      }
    }
  }

  // Don't waste time loading widgets if this will be deleted anyway
  if (this->DeleteLaterRequested())
    return;

  if (!error.empty())
  {
    // Add message
    auto msg = new QLabel(QString::fromStdString(error));

    auto mainLayout = new QVBoxLayout();
    mainLayout->addWidget(msg);
    mainLayout->setAlignment(msg, Qt::AlignCenter);
    this->setLayout(mainLayout);

    return;
  }

  this->OnRefresh();
}

/////////////////////////////////////////////////
void Grid3D::Refresh()
{
  auto scene = this->engine->SceneByName(this->sceneName);
  if (!scene)
    return;

  // Search for all objs currently in the scene
  for (unsigned int i = 0; i < scene->VisualCount(); ++i)
  {
    auto vis = scene->VisualByIndex(i);
    if (!vis || vis->GeometryCount() == 0)
      continue;

    rendering::GridPtr obj;
    for (unsigned int j = 0; j < vis->GeometryCount(); ++j)
    {
      obj = std::dynamic_pointer_cast<rendering::Grid>(
          vis->GeometryByIndex(j));
      if (obj)
        break;
    }
    if (!obj)
      continue;

    // Create widgets
    std::vector<PropertyWidget *> props;
    auto objName = QString::fromStdString(obj->Name());

    auto cellCountWidget = new NumberWidget("Horizontal cell count",
        NumberType::UINT);
    cellCountWidget->SetValue(QVariant::fromValue(obj->CellCount()));
    cellCountWidget->setProperty("objName", objName);
    cellCountWidget->setObjectName("cellCountWidget");
    this->connect(cellCountWidget, SIGNAL(ValueChanged(QVariant)), this,
        SLOT(OnChange(QVariant)));
    props.push_back(cellCountWidget);

    auto vertCellCountWidget = new NumberWidget("Vertical cell count",
        NumberType::UINT);
    vertCellCountWidget->SetValue(
        QVariant::fromValue(obj->VerticalCellCount()));
    vertCellCountWidget->setProperty("objName", objName);
    vertCellCountWidget->setObjectName("vertCellCountWidget");
    this->connect(vertCellCountWidget, SIGNAL(ValueChanged(QVariant)), this,
        SLOT(OnChange(QVariant)));
    props.push_back(vertCellCountWidget);

    auto cellLengthWidget = new NumberWidget("Cell length", NumberType::DOUBLE);
    cellLengthWidget->SetValue(QVariant::fromValue(obj->CellLength()));
    cellLengthWidget->setProperty("objName", objName);
    cellLengthWidget->setObjectName("cellLengthWidget");
    this->connect(cellLengthWidget, SIGNAL(ValueChanged(QVariant)), this,
        SLOT(OnChange(QVariant)));
    props.push_back(cellLengthWidget);

    auto poseWidget = new Pose3dWidget();
    poseWidget->SetValue(QVariant::fromValue(obj->Parent()->WorldPose()));
    poseWidget->setProperty("objName", objName);
    poseWidget->setObjectName("poseWidget");
    this->connect(poseWidget, SIGNAL(ValueChanged(QVariant)), this,
        SLOT(OnChange(QVariant)));
    props.push_back(poseWidget);

    auto colorWidget = new ColorWidget();
    colorWidget->SetValue(QVariant::fromValue(obj->Material()->Ambient()));
    colorWidget->setProperty("objName", objName);
    colorWidget->setObjectName("colorWidget");
    this->connect(colorWidget, SIGNAL(ValueChanged(QVariant)), this,
        SLOT(OnChange(QVariant)));
    props.push_back(colorWidget);

    this->AppendObj(obj, props);
  }
}

/////////////////////////////////////////////////
bool Grid3D::Change(const rendering::ObjectPtr &_obj,
    const std::string &_property, const QVariant &_value)
{
  auto derived = std::dynamic_pointer_cast<rendering::Grid>(_obj);
  if (!derived)
    return false;

  if (_property == "cellCountWidget")
    derived->SetCellCount(_value.toInt());
  else if (_property == "vertCellCountWidget")
    derived->SetVerticalCellCount(_value.toInt());
  else if (_property == "cellLengthWidget")
    derived->SetCellLength(_value.toDouble());
  else if (_property == "poseWidget")
    derived->Parent()->SetWorldPose(_value.value<math::Pose3d>());
  else if (_property == "colorWidget")
    derived->Material()->SetAmbient(_value.value<math::Color>());
  else
  {
    ignwarn << "Unknown property [" << _property << std::endl;
    return false;
  }

  return true;
}

/////////////////////////////////////////////////
bool Grid3D::Delete(const rendering::ObjectPtr &_obj)
{
  auto derived = std::dynamic_pointer_cast<rendering::Grid>(_obj);
  if (!derived)
    return false;

  derived->Scene()->DestroyVisual(derived->Parent());

  return true;
}

/////////////////////////////////////////////////
bool Grid3D::Add()
{
  auto scene = this->engine->SceneByName(this->sceneName);
  if (!scene)
    return false;

  auto root = scene->RootVisual();

  auto obj = scene->CreateGrid();
  obj->SetCellCount(kDefaultCellCount);
  obj->SetVerticalCellCount(kDefaultVertCellCount);
  obj->SetCellLength(kDefaultCellLength);

  auto objVis = scene->CreateVisual();
  root->AddChild(objVis);
  objVis->SetLocalPose(kDefaultPose);
  objVis->AddGeometry(obj);

  auto mat = scene->CreateMaterial();
  mat->SetAmbient(kDefaultColor);
  objVis->SetMaterial(mat);

  return true;
}

// Register this plugin
IGN_COMMON_REGISTER_SINGLE_PLUGIN(ignition::gui::plugins::Grid3D,
                                  ignition::gui::Plugin)

