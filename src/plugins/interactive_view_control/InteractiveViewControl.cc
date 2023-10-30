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

#include <gz/msgs/boolean.pb.h>
#include <gz/msgs/double.pb.h>
#include <gz/msgs/stringmsg.pb.h>

#include <string>
#include <mutex>

#include <gz/common/MouseEvent.hh>

#include <gz/gui/Application.hh>
#include <gz/gui/GuiEvents.hh>
#include <gz/gui/MainWindow.hh>

#include <gz/plugin/Register.hh>

#include <gz/rendering/Camera.hh>
#include <gz/rendering/Geometry.hh>
#include <gz/rendering/Material.hh>
#include <gz/rendering/OrbitViewController.hh>
#include <gz/rendering/OrthoViewController.hh>
#include <gz/rendering/RenderingIface.hh>
#include <gz/rendering/RayQuery.hh>
#include <gz/rendering/Utils.hh>

#include <gz/transport/Node.hh>

#include "InteractiveViewControl.hh"

namespace gz::gui::plugins
{
/// \brief Private data class for InteractiveViewControl
class InteractiveViewControlPrivate
{
  /// \brief Perform rendering calls in the rendering thread.
  public: void OnRender();

  /// \brief Callback for camera view controller request
  /// \param[in] _msg Request message to set the camera view controller
  /// \param[out] _res Response data
  /// \return True if the request is received
  public: bool OnViewControl(const msgs::StringMsg &_msg,
    msgs::Boolean &_res);

  /// \brief Callback for camera reference visual request
  /// \param[in] _msg Request message to enable/disable the reference visual
  /// \param[out] _res Response data
  /// \return True if the request is received
  public: bool OnReferenceVisual(const msgs::Boolean &_msg,
    msgs::Boolean &_res);

  /// \brief Callback for camera view control sensitivity request
  /// \param[in] _msg Request message to set the camera view controller
  /// sensitivity. Value must be greater than zero. The higher the number
  /// the more sensitive camera control is to mouse movements. Affects all
  /// camera movements (pan, orbit, zoom)
  /// \param[out] _res Response data
  /// \return True if the request is received
  public: bool OnViewControlSensitivity(const msgs::Double &_msg,
    msgs::Boolean &_res);

  /// \brief Update the reference visual. Adjust scale based on distance from
  /// camera to target point so it remains the same size on screen.
  public: void UpdateReferenceVisual();

  /// \brief Flag to indicate if mouse event is dirty
  public: bool mouseDirty = false;

  /// \brief Flag to indicate if hover event is dirty
  public: bool hoverDirty = false;

  /// \brief Flag to indicate if mouse press event is dirty
  public: bool mousePressDirty = false;

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

  /// \brief Enable / disable reference visual
  public: bool enableRefVisual{true};

  /// \brief Camera view control service
  public: std::string cameraViewControlService;

  /// \brief Camera reference visual service
  public: std::string cameraRefVisualService;

  /// \brief Camera view control sensitivity service
  public: std::string cameraViewControlSensitivityService;

  /// \brief Ray query for mouse clicks
  public: rendering::RayQueryPtr rayQuery{nullptr};

  //// \brief Pointer to the rendering scene
  public: rendering::ScenePtr scene{nullptr};

  /// \brief Reference visual for visualizing the target point
  public: rendering::VisualPtr refVisual{nullptr};

  /// \brief Transport node for making transform control requests
  public: transport::Node node;

