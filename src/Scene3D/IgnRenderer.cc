#include "IgnRenderer.hh"
#include "SceneManager.hh"

#include <ignition/gui/MainWindow.hh>
#include "ignition/gui/GuiEvents.hh"

#include <mutex>

#include <ignition/common/MouseEvent.hh>
#include <ignition/rendering/MoveToHelper.hh>

#include <ignition/math/Vector2.hh>

// TODO(louise) Remove these pragmas once ign-rendering and ign-msgs
// are disabling the warnings
#ifdef _MSC_VER
#pragma warning(push, 0)
#endif
#include <ignition/msgs.hh>

#include <ignition/rendering/Camera.hh>
#include <ignition/rendering/OrbitViewController.hh>
#include <ignition/rendering/RayQuery.hh>
#include <ignition/rendering/RenderEngine.hh>
#include <ignition/rendering/RenderingIface.hh>
#include <ignition/rendering/Scene.hh>

#ifdef _MSC_VER
#pragma warning(pop)
#endif

using namespace ignition;
using namespace gui;

namespace ignition
{
namespace gui
{
  /// \brief Private data class for IgnRenderer
  class IgnRendererPrivate
  {
    /// \brief Flag to indicate if mouse event is dirty
    public: bool mouseDirty = false;

    /// \brief Flag to indicate if hover event is dirty
    public: bool hoverDirty = false;

    /// \brief The currently hovered mouse position in screen coordinates
    public: math::Vector2i mouseHoverPos = math::Vector2i::Zero;

    /// \brief Flag for indicating whether we are spawning or not.
    public: bool isSpawning = false;

    /// \brief Flag for indicating whether the user is currently placing a
    /// resource with the shapes plugin or not
    public: bool isPlacing = false;

    /// \brief Mouse event
    public: common::MouseEvent mouseEvent;

    /// \brief Mouse move distance since last event.
    public: math::Vector2d drag;

    /// \brief Mutex to protect mouse events
    public: std::mutex mutex;

    /// \brief Target to follow
    public: std::string followTarget;

    /// \brief Wait for follow target
    public: bool followTargetWait = false;

    /// \brief Offset of camera from taget being followed
    public: math::Vector3d followOffset = math::Vector3d(-5, 0, 3);

    /// \brief Flag to indicate the follow offset needs to be updated
    public: bool followOffsetDirty = false;

    /// \brief Follow P gain
    public: double followPGain = 0.01;

    /// \brief True follow the target at an offset that is in world frame,
    /// false to follow in target's local frame
    public: bool followWorldFrame = false;

    /// \brief Target to move the user camera to
    public: std::string moveToTarget;

    /// \brief Helper object to move user camera
    public: ignition::rendering::MoveToHelper moveToHelper;

    /// \brief Last move to animation time
    public: std::chrono::time_point<std::chrono::system_clock> prevMoveToTime;

    /// \brief User camera
    public: rendering::CameraPtr camera;

    /// \brief Camera orbit controller
    public: rendering::OrbitViewController viewControl;

    /// \brief Ray query for mouse clicks
    public: rendering::RayQueryPtr rayQuery;

    /// \brief Scene requester to get scene info
    public: ignition::gui::SceneManager sceneManager;

    /// \brief View control focus target
    public: math::Vector3d target;
  };
}
}

/////////////////////////////////////////////////
IgnRenderer::IgnRenderer()
  : dataPtr(new IgnRendererPrivate)
{
}


/////////////////////////////////////////////////
IgnRenderer::~IgnRenderer()
{
}

