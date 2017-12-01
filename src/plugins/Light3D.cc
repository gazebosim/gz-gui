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
#include "ignition/gui/plugins/Light3D.hh"

// Default pose
static const ignition::math::Pose3d kDefaultPose{
    ignition::math::Pose3d(0, 0, 10, 0, 0, 0)};

// Default color
static const ignition::math::Color kDefaultColor{
    ignition::math::Color(0.5, 0.5, 0.5, 1.0)};

namespace ignition
{
namespace gui
{
namespace plugins
{
  /// \brief Holds configuration for a light
  struct LightInfo
  {
    /// \brief Light pose in the world
    math::Pose3d pose{kDefaultPose};

    /// \brief Light ambient color
    math::Color color{kDefaultColor};
  };

  class Light3DPrivate
  {
    /// \brief Pointer to scene
    public: rendering::ScenePtr scene;

    /// \brief Keep track of lights we currently found on the scene
    public: std::vector<rendering::LightPtr> lights;
  };
}
}
}

using namespace ignition;
using namespace gui;
using namespace plugins;

/////////////////////////////////////////////////
Light3D::Light3D()
  : Plugin(), dataPtr(new Light3DPrivate)
{
}

/////////////////////////////////////////////////
Light3D::~Light3D()
{
}

/////////////////////////////////////////////////
void Light3D::LoadConfig(const tinyxml2::XMLElement *_pluginElem)
{
  if (this->title.empty())
    this->title = "3D Light";

  // Configuration
  std::string engineName{"ogre"};
  std::string sceneName{"scene"};
  std::vector<LightInfo> lights;
  if (_pluginElem)
  {
    // All lights managed belong to the same engine and scene
    if (auto elem = _pluginElem->FirstChildElement("engine"))
      engineName = elem->GetText();

    if (auto elem = _pluginElem->FirstChildElement("scene"))
      sceneName = elem->GetText();

    // For lights to be inserted at startup
    for (auto insertElem = _pluginElem->FirstChildElement("insert");
         insertElem != nullptr;
        insertElem = insertElem->NextSiblingElement("insert"))
    {
      LightInfo lightInfo;
/*
      if (auto elem = insertElem->FirstChildElement("cell_count"))
        elem->QueryIntText(&lightInfo.cellCount);

      if (auto elem = insertElem->FirstChildElement("vertical_cell_count"))
        elem->QueryIntText(&lightInfo.vertCellCount);

      if (auto elem = insertElem->FirstChildElement("cell_length"))
        elem->QueryDoubleText(&lightInfo.cellLength);
*/
      if (auto elem = insertElem->FirstChildElement("pose"))
      {
        std::stringstream poseStr;
        poseStr << std::string(elem->GetText());
        poseStr >> lightInfo.pose;
      }

      if (auto elem = insertElem->FirstChildElement("color"))
      {
        std::stringstream colorStr;
        colorStr << std::string(elem->GetText());
        colorStr >> lightInfo.color;
      }

      lights.push_back(lightInfo);
    }
  }

  // Render engine
  auto engine = rendering::engine(engineName);
  if (!engine)
  {
    ignerr << "Engine [" << engineName
           << "] is not supported, light plugin won't work." << std::endl;
    return;
  }

  // Scene
  this->dataPtr->scene = engine->SceneByName(sceneName);
  if (!this->dataPtr->scene)
  {
    ignerr << "Scene [" << sceneName << "] not found, light plugin won't work."
           << std::endl;
    return;
  }
  auto root = this->dataPtr->scene->RootVisual();

  // Initial lights
  for (const auto &g : lights)
  {
    auto light = this->dataPtr->scene->CreateDirectionalLight();
    light->SetDiffuseColor(g.color);
    root->AddChild(light);
  }

  // Don't waste time loading widgets if this will be deleted anyway
  if (this->DeleteLaterRequested())
    return;

  this->Refresh();
}