  /// \brief View control sensitivity value. Must be greater than 0.
  public: double viewControlSensitivity = 1.0;
};

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
          gzdbg << "InteractiveViewControl plugin is moving camera ["
                 << this->camera->Name() << "]" << std::endl;
          break;
        }
      }
    }

    if (!this->camera)
    {
      gzerr << "InteractiveViewControl camera is not available" << std::endl;
      return;
    }
    this->rayQuery = this->camera->Scene()->CreateRayQuery();
  }

  if (this->blockOrbit)
  {
    this->drag = {0, 0};
    return;
  }

  if (!this->camera)
    return;

  // hover
  if (this->hoverDirty)
  {
    if (this->refVisual)
      this->refVisual->SetVisible(false);
    this->hoverDirty = false;
  }

  if (!this->mouseDirty)
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
    gzerr << "Unknown view controller: " << this->viewController
           << ". Defaulting to orbit view controller" << std::endl;
    this->viewController = "orbit";
    this->viewControl = &this->orbitViewControl;
  }
  this->viewControl->SetCamera(this->camera);

  if (this->enableRefVisual)
  {
    if (!this->refVisual)
    {
      // create ref visual
      this->refVisual = scene->CreateVisual();
      rendering::GeometryPtr sphere = scene->CreateSphere();
      this->refVisual->AddGeometry(sphere);
      this->refVisual->SetLocalScale(math::Vector3d(0.2, 0.2, 0.1));
      this->refVisual->SetVisibilityFlags(
        GZ_VISIBILITY_GUI & ~GZ_VISIBILITY_SELECTABLE
      );

      // create material
      math::Color diffuse(1.0f, 1.0f, 0.0f, 1.0f);
      math::Color specular(1.0f, 1.0f, 0.0f, 1.0f);
      double transparency = 0.3;
      rendering::MaterialPtr material = scene->CreateMaterial();
      material->SetDiffuse(diffuse);
      material->SetSpecular(specular);
      material->SetTransparency(transparency);
      material->SetCastShadows(false);
      this->refVisual->SetMaterial(material);
      scene->DestroyMaterial(material);
    }
    this->refVisual->SetVisible(true);
  }

  if (this->mouseEvent.Type() == common::MouseEvent::SCROLL)
  {
    this->target = rendering::screenToScene(
      this->mouseEvent.Pos(), this->camera, this->rayQuery);

    this->viewControl->SetTarget(this->target);
    double distance = this->camera->WorldPosition().Distance(
        this->target);

    math::Vector2d newDrag = this->drag * this->viewControlSensitivity;
    double amount = -newDrag.Y() * distance / 5.0;
    this->viewControl->Zoom(amount);
    this->UpdateReferenceVisual();
  }
  else if (this->mouseEvent.Type() == common::MouseEvent::PRESS)
  {
    this->target = rendering::screenToScene(
      this->mouseEvent.PressPos(), this->camera, this->rayQuery);

    this->viewControl->SetTarget(this->target);
    this->UpdateReferenceVisual();
    this->mousePressDirty = false;
  }
  else
  {
    math::Vector2d newDrag = this->drag * this->viewControlSensitivity;
    // Pan with left button
    if (this->mouseEvent.Buttons() & common::MouseEvent::LEFT)
    {
      if (Qt::ShiftModifier == QGuiApplication::queryKeyboardModifiers())
        this->viewControl->Orbit(newDrag);
      else
        this->viewControl->Pan(newDrag);
      this->UpdateReferenceVisual();
    }
    // Orbit with middle button
    else if (this->mouseEvent.Buttons() & common::MouseEvent::MIDDLE)
    {
      this->viewControl->Orbit(newDrag);
      this->UpdateReferenceVisual();
    }
    // Zoom with right button
    else if (this->mouseEvent.Buttons() & common::MouseEvent::RIGHT)
    {
      double hfov = this->camera->HFOV().Radian();
      double vfov = 2.0f * atan(tan(hfov / 2.0f) / this->camera->AspectRatio());
      double distance = this->camera->WorldPosition().Distance(this->target);
      double amount = ((-newDrag.Y() /
          static_cast<double>(this->camera->ImageHeight()))
          * distance * tan(vfov/2.0) * 6.0);
      this->viewControl->Zoom(amount);
      this->UpdateReferenceVisual();
    }
  }

  this->drag = 0;
  this->mouseDirty = false;
}

/////////////////////////////////////////////////
void InteractiveViewControlPrivate::UpdateReferenceVisual()
{
  if (!this->refVisual || !this->enableRefVisual)
    return;
  this->refVisual->SetWorldPosition(this->target);
  // Update the size of the reference visual based on the distance to the
  // target point.
  double distance =
      this->camera->WorldPosition().Distance(this->target);

  double scale = distance * atan(GZ_DTOR(1.0));
  this->refVisual->SetLocalScale(
      math::Vector3d(scale, scale, scale * 0.5));
}