/////////////////////////////////////////////////
void IgnRenderer::Render()
{
  if (this->textureDirty)
  {
    this->dataPtr->camera->SetImageWidth(this->textureSize.width());
    this->dataPtr->camera->SetImageHeight(this->textureSize.height());
    this->dataPtr->camera->SetAspectRatio(this->textureSize.width() /
        static_cast<double>(this->textureSize.height()));
    // setting the size should cause the render texture to be rebuilt
    {
      // IGN_PROFILE("IgnRenderer::Render Pre-render camera");
      this->dataPtr->camera->PreRender();
    }
    this->textureId = this->dataPtr->camera->RenderTextureGLId();
    this->textureDirty = false;
  }

  // update the scene
  this->dataPtr->sceneManager.Update();

  // view control
  this->HandleMouseEvent();

  // reset follow mode if target node got removed
  if (!this->dataPtr->followTarget.empty())
  {
    rendering::NodePtr target =
      this->dataPtr->sceneManager.GetScene()->NodeByName(
        this->dataPtr->followTarget);
    if (!target && !this->dataPtr->followTargetWait)
    {
      this->dataPtr->camera->SetFollowTarget(nullptr);
      this->dataPtr->camera->SetTrackTarget(nullptr);
      this->dataPtr->followTarget.clear();
      emit FollowTargetChanged(std::string(), false);
    }
  }

  // update and render to texture
  this->dataPtr->camera->Update();

  // move to
  if (!this->dataPtr->moveToTarget.empty())
  {
    if (this->dataPtr->moveToHelper.Idle())
    {
      rendering::NodePtr target = this->dataPtr->sceneManager.GetScene()->NodeByName(
          this->dataPtr->moveToTarget);

      ignerr << "this->dataPtr->moveToTarget " << this->dataPtr->moveToTarget << std::endl;

      if (target)
      {
        this->dataPtr->moveToHelper.MoveTo(this->dataPtr->camera, target, 0.5,
            std::bind(&IgnRenderer::OnMoveToComplete, this));
        this->dataPtr->prevMoveToTime = std::chrono::system_clock::now();
      }
      else
      {
        ignerr << "Unable to move to target. Target: '"
               << this->dataPtr->moveToTarget << "' not found" << std::endl;
        this->dataPtr->moveToTarget.clear();
      }
    }
    else
    {
      auto now = std::chrono::system_clock::now();
      std::chrono::duration<double> dt = now - this->dataPtr->prevMoveToTime;
      this->dataPtr->moveToHelper.AddTime(dt.count());
      this->dataPtr->prevMoveToTime = now;
    }
  }

  // Follow
  rendering::NodePtr followTarget = this->dataPtr->camera->FollowTarget();
  if (!this->dataPtr->followTarget.empty())
  {
    // for (int i = 0; i < this->dataPtr->sceneManager.GetScene()->NodeCount(); i++)
    // {
    //   ignerr << this->dataPtr->sceneManager.GetScene()->NodeByIndex(i)->Name() << std::endl;;
    // }

    rendering::NodePtr target = this->dataPtr->sceneManager.GetScene()->NodeByName(
        this->dataPtr->followTarget);

    if (target != nullptr)
    {
      if (!followTarget || target != followTarget)
      {
        this->dataPtr->camera->SetFollowTarget(target,
            this->dataPtr->followOffset,
            this->dataPtr->followWorldFrame);
        this->dataPtr->camera->SetFollowPGain(this->dataPtr->followPGain);

        this->dataPtr->camera->SetTrackTarget(target);
        // found target, no need to wait anymore
        this->dataPtr->followTargetWait = false;
      }
      else if (this->dataPtr->followOffsetDirty)
      {
        math::Vector3d offset =
            this->dataPtr->camera->WorldPosition() - target->WorldPosition();
        if (!this->dataPtr->followWorldFrame)
        {
          offset = target->WorldRotation().RotateVectorReverse(offset);
        }
        this->dataPtr->camera->SetFollowOffset(offset);
        this->dataPtr->followOffsetDirty = false;
      }
    }
    else if (!this->dataPtr->followTargetWait)
    {
      ignerr << "Unable to follow target. Target: '"
             << this->dataPtr->followTarget << "' not found" << std::endl;
      this->dataPtr->followTarget.clear();
    }
  }
  else if (followTarget)
  {
    this->dataPtr->camera->SetFollowTarget(nullptr);
    this->dataPtr->camera->SetTrackTarget(nullptr);
  }

  if (ignition::gui::App())
  {
    ignition::gui::App()->sendEvent(
        ignition::gui::App()->findChild<ignition::gui::MainWindow *>(),
        new gui::events::Render());
  }
}

/////////////////////////////////////////////////
void IgnRenderer::OnMoveToComplete()
{
  std::lock_guard<std::mutex> lock(this->dataPtr->mutex);
  this->dataPtr->moveToTarget.clear();
}

/////////////////////////////////////////////////
void IgnRenderer::HandleMouseEvent()
{
  std::lock_guard<std::mutex> lock(this->dataPtr->mutex);
  this->BroadcastHoverPos();
  this->HandleModelPlacement();
  this->HandleMouseViewControl();
}