/////////////////////////////////////////////////
void Light3D::Refresh()
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

    auto addButton = new QPushButton("New light");
    addButton->setObjectName("addLightButton");
    addButton->setToolTip("Add a new light with default values");
    this->connect(addButton, SIGNAL(clicked()), this, SLOT(OnAdd()));

    auto refreshButton = new QPushButton("Refresh");
    refreshButton->setObjectName("refreshLightButton");
    refreshButton->setToolTip("Refresh the list of lights");
    this->connect(refreshButton, SIGNAL(clicked()), this, SLOT(Refresh()));

    auto buttonsLayout = new QHBoxLayout();
    buttonsLayout->addWidget(addButton);
    buttonsLayout->addWidget(refreshButton);

    auto buttonsWidget = new QWidget();
    buttonsWidget->setLayout(buttonsLayout);

    mainLayout->addWidget(buttonsWidget);
  }

  // Search for all lights currently in the scene
  for (unsigned int i = 0; i < this->dataPtr->scene->LightCount(); ++i)
  {
    auto light = this->dataPtr->scene->LightByIndex(i);
    if (!light)
      continue;

    auto directionalLight =
        std::dynamic_pointer_cast<rendering::DirectionalLight>(light);
    if (!directionalLight)
      continue;

    this->dataPtr->lights.push_back(light);
    auto lightName = QString::fromStdString(light->Name());

    auto poseWidget = new Pose3dWidget();
    poseWidget->SetValue(QVariant::fromValue(light->Parent()->WorldPose()));
    poseWidget->setObjectName(lightName + "---poseWidget");
    this->connect(poseWidget, SIGNAL(ValueChanged(QVariant)), this,
        SLOT(OnChange(QVariant)));

    auto colorWidget = new ColorWidget();
    colorWidget->SetValue(QVariant::fromValue(light->DiffuseColor()));
    colorWidget->setObjectName(lightName + "---colorWidget");
    this->connect(colorWidget, SIGNAL(ValueChanged(QVariant)), this,
        SLOT(OnChange(QVariant)));

    auto deleteButton = new QPushButton("Delete light");
    deleteButton->setToolTip("Delete light " + lightName);
    deleteButton->setObjectName(lightName + "---deleteButton");
    this->connect(deleteButton, SIGNAL(clicked()), this, SLOT(OnDelete()));

    auto collapsible = new CollapsibleWidget(light->Name());
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
void Light3D::OnChange(const QVariant &_value)
{
  auto parts = this->sender()->objectName().split("---");
  if (parts.size() != 2)
    return;

  for (auto light : this->dataPtr->lights)
  {
    if (light->Name() != parts[0].toStdString())
      continue;

    if (parts[1] == "poseWidget")
      light->Parent()->SetWorldPose(_value.value<math::Pose3d>());
    else if (parts[1] == "colorWidget")
      light->SetDiffuseColor(_value.value<math::Color>());

    break;
  }
}

/////////////////////////////////////////////////
void Light3D::OnDelete()
{
  auto parts = this->sender()->objectName().split("---");
  if (parts.size() != 2)
    return;

  for (auto light : this->dataPtr->lights)
  {
    if (light->Name() != parts[0].toStdString())
      continue;

    // light->Scene()->DestroyLight(light->Parent());
    this->dataPtr->lights.erase(std::remove(this->dataPtr->lights.begin(),
                                           this->dataPtr->lights.end(), light),
                                           this->dataPtr->lights.end());

    this->Refresh();
    break;
  }
}

/////////////////////////////////////////////////
void Light3D::OnAdd()
{
  auto root = this->dataPtr->scene->RootVisual();

  auto light = this->dataPtr->scene->CreateDirectionalLight();
  light->SetDiffuseColor(kDefaultColor);
  root->AddChild(light);

  this->Refresh();
}

// Register this plugin
IGN_COMMON_REGISTER_SINGLE_PLUGIN(ignition::gui::plugins::Light3D,
                                  ignition::gui::Plugin)

