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
#include <ignition/common/Animation.hh>
#include <ignition/common/PluginMacros.hh>
#include <ignition/common/KeyFrame.hh>
#include <ignition/math/Pose3.hh>
#include <ignition/math/Quaterniond.hh>
#include <ignition/math/Vector3.hh>
#include <ignition/rendering.hh>

#include "AnimatedBoxPlugin.hh"

using namespace ignition;
using namespace gui;

/////////////////////////////////////////////////
AnimatedBoxPlugin::AnimatedBoxPlugin()
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
  box->SetMaterial(green);
  root->AddChild(box);

  auto animation = new common::PoseAnimation("anim", 4, true);

  {
    auto key = animation->CreateKeyFrame(0.0);
    key->Translation(math::Vector3d(-3, 3, 0));
  }

  {
    auto key = animation->CreateKeyFrame(1.0);
    key->Translation(math::Vector3d(3, 3, 0));
    key->Rotation(math::Quaterniond(0, 0, -IGN_PI*0.5));
  }

  {
    auto key = animation->CreateKeyFrame(2.0);
    key->Translation(math::Vector3d(3, -3, 0));
    key->Rotation(math::Quaterniond(0, 0, IGN_PI));
  }

  {
    auto key = animation->CreateKeyFrame(3.0);
    key->Translation(math::Vector3d(-3, -3, 0));
    key->Rotation(math::Quaterniond(0, 0, IGN_PI*0.5));
  }

  {
    auto key = animation->CreateKeyFrame(4.0);
    key->Translation(math::Vector3d(-3, 3, 0));
  }

  this->timer = new QTimer();
  this->connect(this->timer, &QTimer::timeout, [=]()
  {
    common::PoseKeyFrame pose(animTime);
    animation->Time(animTime);
    animation->InterpolatedKeyFrame(pose);

    box->SetWorldPose(math::Pose3d(pose.Translation(), pose.Rotation()));

    animTime += 0.05;
  });
  this->timer->start(100);

  this->DeleteLater();
}

/////////////////////////////////////////////////
AnimatedBoxPlugin::~AnimatedBoxPlugin()
{
}

// Register this plugin
IGN_COMMON_REGISTER_SINGLE_PLUGIN(ignition::gui::AnimatedBoxPlugin,
                                  ignition::gui::Plugin);
