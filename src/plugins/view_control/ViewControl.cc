/*
 * Copyright (C) 2021 Open Source Robotics Foundation
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

#include <ignition/common/MouseEvent.hh>

#include <ignition/gui/Application.hh>
#include <ignition/gui/GuiEvents.hh>
#include <ignition/gui/MainWindow.hh>

#include <ignition/plugin/Register.hh>

#include <ignition/rendering/Camera.hh>
#include <ignition/rendering/OrbitViewController.hh>
#include <ignition/rendering/RenderingIface.hh>
#include <ignition/rendering/RayQuery.hh>

#include "ViewControl.hh"

/// \brief Private data class for ViewControl
class ignition::gui::plugins::ViewControlPrivate
{
  /// \brief
  public: void OnRender();

  math::Vector3d ScreenToScene(
      const math::Vector2i &_screenPos) const;

  /// \brief Flag to indicate if mouse event is dirty
  public: bool mouseDirty = false;

  public: bool blockOrbit = false;

  /// \brief Mouse event
  public: common::MouseEvent mouseEvent;

  /// \brief Mouse move distance since last event.
  public: math::Vector2d drag;

  /// \brief User camera
  public: rendering::CameraPtr camera;

  /// \brief View control focus target
  public: math::Vector3d target;

  /// \brief Camera orbit controller
  public: rendering::OrbitViewController viewControl;

  /// \brief Ray query for mouse clicks
  public: rendering::RayQueryPtr rayQuery;

  //// \brief Pointer to the rendering scene
  public: rendering::ScenePtr scene = nullptr;
};

using namespace ignition;
using namespace gui;
using namespace plugins;

/////////////////////////////////////////////////
void ViewControlPrivate::OnRender()
{
  if (!this->scene)
  {
    this->scene = rendering::sceneFromFirstRenderEngine();
    if (!this->scene)
      return;

    this->camera = std::dynamic_pointer_cast<rendering::Camera>(
      this->scene->SensorByName("Scene3DCamera"));
    if (!this->camera)
    {
      ignerr << "TransformControlLogic Camera is not available" << std::endl;
      return;
    }
    this->rayQuery = this->camera->Scene()->CreateRayQuery();
  }

  if (this->blockOrbit)
  {
    this->drag = 0;
    return;
  }

  if (!this->mouseDirty)
    return;

  this->viewControl.SetCamera(this->camera);

  if (this->mouseEvent.Type() == common::MouseEvent::SCROLL)
  {
    this->target =
        this->ScreenToScene(this->mouseEvent.Pos());
    this->viewControl.SetTarget(this->target);
    double distance = this->camera->WorldPosition().Distance(
        this->target);
    double amount = -this->drag.Y() * distance / 5.0;
    this->viewControl.Zoom(amount);
  }
  else
  {
    if (this->drag == math::Vector2d::Zero)
    {
      this->target = this->ScreenToScene(
          this->mouseEvent.PressPos());
      this->viewControl.SetTarget(this->target);
    }

    // Pan with left button
    if (this->mouseEvent.Buttons() & common::MouseEvent::LEFT)
    {
      if (Qt::ShiftModifier == QGuiApplication::queryKeyboardModifiers())
        this->viewControl.Orbit(this->drag);
      else
        this->viewControl.Pan(this->drag);
    }
    // Orbit with middle button
    else if (this->mouseEvent.Buttons() & common::MouseEvent::MIDDLE)
    {
      this->viewControl.Orbit(this->drag);
    }
    else if (this->mouseEvent.Buttons() & common::MouseEvent::RIGHT)
    {
      double hfov = this->camera->HFOV().Radian();
      double vfov = 2.0f * atan(tan(hfov / 2.0f) /
          this->camera->AspectRatio());
      double distance = this->camera->WorldPosition().Distance(
          this->target);
      double amount = ((-this->drag.Y() /
          static_cast<double>(this->camera->ImageHeight()))
          * distance * tan(vfov/2.0) * 6.0);
      this->viewControl.Zoom(amount);
    }
  }
  this->drag = 0;
  this->mouseDirty = false;
}

/////////////////////////////////////////////////
math::Vector3d ViewControlPrivate::ScreenToScene(
    const math::Vector2i &_screenPos) const
{
  // Normalize point on the image
  double width = this->camera->ImageWidth();
  double height = this->camera->ImageHeight();

  double nx = 2.0 * _screenPos.X() / width - 1.0;
  double ny = 1.0 - 2.0 * _screenPos.Y() / height;

  // Make a ray query
  this->rayQuery->SetFromCamera(
      this->camera, math::Vector2d(nx, ny));

  auto result = this->rayQuery->ClosestPoint();
  if (result)
    return result.point;

  // Set point to be 10m away if no intersection found
  return this->rayQuery->Origin() +
      this->rayQuery->Direction() * 10;
}

/////////////////////////////////////////////////
ViewControl::ViewControl()
  : Plugin(), dataPtr(new ViewControlPrivate)
{
}

/////////////////////////////////////////////////
ViewControl::~ViewControl()
{
}

/////////////////////////////////////////////////
void ViewControl::LoadConfig(const tinyxml2::XMLElement *_pluginElem)
{
  ignition::gui::App()->findChild<
    ignition::gui::MainWindow *>()->installEventFilter(this);
}

/////////////////////////////////////////////////
bool ViewControl::eventFilter(QObject *_obj, QEvent *_event)
{
  if (_event->type() == events::Render::kType)
  {
    this->dataPtr->OnRender();
  }
  else if (_event->type() == events::LeftClickOnScene::kType)
  {
    auto leftClickOnScene =
      reinterpret_cast<ignition::gui::events::LeftClickOnScene *>(_event);
    this->dataPtr->mouseDirty = true;

    auto dragInt =
      leftClickOnScene->Mouse().Pos() - this->dataPtr->mouseEvent.Pos();
    auto dragDistance = math::Vector2d(dragInt.X(), dragInt.Y());

    if (leftClickOnScene->Mouse().Dragging()) {
      this->dataPtr->drag += dragDistance;
    }
    else if (leftClickOnScene->Mouse().Type() ==
      ignition::common::MouseEvent::SCROLL)
    {
      this->dataPtr->drag += math::Vector2d(
        leftClickOnScene->Mouse().Scroll().X(),
        leftClickOnScene->Mouse().Scroll().Y());
    }
    else
    {
      this->dataPtr->drag += 0;
    }

    this->dataPtr->mouseEvent = leftClickOnScene->Mouse();
  }
  else if (_event->type() == ignition::gui::events::BlockOrbit::kType)
  {
    auto blockOrbit = reinterpret_cast<ignition::gui::events::BlockOrbit *>(
      _event);
    this->dataPtr->blockOrbit = blockOrbit->Block();
  }

  // Standard event processing
  return QObject::eventFilter(_obj, _event);
}

// Register this plugin
IGNITION_ADD_PLUGIN(ignition::gui::plugins::ViewControl,
                    ignition::gui::Plugin)
