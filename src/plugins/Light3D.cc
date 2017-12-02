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
  /// \brief Holds configuration for an obj
  struct ObjInfo
  {
    /// \brief Light pose in the world
    math::Pose3d pose{kDefaultPose};

    /// \brief Light ambient color
    math::Color color{kDefaultColor};
  };

  class Light3DPrivate
  {
  };
}
}
}

using namespace ignition;
using namespace gui;
using namespace plugins;

/////////////////////////////////////////////////
Light3D::Light3D()
  : Object3DPlugin(), dataPtr(new Light3DPrivate)
{
}

/////////////////////////////////////////////////
Light3D::~Light3D()
{
}

/////////////////////////////////////////////////
void Light3D::LoadConfig(const tinyxml2::XMLElement *_pluginElem)
{
  this->typeSingular = "light";

  if (this->title.empty())
    this->title = "3D " + this->typeSingular;

  // Configuration
  std::string engineName{"ogre"};
  std::string sceneName{"scene"};
  std::vector<ObjInfo> objInfos;
  if (_pluginElem)
  {
    // All objs managed belong to the same engine and scene
    if (auto elem = _pluginElem->FirstChildElement("engine"))
      engineName = elem->GetText();

    if (auto elem = _pluginElem->FirstChildElement("scene"))
      sceneName = elem->GetText();

    // For objs to be inserted at startup
    for (auto insertElem = _pluginElem->FirstChildElement("insert");
         insertElem != nullptr;
        insertElem = insertElem->NextSiblingElement("insert"))
    {
      ObjInfo objInfo;

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

  // Render engine
  auto engine = rendering::engine(engineName);
  if (!engine)
  {
    ignerr << "Engine [" << engineName << "] is not supported, "
           << this->typeSingular << " plugin won't work." << std::endl;
    return;
  }

  // Scene
  this->scene = engine->SceneByName(sceneName);
  if (!this->scene)
  {
    ignerr << "Scene [" << sceneName << "] not found, "
           << this->typeSingular << " plugin won't work."
           << std::endl;
    return;
  }
  auto root = this->scene->RootVisual();

  // Initial objs
  for (const auto &g : objInfos)
  {
    auto obj = this->scene->CreateDirectionalLight();
    obj->SetDiffuseColor(g.color);
    root->AddChild(obj);
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

    auto addButton = new QPushButton("New " +
        QString::fromStdString(this->typeSingular));
    addButton->setObjectName("addButton" + QString::fromStdString(
        this->typeSingular));
    addButton->setToolTip("Add a new " + QString::fromStdString(
        this->typeSingular) + " with default values");
    this->connect(addButton, SIGNAL(clicked()), this, SLOT(OnAdd()));

    auto refreshButton = new QPushButton("Refresh");
    refreshButton->setObjectName("refreshButton" + QString::fromStdString(
        this->typeSingular));
    refreshButton->setToolTip("Refresh the list of objs");
    this->connect(refreshButton, SIGNAL(clicked()), this, SLOT(Refresh()));

    auto buttonsLayout = new QHBoxLayout();
    buttonsLayout->addWidget(addButton);
    buttonsLayout->addWidget(refreshButton);

    auto buttonsWidget = new QWidget();
    buttonsWidget->setLayout(buttonsLayout);

    mainLayout->addWidget(buttonsWidget);
  }

  // Search for all objs currently in the scene
  for (unsigned int i = 0; i < this->scene->LightCount(); ++i)
  {
    auto obj = this->scene->LightByIndex(i);
    if (!obj)
      continue;

    auto directionalLight =
        std::dynamic_pointer_cast<rendering::DirectionalLight>(obj);
    if (!directionalLight)
      continue;

    this->objs.push_back(obj);
    auto objName = QString::fromStdString(obj->Name());

    auto poseWidget = new Pose3dWidget();
    poseWidget->SetValue(QVariant::fromValue(obj->Parent()->WorldPose()));
    poseWidget->setObjectName(objName + "---poseWidget");
    this->connect(poseWidget, SIGNAL(ValueChanged(QVariant)), this,
        SLOT(OnChange(QVariant)));

    auto colorWidget = new ColorWidget();
    colorWidget->SetValue(QVariant::fromValue(obj->DiffuseColor()));
    colorWidget->setObjectName(objName + "---colorWidget");
    this->connect(colorWidget, SIGNAL(ValueChanged(QVariant)), this,
        SLOT(OnChange(QVariant)));

    auto deleteButton = new QPushButton("Delete obj");
    deleteButton->setToolTip("Delete obj " + objName);
    deleteButton->setObjectName(objName + "---deleteButton");
    this->connect(deleteButton, SIGNAL(clicked()), this, SLOT(OnDelete()));

    auto collapsible = new CollapsibleWidget(obj->Name());
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
bool Light3D::Change(const rendering::ObjectPtr &_obj,
    const std::string &_property, const QVariant &_value)
{
  auto derived = std::dynamic_pointer_cast<rendering::Light>(_obj);
  if (!derived)
    return false;

  if (_property == "poseWidget")
    derived->SetWorldPose(_value.value<math::Pose3d>());
  else if (_property == "colorWidget")
    derived->SetDiffuseColor(_value.value<math::Color>());
  else
  {
    ignwarn << "Unknown property [" << _property << std::endl;
    return false;
  }

  return true;
}

/////////////////////////////////////////////////
bool Light3D::Delete(const rendering::ObjectPtr &_obj)
{
  auto derived = std::dynamic_pointer_cast<rendering::Light>(_obj);
  if (!derived)
    return false;

//  this->scene->DestroyNode(derived);

  return true;
}

/////////////////////////////////////////////////
void Light3D::Add()
{
  auto root = this->scene->RootVisual();

  auto obj = this->scene->CreateDirectionalLight();
  obj->SetDiffuseColor(kDefaultColor);
  root->AddChild(obj);

  this->Refresh();
}

// Register this plugin
IGN_COMMON_REGISTER_SINGLE_PLUGIN(ignition::gui::plugins::Light3D,
                                  ignition::gui::Plugin)

