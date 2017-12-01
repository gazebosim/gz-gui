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
#include "ignition/gui/plugins/Geometry3D.hh"

// Default pose
static const ignition::math::Pose3d kDefaultPose{ignition::math::Pose3d::Zero};

// Default color
static const ignition::math::Color kDefaultColor{
    ignition::math::Color(0.2, 0.2, 0.8, 1.0)};

namespace ignition
{
namespace gui
{
namespace plugins
{
  /// \brief Holds configuration for a geometry
  struct GeometryInfo
  {
    /// \brief Geometry pose in the world
    math::Pose3d pose{kDefaultPose};

    /// \brief Geometry ambient color
    math::Color color{kDefaultColor};
  };

  class Geometry3DPrivate
  {
    /// \brief Pointer to scene
    public: rendering::ScenePtr scene;

    /// \brief Keep track of geometries we currently found on the scene
    public: std::vector<rendering::GeometryPtr> geometries;
  };
}
}
}

using namespace ignition;
using namespace gui;
using namespace plugins;

/////////////////////////////////////////////////
Geometry3D::Geometry3D()
  : Plugin(), dataPtr(new Geometry3DPrivate)
{
}

/////////////////////////////////////////////////
Geometry3D::~Geometry3D()
{
}

/////////////////////////////////////////////////
void Geometry3D::LoadConfig(const tinyxml2::XMLElement *_pluginElem)
{
  if (this->title.empty())
    this->title = "3D Geometry";

  // Configuration
  std::string engineName{"ogre"};
  std::string sceneName{"scene"};
  std::vector<GeometryInfo> geometries;
  if (_pluginElem)
  {
    // All geometries managed belong to the same engine and scene
    if (auto elem = _pluginElem->FirstChildElement("engine"))
      engineName = elem->GetText();

    if (auto elem = _pluginElem->FirstChildElement("scene"))
      sceneName = elem->GetText();

    // For geometries to be inserted at startup
    for (auto insertElem = _pluginElem->FirstChildElement("insert");
         insertElem != nullptr;
        insertElem = insertElem->NextSiblingElement("insert"))
    {
      GeometryInfo geometryInfo;

      if (auto elem = insertElem->FirstChildElement("pose"))
      {
        std::stringstream poseStr;
        poseStr << std::string(elem->GetText());
        poseStr >> geometryInfo.pose;
      }

      if (auto elem = insertElem->FirstChildElement("color"))
      {
        std::stringstream colorStr;
        colorStr << std::string(elem->GetText());
        colorStr >> geometryInfo.color;
      }

      geometries.push_back(geometryInfo);
    }
  }

  // Render engine
  auto engine = rendering::engine(engineName);
  if (!engine)
  {
    ignerr << "Engine [" << engineName
           << "] is not supported, geometry plugin won't work." << std::endl;
    return;
  }

  // Scene
  this->dataPtr->scene = engine->SceneByName(sceneName);
  if (!this->dataPtr->scene)
  {
    ignerr << "Scene [" << sceneName << "] not found, geometry plugin won't work."
           << std::endl;
    return;
  }
  auto root = this->dataPtr->scene->RootVisual();

  // Initial geometries
  for (const auto &g : geometries)
  {
    auto geometry = this->dataPtr->scene->CreateBox();

    auto geometryVis = this->dataPtr->scene->CreateVisual();
    root->AddChild(geometryVis);
    geometryVis->SetLocalPose(g.pose);
    geometryVis->AddGeometry(geometry);

    auto mat = this->dataPtr->scene->CreateMaterial();
    mat->SetAmbient(g.color);
    geometryVis->SetMaterial(mat);
  }

  // Don't waste time loading widgets if this will be deleted anyway
  if (this->DeleteLaterRequested())
    return;

  this->Refresh();
}

