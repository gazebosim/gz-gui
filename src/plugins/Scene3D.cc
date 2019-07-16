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

#include <cmath>
#include <sstream>
#include <string>

#include <ignition/common/Console.hh>
#include <ignition/common/MouseEvent.hh>
#include <ignition/common/PluginMacros.hh>
#include <ignition/math/Color.hh>
#include <ignition/math/Pose3.hh>
#include <ignition/math/Vector2.hh>
#include <ignition/math/Vector3.hh>
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

    /// \brief Store the window id to use at paintEvent once
    /// (Qt complains if we call this->winId() from the paint event)
    public: WId windowId;
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
  if (nullptr == this->dataPtr->camera)
  {
    return;
  }

  igndbg << "Destroy camera [" << this->dataPtr->camera->Name() << "]"
         << std::endl;
  // Destroy camera
  auto scene = this->dataPtr->camera->Scene();
  this->dataPtr->camera->RemoveChildren();
  scene->DestroyNode(this->dataPtr->camera);
  this->dataPtr->camera.reset();

  // If that was the last sensor, destroy scene
  if (scene->SensorCount() == 0)
  {
    igndbg << "Destroy scene [" << scene->Name() << "]" << std::endl;
    auto engine = scene->Engine();
    engine->DestroyScene(scene);

    // TODO: If that was the last scene, terminate engine?
  }
}

/////////////////////////////////////////////////
void Scene3D::LoadConfig(const tinyxml2::XMLElement *_pluginElem)
{
  if (this->title.empty())
    this->title = "3D Scene";

  // Default parameters
  std::string engineName{"ogre"};
  std::string sceneName{"scene"};
  math::Color ambientLight(0.3, 0.3, 0.3);
  math::Color backgroundColor(0.3, 0.3, 0.3);
  math::Pose3d cameraPose(0, 0, 5, 0, 0, 0);

  // Custom parameters
  if (_pluginElem)
  {
    if (auto elem = _pluginElem->FirstChildElement("engine"))
      engineName = elem->GetText();

    if (auto elem = _pluginElem->FirstChildElement("scene"))
      sceneName = elem->GetText();

    if (auto elem = _pluginElem->FirstChildElement("ambient_light"))
    {
      std::stringstream colorStr;
      colorStr << std::string(elem->GetText());
      colorStr >> ambientLight;
    }

    if (auto elem = _pluginElem->FirstChildElement("background_color"))
    {
      std::stringstream colorStr;
      colorStr << std::string(elem->GetText());
      colorStr >> backgroundColor;
    }

    if (auto elem = _pluginElem->FirstChildElement("camera_pose"))
    {
      std::stringstream poseStr;
      poseStr << std::string(elem->GetText());
      poseStr >> cameraPose;
    }
  }

  // Layout
  this->setLayout(new QVBoxLayout());

  this->setMinimumWidth(300);
  this->setMinimumHeight(300);
  this->setAttribute(Qt::WA_OpaquePaintEvent, true);
  this->setAttribute(Qt::WA_PaintOnScreen, true);
  this->setAttribute(Qt::WA_NoSystemBackground, true);

  // Store window id
  this->dataPtr->windowId = this->winId();

  // Render engine
  auto engine = rendering::engine(engineName);
  if (!engine)
  {
    ignerr << "Engine [" << engineName << "] is not supported" << std::endl;
    return;
  }

  // Scene
  auto scene = engine->SceneByName(sceneName);
  if (!scene)
  {
    igndbg << "Create scene [" << sceneName << "]" << std::endl;
    scene = engine->CreateScene(sceneName);
    scene->SetAmbientLight(ambientLight);
    scene->SetBackgroundColor(backgroundColor);
  }
  auto root = scene->RootVisual();

  // Camera
  igndbg << "Create camera" << std::endl;
  this->dataPtr->camera = scene->CreateCamera();
  root->AddChild(this->dataPtr->camera);
  this->dataPtr->camera->SetLocalPose(cameraPose);
  this->dataPtr->camera->SetImageWidth(800);
  this->dataPtr->camera->SetImageHeight(600);
  this->dataPtr->camera->SetAntiAliasing(2);
  this->dataPtr->camera->SetAspectRatio(this->width() / this->height());
  this->dataPtr->camera->SetHFOV(M_PI * 0.5);

  // Timer to repaint
  this->dataPtr->updateTimer = new QTimer(this);
  this->connect(this->dataPtr->updateTimer, SIGNAL(timeout()),
      this, SLOT(update()));
  this->dataPtr->updateTimer->start(std::round(1000.0 / 60.0));
}

/////////////////////////////////////////////////
QPaintEngine *Scene3D::paintEngine() const
{
  return nullptr;
}

/////////////////////////////////////////////////
void Scene3D::paintEvent(QPaintEvent *_e)
{
  if (!this->dataPtr->camera)
  {
    return;
  }
  // Create render window on first paint, so we're sure the window is showing
  // when we attach to it
  if (!this->dataPtr->renderWindow)
  {
    this->dataPtr->renderWindow = this->dataPtr->camera->CreateRenderWindow();
    this->dataPtr->renderWindow->SetHandle(
        std::to_string(static_cast<uint64_t>(this->dataPtr->windowId)));
    this->dataPtr->renderWindow->SetWidth(this->width());
    this->dataPtr->renderWindow->SetHeight(this->height());
  }

  if (this->dataPtr->camera && this->dataPtr->renderWindow)
    this->dataPtr->camera->Update();

  _e->accept();
}

/////////////////////////////////////////////////
void Scene3D::resizeEvent(QResizeEvent *_e)
{
  if (this->dataPtr->renderWindow)
  {
    this->dataPtr->renderWindow->OnResize(_e->size().width(),
                                          _e->size().height());
  }

  if (this->dataPtr->camera)
  {
    this->dataPtr->camera->SetAspectRatio(
        static_cast<double>(this->width()) / this->height());
    this->dataPtr->camera->SetHFOV(M_PI * 0.5);
  }
}

/////////////////////////////////////////////////
void Scene3D::mousePressEvent(QMouseEvent *_e)
{
  auto event = convert(*_e);
  event.SetPressPos(event.Pos());
  this->dataPtr->mouseEvent = event;

  // Update target
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

