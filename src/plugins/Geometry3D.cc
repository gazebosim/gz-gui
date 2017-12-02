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
  };
}
}
}

using namespace ignition;
using namespace gui;
using namespace plugins;

/////////////////////////////////////////////////
Geometry3D::Geometry3D()
  : Object3DPlugin(), dataPtr(new Geometry3DPrivate)
{
}

/////////////////////////////////////////////////
Geometry3D::~Geometry3D()
{
}

/////////////////////////////////////////////////
void Geometry3D::LoadConfig(const tinyxml2::XMLElement *_pluginElem)
{
  this->typeSingular = "geometry";

  if (this->title.empty())
    this->title = "3D " + this->typeSingular;

  // Configuration
  std::string engineName{"ogre"};
  std::string sceneName{"scene"};
  std::vector<GeometryInfo> objInfos;
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
      GeometryInfo objInfo;

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
    auto geometry = this->scene->CreateBox();

    auto geometryVis = this->scene->CreateVisual();
    root->AddChild(geometryVis);
    geometryVis->SetLocalPose(g.pose);
    geometryVis->AddGeometry(geometry);

    auto mat = this->scene->CreateMaterial();
    mat->SetAmbient(g.color);
    geometryVis->SetMaterial(mat);
  }

  // Don't waste time loading widgets if this will be deleted anyway
  if (this->DeleteLaterRequested())
    return;

  this->OnRefresh();
}

/////////////////////////////////////////////////
void Geometry3D::Refresh()
{
  // Search for all geometries currently in the scene
  for (unsigned int i = 0; i < this->scene->VisualCount(); ++i)
  {
    auto vis = this->scene->VisualByIndex(i);
    if (!vis || vis->GeometryCount() == 0)
      continue;

    rendering::GeometryPtr obj;
    for (unsigned int j = 0; j < vis->GeometryCount(); ++j)
    {
      obj = std::dynamic_pointer_cast<rendering::Geometry>(
          vis->GeometryByIndex(j));
      if (obj)
        break;
    }
    if (!obj)
      continue;

    // Create widgets
    std::vector<PropertyWidget *> props;
    auto objName = QString::fromStdString(obj->Name());

    auto poseWidget = new Pose3dWidget();
    poseWidget->SetValue(QVariant::fromValue(obj->Parent()->WorldPose()));
    poseWidget->setObjectName(objName + "---poseWidget");
    this->connect(poseWidget, SIGNAL(ValueChanged(QVariant)), this,
        SLOT(OnChange(QVariant)));
    props.push_back(poseWidget);

    auto colorWidget = new ColorWidget();
    colorWidget->SetValue(QVariant::fromValue(obj->Material()->Ambient()));
    colorWidget->setObjectName(objName + "---colorWidget");
    this->connect(colorWidget, SIGNAL(ValueChanged(QVariant)), this,
        SLOT(OnChange(QVariant)));
    props.push_back(colorWidget);

    this->AppendObj(obj, props);
  }
}

/////////////////////////////////////////////////
bool Geometry3D::Change(const rendering::ObjectPtr &_obj,
    const std::string &_property, const QVariant &_value)
{
  auto derived = std::dynamic_pointer_cast<rendering::Geometry>(_obj);
  if (!derived)
    return false;

  if (_property == "poseWidget")
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
bool Geometry3D::Delete(const rendering::ObjectPtr &_obj)
{
  auto derived = std::dynamic_pointer_cast<rendering::Geometry>(_obj);
  if (!derived)
    return false;

  this->scene->DestroyVisual(derived->Parent());

  return true;
}

/////////////////////////////////////////////////
void Geometry3D::Add()
{
  auto root = this->scene->RootVisual();

  auto geometry = this->scene->CreateBox();

  auto geometryVis = this->scene->CreateVisual();
  root->AddChild(geometryVis);
  geometryVis->SetLocalPose(kDefaultPose);
  geometryVis->AddGeometry(geometry);

  auto mat = this->scene->CreateMaterial();
  mat->SetAmbient(kDefaultColor);
  geometryVis->SetMaterial(mat);
}

// Register this plugin
IGN_COMMON_REGISTER_SINGLE_PLUGIN(ignition::gui::plugins::Geometry3D,
                                  ignition::gui::Plugin)