/////////////////////////////////////////////////
void Geometry3D::Refresh()
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

    auto addButton = new QPushButton("New geometry");
    addButton->setObjectName("addGeometryButton");
    addButton->setToolTip("Add a new geometry with default values");
    this->connect(addButton, SIGNAL(clicked()), this, SLOT(OnAdd()));

    auto refreshButton = new QPushButton("Refresh");
    refreshButton->setObjectName("refreshGeometryButton");
    refreshButton->setToolTip("Refresh the list of geometries");
    this->connect(refreshButton, SIGNAL(clicked()), this, SLOT(Refresh()));

    auto buttonsLayout = new QHBoxLayout();
    buttonsLayout->addWidget(addButton);
    buttonsLayout->addWidget(refreshButton);

    auto buttonsWidget = new QWidget();
    buttonsWidget->setLayout(buttonsLayout);

    mainLayout->addWidget(buttonsWidget);
  }

  // Search for all geometries currently in the scene
  for (unsigned int i = 0; i < this->dataPtr->scene->VisualCount(); ++i)
  {
    auto vis = this->dataPtr->scene->VisualByIndex(i);
    if (!vis || vis->GeometryCount() == 0)
      continue;

    rendering::GeometryPtr geometry;
    for (unsigned int j = 0; j < vis->GeometryCount(); ++j)
    {
      geometry = std::dynamic_pointer_cast<rendering::Geometry>(
          vis->GeometryByIndex(j));
      if (geometry)
        break;
    }
    if (!geometry)
      continue;

    this->dataPtr->geometries.push_back(geometry);
    auto geometryName = QString::fromStdString(geometry->Name());

    auto poseWidget = new Pose3dWidget();
    poseWidget->SetValue(QVariant::fromValue(geometry->Parent()->WorldPose()));
    poseWidget->setObjectName(geometryName + "---poseWidget");
    this->connect(poseWidget, SIGNAL(ValueChanged(QVariant)), this,
        SLOT(OnChange(QVariant)));

    auto colorWidget = new ColorWidget();
    colorWidget->SetValue(QVariant::fromValue(geometry->Material()->Ambient()));
    colorWidget->setObjectName(geometryName + "---colorWidget");
    this->connect(colorWidget, SIGNAL(ValueChanged(QVariant)), this,
        SLOT(OnChange(QVariant)));

    auto deleteButton = new QPushButton("Delete geometry");
    deleteButton->setToolTip("Delete geometry " + geometryName);
    deleteButton->setObjectName(geometryName + "---deleteButton");
    this->connect(deleteButton, SIGNAL(clicked()), this, SLOT(OnDelete()));

    auto collapsible = new CollapsibleWidget(geometry->Name());
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
void Geometry3D::OnChange(const QVariant &_value)
{
  auto parts = this->sender()->objectName().split("---");
  if (parts.size() != 2)
    return;

  for (auto geometry : this->dataPtr->geometries)
  {
    if (geometry->Name() != parts[0].toStdString())
      continue;

    if (parts[1] == "poseWidget")
      geometry->Parent()->SetWorldPose(_value.value<math::Pose3d>());
    else if (parts[1] == "colorWidget")
      geometry->Material()->SetAmbient(_value.value<math::Color>());

    break;
  }
}

/////////////////////////////////////////////////
void Geometry3D::OnDelete()
{
  auto parts = this->sender()->objectName().split("---");
  if (parts.size() != 2)
    return;

  for (auto geometry : this->dataPtr->geometries)
  {
    if (geometry->Name() != parts[0].toStdString())
      continue;

    geometry->Scene()->DestroyVisual(geometry->Parent());
    this->dataPtr->geometries.erase(std::remove(this->dataPtr->geometries.begin(),
                                           this->dataPtr->geometries.end(), geometry),
                                           this->dataPtr->geometries.end());

    this->Refresh();
    break;
  }
}

/////////////////////////////////////////////////
void Geometry3D::OnAdd()
{
  auto root = this->dataPtr->scene->RootVisual();

  auto geometry = this->dataPtr->scene->CreateBox();

  auto geometryVis = this->dataPtr->scene->CreateVisual();
  root->AddChild(geometryVis);
  geometryVis->SetLocalPose(kDefaultPose);
  geometryVis->AddGeometry(geometry);

  auto mat = this->dataPtr->scene->CreateMaterial();
  mat->SetAmbient(kDefaultColor);
  geometryVis->SetMaterial(mat);

  this->Refresh();
}

// Register this plugin
IGN_COMMON_REGISTER_SINGLE_PLUGIN(ignition::gui::plugins::Geometry3D,
                                  ignition::gui::Plugin)