/////////////////////////////////////////////////
void IgnRenderer::HandleModelPlacement()
{
}

/////////////////////////////////////////////////
void IgnRenderer::BroadcastHoverPos()
{
  if (this->dataPtr->hoverDirty)
  {
    math::Vector3d pos = this->ScreenToScene(this->dataPtr->mouseHoverPos);

    ignition::gui::events::HoverToScene hoverToSceneEvent(pos);
    ignition::gui::App()->sendEvent(
        ignition::gui::App()->findChild<ignition::gui::MainWindow *>(),
        &hoverToSceneEvent);
  }
}

/////////////////////////////////////////////////
void IgnRenderer::HandleMouseViewControl()
{
  if (!this->dataPtr->mouseDirty)
    return;

  math::Vector3d camWorldPos;
  if (!this->dataPtr->followTarget.empty())
    this->dataPtr->camera->WorldPosition();

  this->dataPtr->viewControl.SetCamera(this->dataPtr->camera);

  if (this->dataPtr->mouseEvent.Type() == common::MouseEvent::SCROLL)
  {
    this->dataPtr->target =
        this->ScreenToScene(this->dataPtr->mouseEvent.Pos());
    this->dataPtr->viewControl.SetTarget(this->dataPtr->target);
    double distance = this->dataPtr->camera->WorldPosition().Distance(
        this->dataPtr->target);
    double amount = -this->dataPtr->drag.Y() * distance / 5.0;
    this->dataPtr->viewControl.Zoom(amount);
  }
  else
  {
    if (this->dataPtr->mouseEvent.Type() == common::MouseEvent::PRESS)
    {
      this->dataPtr->target = this->ScreenToScene(
          this->dataPtr->mouseEvent.PressPos());
      this->dataPtr->viewControl.SetTarget(this->dataPtr->target);
    }
    // Pan with left button
    if (this->dataPtr->mouseEvent.Buttons() & common::MouseEvent::LEFT)
    {
      if (Qt::ShiftModifier == QGuiApplication::queryKeyboardModifiers())
        this->dataPtr->viewControl.Orbit(this->dataPtr->drag);
      else
        this->dataPtr->viewControl.Pan(this->dataPtr->drag);
    }
    // Orbit with middle button
    else if (this->dataPtr->mouseEvent.Buttons() & common::MouseEvent::MIDDLE)
    {
      this->dataPtr->viewControl.Orbit(this->dataPtr->drag);
    }
    else if (this->dataPtr->mouseEvent.Buttons() & common::MouseEvent::RIGHT)
    {
      double hfov = this->dataPtr->camera->HFOV().Radian();
      double vfov = 2.0f * atan(tan(hfov / 2.0f) /
          this->dataPtr->camera->AspectRatio());
      double distance = this->dataPtr->camera->WorldPosition().Distance(
          this->dataPtr->target);
      double amount = ((-this->dataPtr->drag.Y() /
          static_cast<double>(this->dataPtr->camera->ImageHeight()))
          * distance * tan(vfov/2.0) * 6.0);
      this->dataPtr->viewControl.Zoom(amount);
    }
  }
  this->dataPtr->drag = 0;
  this->dataPtr->mouseDirty = false;

  if (!this->dataPtr->followTarget.empty())
  {
    math::Vector3d dPos = this->dataPtr->camera->WorldPosition() - camWorldPos;
    if (dPos != math::Vector3d::Zero)
    {
      this->dataPtr->followOffsetDirty = true;
    }
  }
}

