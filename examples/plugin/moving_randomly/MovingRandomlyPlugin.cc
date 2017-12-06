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

#include <iostream>
#include <ignition/common/PluginMacros.hh>
#include <ignition/math/Pose3.hh>
#include <ignition/math/Rand.hh>
#include <ignition/rendering.hh>

#include "MovingRandomlyPlugin.hh"

using namespace ignition;
using namespace gui;

/////////////////////////////////////////////////
MovingRandomlyPlugin::MovingRandomlyPlugin()
  : Plugin()
{
  auto engine = rendering::engine("ogre");
  auto scene = engine->SceneByName("scene");
  auto root = scene->RootVisual();

  auto light = scene->CreateDirectionalLight();
  light->SetDirection(-0.5, 0.5, -1);
  light->SetDiffuseColor(0.5, 0.5, 0.5);
  light->SetSpecularColor(0.5, 0.5, 0.5);
  root->AddChild(light);

  auto green = scene->CreateMaterial();
  green->SetAmbient(0.0, 0.5, 0.0);
  green->SetDiffuse(0.0, 0.7, 0.0);
  green->SetSpecular(0.5, 0.5, 0.5);
  green->SetShininess(50);
  green->SetReflectivity(0);

  auto box = scene->CreateVisual();
  box->AddGeometry(scene->CreateBox());
  box->SetLocalPosition(3, 0, 0);
  box->SetMaterial(green);
  root->AddChild(box);

  this->timer = new QTimer();
  this->connect(this->timer, &QTimer::timeout, [=](){
    auto pose = box->WorldPose();
    pose.Pos().X() += math::Rand::DblUniform(-0.1, 0.3);
    pose.Pos().Y() += math::Rand::DblUniform(-0.1, 0.1);
    pose.Pos().Z() += math::Rand::DblUniform(-0.1, 0.1);
    pose.Rot().X() += math::Rand::DblUniform(-0.1, 0.1);
    pose.Rot().Y() += math::Rand::DblUniform(-0.1, 0.1);
    pose.Rot().Z() += math::Rand::DblUniform(-0.1, 0.1);
    box->SetWorldPose(pose);
  });
  this->timer->start(100);

  this->DeleteLater();
}

/////////////////////////////////////////////////
MovingRandomlyPlugin::~MovingRandomlyPlugin()
{
}

// Register this plugin
IGN_COMMON_REGISTER_SINGLE_PLUGIN(ignition::gui::MovingRandomlyPlugin,
                                  ignition::gui::Plugin);
