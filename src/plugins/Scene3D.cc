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
#include <mutex>
#include <string>

#include <ignition/common/Console.hh>
#include <ignition/common/PluginMacros.hh>
#include <ignition/rendering.hh>

#include "ignition/gui/plugins/Scene3D.hh"

namespace ignition
{
namespace gui
{
namespace plugins
{
  class Scene3DPrivate
  {
    /// \brief Timer to repaint the widget
    public: QTimer *updateTimer;

    /// \brief Pointer to user camera
    public: rendering::CameraPtr camera;

    /// \brief Pointer to render window
    public: rendering::RenderWindowPtr renderWindow;
  };
}
}
}

using namespace ignition;
using namespace gui;
using namespace plugins;

/////////////////////////////////////////////////
Scene3D::Scene3D()
  : Plugin(), dataPtr(new Scene3DPrivate)
{
}

/////////////////////////////////////////////////
Scene3D::~Scene3D()
{
}

/////////////////////////////////////////////////
void Scene3D::LoadConfig(const tinyxml2::XMLElement */*_pluginElem*/)
{
  if (this->title.empty())
    this->title = "3D Scene";

  // Layout
  this->setLayout(new QVBoxLayout());
  this->setMinimumWidth(300);
  this->setMinimumHeight(300);

  // Render engine
  std::string engineName{"ogre"};
  auto engine = rendering::engine(engineName);
  if (!engine)
  {
    ignerr << "Engine [" << engineName << "] is not supported" << std::endl;
    return;
  }

  // Scene
  std::string sceneName{"scene"};
  auto scene = engine->SceneByName(sceneName);
  if (!scene)
  {
    scene = engine->CreateScene(sceneName);
    scene->SetAmbientLight(0.3, 0.3, 0.3);
    scene->SetBackgroundColor(0.3, 0.3, 0.3);
  }
  auto root = scene->RootVisual();

  // Directional light
  auto light = scene->CreateDirectionalLight();
  root->AddChild(light);
  light->SetDirection(0.5, 0.5, -1);
  light->SetDiffuseColor(0.8, 0.8, 0.8);
  light->SetSpecularColor(0.5, 0.5, 0.5);

  // Grid
  auto gridGeom = scene->CreateGrid();
  gridGeom->SetCellCount(20);
  gridGeom->SetCellLength(1);
  gridGeom->SetVerticalCellCount(0);

  auto grid = scene->CreateVisual();
  root->AddChild(grid);
  grid->SetLocalPosition(3, 0, 0.0);
  grid->AddGeometry(gridGeom);

  auto gray = scene->CreateMaterial();
  gray->SetAmbient(0.7, 0.7, 0.7);
  gray->SetDiffuse(0.7, 0.7, 0.7);
  gray->SetSpecular(0.7, 0.7, 0.7);
  grid->SetMaterial(gray);

  // Camera
  this->dataPtr->camera = scene->CreateCamera("camera");
  root->AddChild(this->dataPtr->camera);
  this->dataPtr->camera->SetLocalPosition(0.0, 0.0, 0.5);
  this->dataPtr->camera->SetLocalRotation(0.0, 0.0, 0.0);
  this->dataPtr->camera->SetImageWidth(800);
  this->dataPtr->camera->SetImageHeight(600);
  this->dataPtr->camera->SetAntiAliasing(2);
  this->dataPtr->camera->SetAspectRatio(this->width() / this->height());
  this->dataPtr->camera->SetHFOV(M_PI * 0.5);

  // Render window
  this->dataPtr->renderWindow = this->dataPtr->camera->CreateRenderWindow();
  this->dataPtr->renderWindow->SetHandle(
      std::to_string(static_cast<uint64_t>(this->winId())));
  this->dataPtr->renderWindow->SetWidth(this->width());
  this->dataPtr->renderWindow->SetHeight(this->height());

  // Timer to repaint
  this->dataPtr->updateTimer = new QTimer(this);
  this->connect(this->dataPtr->updateTimer, SIGNAL(timeout()),
      this, SLOT(update()));
  this->dataPtr->updateTimer->start(std::round(1000.0 / 60.0));
}

/////////////////////////////////////////////////
void Scene3D::paintEvent(QPaintEvent *_e)
{
  if (this->dataPtr->camera)
    this->dataPtr->camera->Update();

  _e->accept();
}

/////////////////////////////////////////////////
void Scene3D::resizeEvent(QResizeEvent *_e)
{
  if (!this->dataPtr->renderWindow || !this->dataPtr->camera)
    return;

  this->dataPtr->renderWindow->OnResize(_e->size().width(),
                                        _e->size().height());

  this->dataPtr->camera->SetAspectRatio(
      static_cast<double>(this->width()) / this->height());
  this->dataPtr->camera->SetHFOV(M_PI * 0.5);
}

// Register this plugin
IGN_COMMON_REGISTER_SINGLE_PLUGIN(ignition::gui::plugins::Scene3D,
                                  ignition::gui::Plugin)

