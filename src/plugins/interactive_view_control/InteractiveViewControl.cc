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

#include <string>
#include <mutex>

#include <ignition/common/MouseEvent.hh>

#include <ignition/gui/Application.hh>
#include <ignition/gui/GuiEvents.hh>
#include <ignition/gui/MainWindow.hh>

#include <ignition/plugin/Register.hh>

#include <ignition/rendering/Camera.hh>
#include <ignition/rendering/OrbitViewController.hh>
#include <ignition/rendering/OrthoViewController.hh>
#include <ignition/rendering/RenderingIface.hh>
#include <ignition/rendering/RayQuery.hh>
#include <ignition/rendering/Utils.hh>

#include <ignition/transport/Node.hh>

#include "InteractiveViewControl.hh"

/// \brief Private data class for InteractiveViewControl
class ignition::gui::plugins::InteractiveViewControlPrivate
{
  /// \brief Perform rendering calls in the rendering thread.
  public: void OnRender();

  /// \brief Callback for camera view controller request
  /// \param[in] _msg Request message to set the camera view controller
  /// \param[in] _res Response data
  /// \return True if the request is received
  public: bool OnViewControl(const msgs::StringMsg &_msg,
    msgs::Boolean &_res);

  /// \brief Flag to indicate if mouse event is dirty
  public: bool mouseDirty = false;

  /// \brief True to block orbiting with the mouse.
  public: bool blockOrbit = false;

  /// \brief Mouse event
  public: common::MouseEvent mouseEvent;

  /// \brief Mouse move distance since last event.
  public: math::Vector2d drag;

  /// \brief User camera
  public: rendering::CameraPtr camera{nullptr};

  /// \brief View control focus target
  public: math::Vector3d target;

  /// \brief Orbit view controller
  public: rendering::OrbitViewController orbitViewControl;

  /// \brief Ortho view controller
  public: rendering::OrthoViewController orthoViewControl;

  /// \brief Camera view controller
  public: rendering::ViewController *viewControl{nullptr};

  /// \brief Mutex to protect View Controllers
  public: std::mutex mutex;

  /// \brief View controller
  public: std::string viewController{"orbit"};

  /// \brief Camera view control service
  public: std::string cameraViewControlService;

  /// \brief Ray query for mouse clicks
  public: rendering::RayQueryPtr rayQuery{nullptr};

  //// \brief Pointer to the rendering scene
  public: rendering::ScenePtr scene{nullptr};

  /// \brief Transport node for making transform control requests
  public: transport::Node node;
};

using namespace ignition;
using namespace gui;
using namespace plugins;

/////////////////////////////////////////////////
void InteractiveViewControlPrivate::OnRender()
{
  if (!this->scene)
  {
    this->scene = rendering::sceneFromFirstRenderEngine();
    if (!this->scene)
      return;

    for (unsigned int i = 0; i < this->scene->NodeCount(); ++i)
    {
      auto cam = std::dynamic_pointer_cast<rendering::Camera>(
        this->scene->NodeByIndex(i));
      if (cam)
      {
        bool isUserCamera = false;
        try
        {
          isUserCamera = std::get<bool>(cam->UserData("user-camera"));
        }
        catch (std::bad_variant_access &)
        {
          continue;
        }
        if (isUserCamera)
        {
          this->camera = cam;
          igndbg << "InteractiveViewControl plugin is moving camera ["
                 << this->camera->Name() << "]" << std::endl;
          break;
        }
      }
    }

    if (!this->camera)
    {
      ignerr << "InteractiveViewControl camera is not available" << std::endl;
      return;
    }
    this->rayQuery = this->camera->Scene()->CreateRayQuery();
  }

  if (this->blockOrbit)
  {
    this->drag = {0, 0};
    return;
  }

  if (!this->mouseDirty)
    return;

  if (!this->camera)
    return;

  std::lock_guard<std::mutex> lock(this->mutex);

  if (this->viewController == "ortho")
  {
    this->viewControl = &this->orthoViewControl;
  }
  else if (this->viewController == "orbit")
  {
    this->viewControl = &this->orbitViewControl;
  }
  else
  {
    ignerr << "Unknown view controller: " << this->viewController
           << ". Defaulting to orbit view controller" << std::endl;
    this->viewController = "orbit";
    this->viewControl = &this->orbitViewControl;
  }
  this->viewControl->SetCamera(this->camera);

  if (this->mouseEvent.Type() == common::MouseEvent::SCROLL)
  {
    this->target = rendering::screenToScene(
      this->mouseEvent.Pos(), this->camera, this->rayQuery);

    this->viewControl->SetTarget(this->target);
    double distance = this->camera->WorldPosition().Distance(
        this->target);
    double amount = -this->drag.Y() * distance / 5.0;
    this->viewControl->Zoom(amount);
  }
  else if (this->mouseEvent.Type() == common::MouseEvent::PRESS)
  {
    this->target = rendering::screenToScene(
      this->mouseEvent.PressPos(), this->camera, this->rayQuery);
    this->viewControl->SetTarget(this->target);
  }
  else
  {
    // Pan with left button
    if (this->mouseEvent.Buttons() & common::MouseEvent::LEFT)
    {
      if (Qt::ShiftModifier == QGuiApplication::queryKeyboardModifiers())
        this->viewControl->Orbit(this->drag);
      else
        this->viewControl->Pan(this->drag);
    }
    // Orbit with middle button
    else if (this->mouseEvent.Buttons() & common::MouseEvent::MIDDLE)
    {
      this->viewControl->Orbit(this->drag);
    }
    // Zoom with right button
    else if (this->mouseEvent.Buttons() & common::MouseEvent::RIGHT)
    {
      double hfov = this->camera->HFOV().Radian();
      double vfov = 2.0f * atan(tan(hfov / 2.0f) / this->camera->AspectRatio());
      double distance = this->camera->WorldPosition().Distance(this->target);
      double amount = ((-this->drag.Y() /
          static_cast<double>(this->camera->ImageHeight()))
          * distance * tan(vfov/2.0) * 6.0);
      this->viewControl->Zoom(amount);
    }
  }
  this->drag = 0;
  this->mouseDirty = false;
}

