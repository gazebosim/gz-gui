/*
 * Copyright (C) 2020 Open Source Robotics Foundation
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
#include <ignition/plugin/Register.hh>
#include "Scene3DNew.hh"
#include <ignition/common/Console.hh>

#include <iostream>

using namespace ignition;
using namespace gui;
using namespace plugins;

Scene3DNew::~Scene3DNew()
{
}

//////////////////////////////////////////
void Scene3DNew::LoadConfig(const tinyxml2::XMLElement * _pluginElem)
{
  ignerr << "Scene3DNew LoadConfig" << std::endl;

  if (this->title.empty())
    this->title = "Scene3D new!";

  this->dataPtr->SetPluginItem(this->PluginItem());

  if (_pluginElem)
  {
    auto elem = _pluginElem->FirstChildElement("engine");
    if (nullptr != elem && nullptr != elem->GetText())
    {
      this->dataPtr->SetEngineName(elem->GetText());
      // renderWindow->SetEngineName();
      // there is a problem with displaying ogre2 render textures that are in
      // sRGB format. Workaround for now is to apply gamma correction manually.
      // There maybe a better way to solve the problem by making OpenGL calls..
      if (elem->GetText() == std::string("ogre2"))
        this->PluginItem()->setProperty("gammaCorrect", true);

      elem = _pluginElem->FirstChildElement("scene");
      if (nullptr != elem && nullptr != elem->GetText())
        this->dataPtr->SetSceneName(elem->GetText());

      elem = _pluginElem->FirstChildElement("ambient_light");
      if (nullptr != elem && nullptr != elem->GetText())
      {
        math::Color ambient;
        std::stringstream colorStr;
        colorStr << std::string(elem->GetText());
        colorStr >> ambient;
        this->dataPtr->SetAmbientLight(ambient);
      }

      elem = _pluginElem->FirstChildElement("background_color");
      if (nullptr != elem && nullptr != elem->GetText())
      {
        math::Color bgColor;
        std::stringstream colorStr;
        colorStr << std::string(elem->GetText());
        colorStr >> bgColor;
        this->dataPtr->SetBackgroundColor(bgColor);
      }

      elem = _pluginElem->FirstChildElement("camera_pose");
      if (nullptr != elem && nullptr != elem->GetText())
      {
        math::Pose3d pose;
        std::stringstream poseStr;
        poseStr << std::string(elem->GetText());
        poseStr >> pose;
        this->dataPtr->SetCameraPose(pose);
      }

      elem = _pluginElem->FirstChildElement("service");
      if (nullptr != elem && nullptr != elem->GetText())
      {
        std::string service = elem->GetText();
        this->dataPtr->SetSceneService(service);
      }

      elem = _pluginElem->FirstChildElement("pose_topic");
      if (nullptr != elem && nullptr != elem->GetText())
      {
        std::string topic = elem->GetText();
        this->dataPtr->SetPoseTopic(topic);
      }

      elem = _pluginElem->FirstChildElement("deletion_topic");
      if (nullptr != elem && nullptr != elem->GetText())
      {
        std::string topic = elem->GetText();
        this->dataPtr->SetDeletionTopic(topic);
      }

      elem = _pluginElem->FirstChildElement("scene_topic");
      if (nullptr != elem && nullptr != elem->GetText())
      {
        std::string topic = elem->GetText();
        this->dataPtr->SetSceneTopic(topic);
      }
    }
    this->dataPtr->SetSceneService("/world/shapes/scene/info");
    this->dataPtr->SetPoseTopic("/world/shapes/pose/info");
    this->dataPtr->SetSceneTopic("/world/shapes/scene/info");
  }
}

//////////////////////////////////////////
Scene3DNew::Scene3DNew() : Plugin(),
    dataPtr(new Scene3DInterface())
{
}

// Register this plugin
IGNITION_ADD_PLUGIN(ignition::gui::plugins::Scene3DNew,
                    ignition::gui::Plugin)
