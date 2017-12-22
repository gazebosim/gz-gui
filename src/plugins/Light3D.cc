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

  this->OnRefresh();
}

/////////////////////////////////////////////////
void Light3D::Refresh()
{
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

    auto objName = QString::fromStdString(obj->Name());

    // Create widgets
    std::vector<PropertyWidget *> props;
    auto poseWidget = new Pose3dWidget();
    poseWidget->SetValue(QVariant::fromValue(obj->Parent()->WorldPose()));
    poseWidget->setProperty("objName", objName);
    poseWidget->setObjectName("poseWidget");
    this->connect(poseWidget, SIGNAL(ValueChanged(QVariant)), this,
        SLOT(OnChange(QVariant)));
    props.push_back(poseWidget);

    auto colorWidget = new ColorWidget();
    colorWidget->SetValue(QVariant::fromValue(obj->DiffuseColor()));
    colorWidget->setProperty("objName", objName);
    colorWidget->setObjectName("colorWidget");
    this->connect(colorWidget, SIGNAL(ValueChanged(QVariant)), this,
        SLOT(OnChange(QVariant)));
    props.push_back(colorWidget);

    this->AppendObj(obj, props);
  }
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
}

// Register this plugin
IGN_COMMON_REGISTER_SINGLE_PLUGIN(ignition::gui::plugins::Light3D,
                                  ignition::gui::Plugin)