/////////////////////////////////////////////////
bool InteractiveViewControlPrivate::OnViewControl(const msgs::StringMsg &_msg,
  msgs::Boolean &_res)
{
  std::lock_guard<std::mutex> lock(this->mutex);

  if (_msg.data() != "orbit" && _msg.data() != "ortho")
  {
    gzwarn << "View controller type not supported [" << _msg.data() << "]"
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
bool InteractiveViewControlPrivate::OnReferenceVisual(const msgs::Boolean &_msg,
  msgs::Boolean &_res)
{
  std::lock_guard<std::mutex> lock(this->mutex);
  this->enableRefVisual = _msg.data();

  _res.set_data(true);
  return true;
}

/////////////////////////////////////////////////
bool InteractiveViewControlPrivate::OnViewControlSensitivity(
  const msgs::Double &_msg, msgs::Boolean &_res)
{
  std::lock_guard<std::mutex> lock(this->mutex);

  if (_msg.data() <= 0.0)
  {
    gzwarn << "View controller sensitivity must be greater than zero ["
            << _msg.data() << "]" << std::endl;
    _res.set_data(false);
    return true;
  }

  this->viewControlSensitivity = _msg.data();

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
  if (this->title.empty())
    this->title = "Interactive view control";

  // camera view control mode
  this->dataPtr->cameraViewControlService = "/gui/camera/view_control";
  this->dataPtr->node.Advertise(this->dataPtr->cameraViewControlService,
      &InteractiveViewControlPrivate::OnViewControl, this->dataPtr.get());
  gzmsg << "Camera view controller topic advertised on ["
         << this->dataPtr->cameraViewControlService << "]" << std::endl;

  // camera reference visual
  this->dataPtr->cameraRefVisualService =
      "/gui/camera/view_control/reference_visual";
  this->dataPtr->node.Advertise(this->dataPtr->cameraRefVisualService,
      &InteractiveViewControlPrivate::OnReferenceVisual, this->dataPtr.get());
  gzmsg << "Camera reference visual topic advertised on ["
        << this->dataPtr->cameraRefVisualService << "]" << std::endl;

  // camera view control sensitivity
  this->dataPtr->cameraViewControlSensitivityService =
      "/gui/camera/view_control/sensitivity";
  this->dataPtr->node.Advertise(
      this->dataPtr->cameraViewControlSensitivityService,
      &InteractiveViewControlPrivate::OnViewControlSensitivity,
      this->dataPtr.get());
  gzmsg << "Camera view control sensitivity advertised on ["
        << this->dataPtr->cameraViewControlSensitivityService << "]"
        << std::endl;

  gz::gui::App()->findChild<
    gz::gui::MainWindow *>()->installEventFilter(this);
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
      reinterpret_cast<gz::gui::events::LeftClickOnScene *>(_event);
    this->dataPtr->mouseDirty = true;

    this->dataPtr->drag = math::Vector2d::Zero;
    this->dataPtr->mouseEvent = leftClickOnScene->Mouse();
  }
  else if (_event->type() == events::MousePressOnScene::kType)
  {
    auto pressOnScene =
      reinterpret_cast<gz::gui::events::MousePressOnScene *>(_event);
    this->dataPtr->mouseDirty = true;
    this->dataPtr->mousePressDirty = true;

    this->dataPtr->drag = math::Vector2d::Zero;
    this->dataPtr->mouseEvent = pressOnScene->Mouse();
  }
  else if (_event->type() == events::DragOnScene::kType)
  {
    if (this->dataPtr->mousePressDirty)
      return QObject::eventFilter(_obj, _event);

    auto dragOnScene =
      reinterpret_cast<gz::gui::events::DragOnScene *>(_event);
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
      reinterpret_cast<gz::gui::events::ScrollOnScene *>(_event);
    this->dataPtr->mouseDirty = true;

    this->dataPtr->drag += math::Vector2d(
      scrollOnScene->Mouse().Scroll().X(),
      scrollOnScene->Mouse().Scroll().Y());

    this->dataPtr->mouseEvent = scrollOnScene->Mouse();
  }
  else if (_event->type() == gz::gui::events::BlockOrbit::kType)
  {
    auto blockOrbit = reinterpret_cast<gz::gui::events::BlockOrbit *>(
      _event);
    this->dataPtr->blockOrbit = blockOrbit->Block();
  }
  else if (_event->type() == gui::events::HoverOnScene::kType)
  {
    this->dataPtr->hoverDirty = true;
  }

  // Standard event processing
  return QObject::eventFilter(_obj, _event);
}
}  // namespace gz::gui::plugins

// Register this plugin
GZ_ADD_PLUGIN(gz::gui::plugins::InteractiveViewControl,
              gz::gui::Plugin)
