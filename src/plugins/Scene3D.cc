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
#include <ignition/common/MouseEvent.hh>
#include <ignition/common/PluginMacros.hh>
#include <ignition/rendering.hh>

#include "ignition/gui/Conversions.hh"
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

    /// \brief Keep latest mouse event
    public: common::MouseEvent mouseEvent;

    /// \brief Keep latest target point in the 3D world (for camera orbiting)
    public: math::Vector3d target;
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

/////////////////////////////////////////////////
void Scene3D::mousePressEvent(QMouseEvent *_e)
{
  auto event = convert(*_e);
  event.SetPressPos(event.Pos());
  this->dataPtr->mouseEvent = event;

  this->dataPtr->target = this->ScreenToScene(event.PressPos());
}

/////////////////////////////////////////////////
void Scene3D::mouseReleaseEvent(QMouseEvent *_e)
{
  this->dataPtr->mouseEvent = convert(*_e);

  // Clear target
  this->dataPtr->target = math::Vector3d::Zero;
}

/////////////////////////////////////////////////
void Scene3D::mouseMoveEvent(QMouseEvent *_e)
{
  auto event = convert(*_e);
  event.SetPressPos(this->dataPtr->mouseEvent.PressPos());

  if (!event.Dragging())
    return;

  auto dragInt = event.Pos() - this->dataPtr->mouseEvent.Pos();
  auto dragDistance = math::Vector2d(dragInt.X(), dragInt.Y());

  rendering::OrbitViewController controller;
  controller.SetCamera(this->dataPtr->camera);
  controller.SetTarget(this->dataPtr->target);

  // Pan with left button
  if (event.Buttons() & common::MouseEvent::LEFT)
    controller.Pan(dragDistance);
  // Orbit with middle button
  else if (event.Buttons() & common::MouseEvent::MIDDLE)
    controller.Orbit(dragDistance);

  this->dataPtr->mouseEvent = event;
}

/////////////////////////////////////////////////
void Scene3D::wheelEvent(QWheelEvent *_e)
{
  // 3D target
  auto target = this->ScreenToScene(math::Vector2i(_e->x(), _e->y()));

  // Scroll amount
  double distance = this->dataPtr->camera->WorldPosition().Distance(target);
  double scroll = (_e->angleDelta().y() > 0) ? -1.0 : 1.0;
  double amount = -scroll * distance / 5.0;

  // Zoom
  rendering::OrbitViewController controller;
  controller.SetCamera(this->dataPtr->camera);
  controller.SetTarget(target);
  controller.Zoom(amount);
}

/////////////////////////////////////////////////
math::Vector3d Scene3D::ScreenToScene(const math::Vector2i &_screenPos) const
{
  // Normalize point on the image
  double width = this->dataPtr->camera->ImageWidth();
  double height = this->dataPtr->camera->ImageHeight();

  double nx = 2.0 * _screenPos.X() / width - 1.0;
  double ny = 1.0 - 2.0 * _screenPos.Y() / height;

  // Make a ray query
  auto rayQuery = this->dataPtr->camera->Scene()->CreateRayQuery();
  rayQuery->SetFromCamera(this->dataPtr->camera, math::Vector2d(nx, ny));

  auto result = rayQuery->ClosestPoint();
  if (result)
    return result.point;

  // Set point to be 10m away if no intersection found
  return rayQuery->Origin() + rayQuery->Direction() * 10;
}

// Register this plugin
IGN_COMMON_REGISTER_SINGLE_PLUGIN(ignition::gui::plugins::Scene3D,
                                  ignition::gui::Plugin)