/////////////////////////////////////////////////
void IgnRenderer::Initialize()
{
  if (this->initialized)
    return;

  std::map<std::string, std::string> params;
  params["useCurrentGLContext"] = "1";
  auto engine = rendering::engine(this->engineName, params);
  if (!engine)
  {
    ignerr << "Engine [" << this->engineName << "] is not supported"
           << std::endl;
    return;
  }

  // Scene
  auto scene = engine->SceneByName(this->sceneName);
  if (!scene)
  {
    igndbg << "Create scene! [" << this->sceneName << "]" << std::endl;
    scene = engine->CreateScene(this->sceneName);
    scene->SetAmbientLight(this->ambientLight);
    scene->SetBackgroundColor(this->backgroundColor);
    scene->SetSkyEnabled(this->sky);
  }

  auto root = scene->RootVisual();

  // Camera
  this->dataPtr->camera = scene->CreateCamera();
  root->AddChild(this->dataPtr->camera);
  this->dataPtr->camera->SetLocalPose(this->cameraPose);
  this->dataPtr->camera->SetImageWidth(this->textureSize.width());
  this->dataPtr->camera->SetImageHeight(this->textureSize.height());
  this->dataPtr->camera->SetAntiAliasing(8);
  this->dataPtr->camera->SetHFOV(M_PI * 0.5);
  this->dataPtr->camera->SetVisibilityMask(this->visibilityMask);
  // setting the size and calling PreRender should cause the render texture to
  //  be rebuilt
  this->dataPtr->camera->PreRender();
  this->textureId = this->dataPtr->camera->RenderTextureGLId();

  igndbg << "Service name [" << this->sceneService << "]" << std::endl;

  // Make service call to populate scene
  if (!this->sceneService.empty())
  {
    this->dataPtr->sceneManager.Load(this->sceneService, this->poseTopic,
                                     this->deletionTopic, this->sceneTopic,
                                     scene);
    this->dataPtr->sceneManager.Request();
  }

  // Ray Query
  this->dataPtr->rayQuery = this->dataPtr->camera->Scene()->CreateRayQuery();

  this->initialized = true;
}

/////////////////////////////////////////////////
void IgnRenderer::Destroy()
{
  auto engine = rendering::engine(this->engineName);
  if (!engine)
    return;
  auto scene = engine->SceneByName(this->sceneName);
  if (!scene)
    return;
  scene->DestroySensor(this->dataPtr->camera);

  // If that was the last sensor, destroy scene
  if (scene->SensorCount() == 0)
  {
    igndbg << "Destroy scene [" << scene->Name() << "]" << std::endl;
    engine->DestroyScene(scene);

    // TODO(anyone) If that was the last scene, terminate engine?
  }
}

/////////////////////////////////////////////////
void IgnRenderer::SetMoveTo(const std::string &_target)
{
  std::lock_guard<std::mutex> lock(this->dataPtr->mutex);
  this->dataPtr->moveToTarget = _target;
}

void IgnRenderer::SetFollowWorldFrame(bool _worldFrame)
{
  std::lock_guard<std::mutex> lock(this->dataPtr->mutex);
  this->dataPtr->followWorldFrame = _worldFrame;
}

/////////////////////////////////////////////////
void IgnRenderer::SetFollowOffset(const math::Vector3d &_offset)
{
  std::lock_guard<std::mutex> lock(this->dataPtr->mutex);
  this->dataPtr->followOffset = _offset;
}

/////////////////////////////////////////////////
void IgnRenderer::SetFollowTarget(const std::string &_target,
    bool _waitForTarget)
{
  std::lock_guard<std::mutex> lock(this->dataPtr->mutex);
  this->dataPtr->followTarget = _target;
  this->dataPtr->followTargetWait = _waitForTarget;
}

/////////////////////////////////////////////////
void IgnRenderer::SetFollowPGain(double _gain)
{
  std::lock_guard<std::mutex> lock(this->dataPtr->mutex);
  this->dataPtr->followPGain = _gain;
}

/////////////////////////////////////////////////
void IgnRenderer::NewMouseEvent(const common::MouseEvent &_e,
    const math::Vector2d &_drag)
{
  std::lock_guard<std::mutex> lock(this->dataPtr->mutex);
  this->dataPtr->mouseEvent = _e;
  this->dataPtr->drag += _drag;
  this->dataPtr->mouseDirty = true;
}

/////////////////////////////////////////////////
math::Vector3d IgnRenderer::ScreenToScene(
    const math::Vector2i &_screenPos) const
{
  // Normalize point on the image
  double width = this->dataPtr->camera->ImageWidth();
  double height = this->dataPtr->camera->ImageHeight();

  double nx = 2.0 * _screenPos.X() / width - 1.0;
  double ny = 1.0 - 2.0 * _screenPos.Y() / height;

  // Make a ray query
  this->dataPtr->rayQuery->SetFromCamera(
      this->dataPtr->camera, math::Vector2d(nx, ny));

  auto result = this->dataPtr->rayQuery->ClosestPoint();
  if (result)
    return result.point;

  // Set point to be 10m away if no intersection found
  return this->dataPtr->rayQuery->Origin() +
      this->dataPtr->rayQuery->Direction() * 10;
}