/////////////////////////////////////////////////
bool InteractiveViewControlPrivate::OnViewControl(const msgs::StringMsg &_msg,
  msgs::Boolean &_res)
{
  std::lock_guard<std::mutex> lock(this->mutex);

  if (_msg.data() != "orbit" && _msg.data() != "ortho")
  {
    ignwarn << "View controller type not supported [" << _msg.data() << "]"
            << std::endl;
    _res.set_data(false);
    return true;
  }

  this->viewController = _msg.data();

  // mark mouse dirty to trigger HandleMouseEvent call and
  // set up a new view controller
  this->mouseDirty = true;

  _res.set_data(true);
  return true;
}

/////////////////////////////////////////////////
InteractiveViewControl::InteractiveViewControl()
  : Plugin(), dataPtr(std::make_unique<InteractiveViewControlPrivate>())
{
}

/////////////////////////////////////////////////
InteractiveViewControl::~InteractiveViewControl() = default;

/////////////////////////////////////////////////
void InteractiveViewControl::LoadConfig(
  const tinyxml2::XMLElement * /*_pluginElem*/)
{
  // camera view control mode
  this->dataPtr->cameraViewControlService = "/gui/camera/view_control";
  this->dataPtr->node.Advertise(this->dataPtr->cameraViewControlService,
      &InteractiveViewControlPrivate::OnViewControl, this->dataPtr.get());
  ignmsg << "Camera view controller topic advertised on ["
         << this->dataPtr->cameraViewControlService << "]" << std::endl;

  ignition::gui::App()->findChild<
    ignition::gui::MainWindow *>()->installEventFilter(this);
}

/////////////////////////////////////////////////
bool InteractiveViewControl::eventFilter(QObject *_obj, QEvent *_event)
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

    this->dataPtr->drag = math::Vector2d::Zero;
    this->dataPtr->mouseEvent = leftClickOnScene->Mouse();
  }
  else if (_event->type() == events::MousePressOnScene::kType)
  {
    auto pressOnScene =
      reinterpret_cast<ignition::gui::events::MousePressOnScene *>(_event);
    this->dataPtr->mouseDirty = true;

    this->dataPtr->drag = math::Vector2d::Zero;
    this->dataPtr->mouseEvent = pressOnScene->Mouse();
  }
  else if (_event->type() == events::DragOnScene::kType)
  {
    auto dragOnScene =
      reinterpret_cast<ignition::gui::events::DragOnScene *>(_event);
    this->dataPtr->mouseDirty = true;

    auto dragStart = this->dataPtr->mouseEvent.Pos();
    auto dragInt = dragOnScene->Mouse().Pos() - dragStart;
    auto dragDistance = math::Vector2d(dragInt.X(), dragInt.Y());

    this->dataPtr->drag += dragDistance;

    this->dataPtr->mouseEvent = dragOnScene->Mouse();
  }
  else if (_event->type() == events::ScrollOnScene::kType)
  {
    auto scrollOnScene =
      reinterpret_cast<ignition::gui::events::ScrollOnScene *>(_event);
    this->dataPtr->mouseDirty = true;

    this->dataPtr->drag += math::Vector2d(
      scrollOnScene->Mouse().Scroll().X(),
      scrollOnScene->Mouse().Scroll().Y());

    this->dataPtr->mouseEvent = scrollOnScene->Mouse();
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
IGNITION_ADD_PLUGIN(ignition::gui::plugins::InteractiveViewControl,
                    ignition::gui::Plugin)
