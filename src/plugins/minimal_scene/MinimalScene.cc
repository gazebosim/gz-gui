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

#include <gz/msgs/boolean.pb.h>
#include <gz/msgs/stringmsg.pb.h>

#include "MinimalScene.hh"
#include "MinimalSceneRhi.hh"
#include "MinimalSceneRhiMetal.hh"
#include "MinimalSceneRhiOpenGL.hh"

#include <algorithm>
#include <list>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include <gz/common/Console.hh>
#include <gz/common/KeyEvent.hh>
#include <gz/common/MouseEvent.hh>
#include <gz/math/Vector2.hh>
#include <gz/math/Vector3.hh>
#include <gz/plugin/Register.hh>
#include <gz/rendering/Camera.hh>
#include <gz/rendering/RayQuery.hh>
#include <gz/rendering/RenderEngine.hh>
#include <gz/rendering/RenderingIface.hh>
#include <gz/rendering/Scene.hh>
#include <gz/rendering/Utils.hh>
#include <gz/transport/Node.hh>

#include "gz/gui/Application.hh"
#include "gz/gui/Conversions.hh"
#include "gz/gui/GuiEvents.hh"
#include "gz/gui/Helpers.hh"
#include "gz/gui/MainWindow.hh"

Q_DECLARE_METATYPE(gz::gui::plugins::RenderSync*)

/// \brief Private data class for GzRenderer
class gz::gui::plugins::GzRenderer::Implementation
{
  /// \brief Flag to indicate if mouse event is dirty
  public: bool mouseDirty{false};

  /// \brief Flag to indicate if hover event is dirty
  public: bool hoverDirty{false};

  /// \brief Flag to indicate if drop event is dirty
  public: bool dropDirty{false};

  /// \brief Current mouse event
  public: common::MouseEvent mouseEvent;

  /// \brief A list of mouse events
  public: std::list<common::MouseEvent> mouseEvents;

  /// \brief Key event
  public: common::KeyEvent keyEvent;

  /// \brief Max number of mouse events to store in the queue.
  /// These events are then propagated to other gui plugins. A queue is used
  /// instead of just keeping the latest mouse event so that we can capture
  /// important events like mouse presses. However we keep the queue size
  /// small on purpose so that we do not flood other gui plugins with events
  /// that may be outdated.
  public: const unsigned int kMaxMouseEventSize = 5u;

  /// \brief Mutex to protect mouse events
  public: std::mutex mutex;

  /// \brief User camera
  public: rendering::CameraPtr camera{nullptr};

  /// \brief The currently hovered mouse position in screen coordinates
  public: math::Vector2i mouseHoverPos{math::Vector2i::Zero};

  /// \brief The currently drop mouse position in screen coordinates
  public: math::Vector2i mouseDropPos{math::Vector2i::Zero};

  /// \brief The dropped text in the scene
  public: std::string dropText{""};

  /// \brief Ray query for mouse clicks
  public: rendering::RayQueryPtr rayQuery{nullptr};

  /// \brief View control focus target
  public: math::Vector3d target;

  /// \brief Render system parameters
  public: std::map<std::string, std::string> rhiParams;

  /// \brief Render hardware interface for the texture
  public: std::unique_ptr<GzCameraTextureRhi> rhi;
};

/// \brief Qt and Ogre rendering is happening in different threads
/// The original sample 'textureinthread' from Qt used a double-buffer
/// scheme so that the worker (Ogre) thread write to FBO A, while
/// Qt is displaying FBO B.
///
/// However Qt's implementation doesn't handle all the edge cases
/// (like resizing a window), and also it increases our VRAM
/// consumption in multiple ways (since we have to double other
/// resources as well or re-architect certain parts of the code
/// to avoid it)
///
/// Thus we just serialize both threads so that when Qt reaches
/// drawing preparation, it halts and Ogre worker thread starts rendering,
/// then resumes when Ogre is done.
///
/// This code is admitedly more complicated than it should be
/// because Qt's synchronization using signals and slots causes
/// deadlocks when other means of synchronization are introduced.
/// The whole threaded loop should be rewritten.
///
/// All RenderSync does is conceptually:
///
/// \code
///   TextureNode::PrepareNode()
///   {
///     renderSync.WaitForWorkerThread(); // Qt thread
///       // WaitForQtThreadAndBlock();
///       // Now worker thread begins executing what's between
///       // ReleaseQtThreadFromBlock();
///     continue with qt code...
///   }
/// \endcode
///
///
/// For more info see
/// https://github.com/gazebosim/gz-rendering/issues/304
class gz::gui::plugins::RenderSync
{
  /// \brief Cond. variable to synchronize rendering on specific events
  /// (e.g. texture resize) or for debugging (e.g. keep
  /// all API calls sequential)
  public: std::mutex mutex;

  /// \brief Cond. variable to synchronize rendering on specific events
  /// (e.g. texture resize) or for debugging (e.g. keep
  /// all API calls sequential)
  public: std::condition_variable cv;

  public: enum class RenderStallState
          {
            /// Qt is stuck inside WaitForWorkerThread
            /// Worker thread can proceed
            WorkerCanProceed,
            /// Qt is stuck inside WaitForWorkerThread
            /// Worker thread is between WaitForQtThreadAndBlock
            /// and ReleaseQtThreadFromBlock
            WorkerIsProceeding,
            /// Worker is stuck inside WaitForQtThreadAndBlock
            /// Qt can proceed
            QtCanProceed,
            /// Do not block
            ShuttingDown,
          };

  /// \brief See TextureNode::RenderSync::RenderStallState
  public: RenderStallState renderStallState =
      RenderStallState::QtCanProceed /*GUARDED_BY(sharedRenderMutex)*/;

  /// \brief Must be called from worker thread when we want to block
  /// \param[in] lock Acquired lock. Must be based on this->mutex
  public: void WaitForQtThreadAndBlock(std::unique_lock<std::mutex> &_lock);

  /// \brief Must be called from worker thread when we are done
  /// \param[in] lock Acquired lock. Must be based on this->mutex
  public: void ReleaseQtThreadFromBlock(std::unique_lock<std::mutex> &_lock);

  /// \brief Must be called from Qt thread periodically
  public: void WaitForWorkerThread();

  /// \brief Must be called from GUI thread when shutting down
  public: void Shutdown();
};

/// \brief Private data class for RenderWindowItem
class gz::gui::plugins::RenderWindowItem::Implementation
{
  /// \brief Keep latest mouse event
  public: common::MouseEvent mouseEvent;

  /// \brief True if initialized
  public: bool initialized = false;

  /// \brief True if initializing (started but not complete)
  public: bool initializing = false;

  /// \brief Graphics API. The default is platform specific.
  public: gz::rendering::GraphicsAPI graphicsAPI =
#ifdef __APPLE__
      rendering::GraphicsAPI::METAL;
#else
      rendering::GraphicsAPI::OPENGL;
#endif

  /// \brief Render thread
  public: RenderThread *renderThread = nullptr;

  /// \brief See RenderSync
  public: RenderSync renderSync;

  /// \brief List of threads
  public: static QList<QThread *> threads;

  /// \brief List of our QT connections.
  public: QList<QMetaObject::Connection> connections;
};

/// \brief Private data class for MinimalScene
class gz::gui::plugins::MinimalScene::Implementation
{
};

using namespace gz;
using namespace gui;
using namespace plugins;

QList<QThread *> RenderWindowItem::Implementation::threads;

/////////////////////////////////////////////////
void RenderSync::WaitForQtThreadAndBlock(std::unique_lock<std::mutex> &_lock)
{
  this->cv.wait(_lock, [this]
  { return this->renderStallState == RenderStallState::WorkerCanProceed ||
           this->renderStallState == RenderStallState::ShuttingDown; });

  this->renderStallState = RenderStallState::WorkerIsProceeding;
}

/////////////////////////////////////////////////
void RenderSync::ReleaseQtThreadFromBlock(std::unique_lock<std::mutex> &_lock)
{
  this->renderStallState = RenderStallState::QtCanProceed;
  _lock.unlock();
  this->cv.notify_one();
}

/////////////////////////////////////////////////
void RenderSync::WaitForWorkerThread()
{
  std::unique_lock<std::mutex> lock(this->mutex);

  // Wait until we're clear to go
  this->cv.wait( lock, [this]
  {
    return this->renderStallState == RenderStallState::QtCanProceed ||
           this->renderStallState == RenderStallState::ShuttingDown;
  } );

  // Worker thread asked us to wait!
  this->renderStallState = RenderStallState::WorkerCanProceed;
  lock.unlock();
  // Wake up worker thread
  this->cv.notify_one();
  lock.lock();

  // Wait until we're clear to go
  this->cv.wait( lock, [this]
  {
    return this->renderStallState == RenderStallState::QtCanProceed ||
           this->renderStallState == RenderStallState::ShuttingDown;
  } );
}

/////////////////////////////////////////////////
void RenderSync::Shutdown()
{
  {
    std::unique_lock<std::mutex> lock(this->mutex);

    this->renderStallState = RenderStallState::ShuttingDown;

    lock.unlock();
    this->cv.notify_one();
  }
}

/////////////////////////////////////////////////
GzRenderer::GzRenderer()
  : dataPtr(utils::MakeUniqueImpl<Implementation>())
{
  // Set default graphics API to OpenGL
  this->SetGraphicsAPI(rendering::GraphicsAPI::OPENGL);
}

/////////////////////////////////////////////////
void GzRenderer::Render(RenderSync *_renderSync)
{
  std::unique_lock<std::mutex> lock(_renderSync->mutex);
  _renderSync->WaitForQtThreadAndBlock(lock);

  if (this->textureDirty)
  {
    // TODO(anyone) If SwapFromThread gets implemented,
    // then we only need to lock when texture is dirty
    // (but we still need to lock the whole routine if
    // debugging from RenderDoc or if user is not willing
    // to sacrifice VRAM)
    //
    // std::unique_lock<std::mutex> lock(renderSync->mutex);
    // _renderSync->WaitForQtThreadAndBlock(lock);
    this->dataPtr->camera->SetImageWidth(this->textureSize.width());
    this->dataPtr->camera->SetImageHeight(this->textureSize.height());
    this->dataPtr->camera->SetHFOV(this->cameraHFOV);
    // setting the size should cause the render texture to be rebuilt
    this->dataPtr->camera->PreRender();
    this->textureDirty = false;

    // TODO(anyone) See SwapFromThread comments
    // _renderSync->ReleaseQtThreadFromBlock(lock);
  }

  // Update the render interface (texture)
  this->dataPtr->rhi->Update(this->dataPtr->camera);

  // view control
  this->HandleMouseEvent();

  if (gz::gui::App())
  {
    gz::gui::App()->sendEvent(
        gz::gui::App()->findChild<gz::gui::MainWindow *>(),
        new gui::events::PreRender());
  }

  // update and render to texture
  this->dataPtr->camera->Update();

  if (!this->cameraViewController.empty())
  {
    std::string viewControlService = "/gui/camera/view_control";
    transport::Node node;
    std::function<void(const msgs::Boolean &, const bool)> cb =
        [&](const msgs::Boolean &/*_rep*/, const bool _result)
    {
      if (!_result)
      {
        // LCOV_EXCL_START
        gzerr << "Error setting view controller. Check if the View Angle GUI "
                 "plugin is loaded." << std::endl;
        // LCOV_EXCL_STOP
      }
      this->cameraViewController = "";
    };

    msgs::StringMsg req;
    req.set_data(this->cameraViewController);
    node.Request(viewControlService, req, cb);
  }

  if (gz::gui::App())
  {
    gz::gui::App()->sendEvent(
        gz::gui::App()->findChild<gz::gui::MainWindow *>(),
        new gui::events::Render());
  }
  _renderSync->ReleaseQtThreadFromBlock(lock);
}

/////////////////////////////////////////////////
void GzRenderer::HandleMouseEvent()
{
  std::lock_guard<std::mutex> lock(this->dataPtr->mutex);
  for (const auto &e : this->dataPtr->mouseEvents)
  {
    this->dataPtr->mouseEvent = e;

    this->BroadcastDrag();
    this->BroadcastMousePress();
    this->BroadcastLeftClick();
    this->BroadcastRightClick();
    this->BroadcastScroll();
    this->BroadcastKeyPress();
    this->BroadcastKeyRelease();
  }
  this->dataPtr->mouseEvents.clear();

  this->BroadcastHoverPos();
  this->BroadcastDrop();
  this->dataPtr->mouseDirty = false;
}

////////////////////////////////////////////////
void GzRenderer::HandleKeyPress(const common::KeyEvent &_e)
{
  std::lock_guard<std::mutex> lock(this->dataPtr->mutex);

  this->dataPtr->keyEvent = _e;

  this->dataPtr->mouseEvent.SetControl(this->dataPtr->keyEvent.Control());
  this->dataPtr->mouseEvent.SetShift(this->dataPtr->keyEvent.Shift());
  this->dataPtr->mouseEvent.SetAlt(this->dataPtr->keyEvent.Alt());
}

////////////////////////////////////////////////
void GzRenderer::HandleKeyRelease(const common::KeyEvent &_e)
{
  std::lock_guard<std::mutex> lock(this->dataPtr->mutex);

  this->dataPtr->keyEvent = _e;

  this->dataPtr->mouseEvent.SetControl(this->dataPtr->keyEvent.Control());
  this->dataPtr->mouseEvent.SetShift(this->dataPtr->keyEvent.Shift());
  this->dataPtr->mouseEvent.SetAlt(this->dataPtr->keyEvent.Alt());
}

/////////////////////////////////////////////////
void GzRenderer::BroadcastDrop()
{
  if (!this->dataPtr->dropDirty)
    return;
  events::DropOnScene dropOnSceneEvent(
    this->dataPtr->dropText, this->dataPtr->mouseDropPos);
  App()->sendEvent(App()->findChild<MainWindow *>(), &dropOnSceneEvent);
  this->dataPtr->dropDirty = false;
}

/////////////////////////////////////////////////
void GzRenderer::BroadcastHoverPos()
{
  if (!this->dataPtr->hoverDirty)
    return;

  auto pos = rendering::screenToScene(this->dataPtr->mouseHoverPos,
      this->dataPtr->camera, this->dataPtr->rayQuery, 1000);

  events::HoverToScene hoverToSceneEvent(pos);
  App()->sendEvent(App()->findChild<MainWindow *>(), &hoverToSceneEvent);

  common::MouseEvent hoverMouseEvent = this->dataPtr->mouseEvent;
  hoverMouseEvent.SetPos(this->dataPtr->mouseHoverPos);
  hoverMouseEvent.SetDragging(false);
  hoverMouseEvent.SetType(common::MouseEvent::MOVE);
  events::HoverOnScene hoverOnSceneEvent(hoverMouseEvent);
  App()->sendEvent(App()->findChild<MainWindow *>(), &hoverOnSceneEvent);

  this->dataPtr->hoverDirty = false;
}

/////////////////////////////////////////////////
void GzRenderer::BroadcastDrag()
{
  if (!this->dataPtr->mouseDirty)
    return;

  // Only broadcast drag if dragging
  if (!this->dataPtr->mouseEvent.Dragging())
    return;

  events::DragOnScene dragEvent(this->dataPtr->mouseEvent);
  App()->sendEvent(App()->findChild<MainWindow *>(), &dragEvent);
}

/////////////////////////////////////////////////
void GzRenderer::BroadcastLeftClick()
{
  if (!this->dataPtr->mouseDirty)
    return;

  if (this->dataPtr->mouseEvent.Button() != common::MouseEvent::LEFT ||
      this->dataPtr->mouseEvent.Type() != common::MouseEvent::RELEASE)
    return;

  auto pos = rendering::screenToScene(this->dataPtr->mouseEvent.Pos(),
      this->dataPtr->camera, this->dataPtr->rayQuery, 1000);

  events::LeftClickToScene leftClickToSceneEvent(pos);
  App()->sendEvent(App()->findChild<MainWindow *>(), &leftClickToSceneEvent);

  events::LeftClickOnScene leftClickOnSceneEvent(this->dataPtr->mouseEvent);
  App()->sendEvent(App()->findChild<MainWindow *>(), &leftClickOnSceneEvent);
}

/////////////////////////////////////////////////
void GzRenderer::BroadcastRightClick()
{
  if (!this->dataPtr->mouseDirty)
    return;

  if (this->dataPtr->mouseEvent.Button() != common::MouseEvent::RIGHT ||
      this->dataPtr->mouseEvent.Type() != common::MouseEvent::RELEASE)
    return;

  auto pos = rendering::screenToScene(this->dataPtr->mouseEvent.Pos(),
      this->dataPtr->camera, this->dataPtr->rayQuery, 1000);

  events::RightClickToScene rightClickToSceneEvent(pos);
  App()->sendEvent(App()->findChild<MainWindow *>(), &rightClickToSceneEvent);

  events::RightClickOnScene rightClickOnSceneEvent(this->dataPtr->mouseEvent);
  App()->sendEvent(App()->findChild<MainWindow *>(), &rightClickOnSceneEvent);
}

/////////////////////////////////////////////////
void GzRenderer::BroadcastMousePress()
{
  if (!this->dataPtr->mouseDirty)
    return;

  if (this->dataPtr->mouseEvent.Type() != common::MouseEvent::PRESS)
    return;

  events::MousePressOnScene event(this->dataPtr->mouseEvent);
  App()->sendEvent(App()->findChild<MainWindow *>(), &event);
}

/////////////////////////////////////////////////
void GzRenderer::BroadcastScroll()
{
  if (!this->dataPtr->mouseDirty)
    return;

  if (this->dataPtr->mouseEvent.Type() != common::MouseEvent::SCROLL)
    return;

  events::ScrollOnScene scrollOnSceneEvent(this->dataPtr->mouseEvent);
  App()->sendEvent(App()->findChild<MainWindow *>(), &scrollOnSceneEvent);
}

/////////////////////////////////////////////////
void GzRenderer::BroadcastKeyRelease()
{
  if (this->dataPtr->keyEvent.Type() != common::KeyEvent::RELEASE)
    return;

  events::KeyReleaseOnScene keyRelease(this->dataPtr->keyEvent);
  App()->sendEvent(App()->findChild<MainWindow *>(), &keyRelease);

  this->dataPtr->keyEvent.SetType(common::KeyEvent::NO_EVENT);
}

/////////////////////////////////////////////////
void GzRenderer::BroadcastKeyPress()
{
  if (this->dataPtr->keyEvent.Type() != common::KeyEvent::PRESS)
    return;

  events::KeyPressOnScene keyPress(this->dataPtr->keyEvent);
  App()->sendEvent(App()->findChild<MainWindow *>(), &keyPress);

  this->dataPtr->keyEvent.SetType(common::KeyEvent::NO_EVENT);
}

/////////////////////////////////////////////////
std::string GzRenderer::Initialize()
{
  if (this->initialized)
    return std::string();

  // Currently only support one engine at a time
  rendering::RenderEngine *engine{nullptr};
  auto loadedEngines = rendering::loadedEngines();

  // Load engine if there's no engine yet
  if (loadedEngines.empty())
  {
    this->dataPtr->rhiParams["winID"] = std::to_string(
        gz::gui::App()->findChild<gz::gui::MainWindow *>()->
        QuickWindow()->winId());
    engine = rendering::engine(this->engineName, this->dataPtr->rhiParams);
  }
  else
  {
    if (!this->engineName.empty() && loadedEngines.front() != this->engineName)
    {
      gzwarn << "Failed to load engine [" << this->engineName
             << "]. Using engine [" << loadedEngines.front()
             << "], which is already loaded. Currently only one engine is "
             << "supported at a time." << std::endl;
    }
    this->engineName = loadedEngines.front();
    engine = rendering::engine(loadedEngines.front());
  }

  if (!engine)
  {
    return "Engine [" + this->engineName + "] is not supported";
  }

  // Scene
  if (engine->SceneCount() > 0)
  {
    return "Currently only one plugin providing a 3D scene is supported at a "
            "time.";
  }

  gzdbg << "Create scene [" << this->sceneName << "]" << std::endl;
  auto scene = engine->CreateScene(this->sceneName);
  if (nullptr == scene)
  {
    return "Failed to create scene [" + this->sceneName + "] for engine [" +
        this->engineName + "]";
  }
  scene->SetAmbientLight(this->ambientLight);
  scene->SetBackgroundColor(this->backgroundColor);
  scene->SetCameraPassCountPerGpuFlush(6u);

  if (this->skyEnable)
  {
    scene->SetSkyEnabled(true);
  }

  auto root = scene->RootVisual();

  // Camera
  this->dataPtr->camera = scene->CreateCamera();
  this->dataPtr->camera->SetUserData("user-camera", true);
  root->AddChild(this->dataPtr->camera);
  this->dataPtr->camera->SetLocalPose(this->cameraPose);
  this->dataPtr->camera->SetNearClipPlane(this->cameraNearClip);
  this->dataPtr->camera->SetFarClipPlane(this->cameraFarClip);
  this->dataPtr->camera->SetImageWidth(this->textureSize.width());
  this->dataPtr->camera->SetImageHeight(this->textureSize.height());
  this->dataPtr->camera->SetAntiAliasing(8);
  this->dataPtr->camera->SetHFOV(this->cameraHFOV);
  // setting the size and calling PreRender should cause the render texture to
  // be rebuilt
  this->dataPtr->camera->PreRender();

  // Update the render interface (texture)
  this->dataPtr->rhi->Update(this->dataPtr->camera);

  // Ray Query
  this->dataPtr->rayQuery = this->dataPtr->camera->Scene()->CreateRayQuery();

  this->initialized = true;
  return std::string();
}

/////////////////////////////////////////////////
void GzRenderer::SetGraphicsAPI(const rendering::GraphicsAPI &_graphicsAPI)
{
  // Create render interface and reset params
  this->dataPtr->rhiParams.clear();

  if (_graphicsAPI == rendering::GraphicsAPI::OPENGL)
  {
    gzdbg << "Creating gz-rendering interface for OpenGL" << std::endl;
    this->dataPtr->rhiParams["useCurrentGLContext"] = "1";
    this->dataPtr->rhi = std::make_unique<GzCameraTextureRhiOpenGL>();
  }
#ifdef __APPLE__
  else if (_graphicsAPI == rendering::GraphicsAPI::METAL)
  {
    gzdbg << "Creating gz-renderering interface for Metal" << std::endl;
    this->dataPtr->rhiParams["metal"] = "1";
    this->dataPtr->rhi = std::make_unique<GzCameraTextureRhiMetal>();
  }
#endif
}

/////////////////////////////////////////////////
void GzRenderer::Destroy()
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
    gzdbg << "Destroy scene [" << scene->Name() << "]" << std::endl;
    engine->DestroyScene(scene);

    // TODO(anyone) If that was the last scene, terminate engine?
  }
}

/////////////////////////////////////////////////
void GzRenderer::NewHoverEvent(const math::Vector2i &_hoverPos)
{
  std::lock_guard<std::mutex> lock(this->dataPtr->mutex);
  this->dataPtr->mouseHoverPos = _hoverPos;
  this->dataPtr->hoverDirty = true;
}

/////////////////////////////////////////////////
void GzRenderer::NewDropEvent(const std::string &_dropText,
  const math::Vector2i &_dropPos)
{
  std::lock_guard<std::mutex> lock(this->dataPtr->mutex);
  this->dataPtr->dropText = _dropText;
  this->dataPtr->mouseDropPos = _dropPos;
  this->dataPtr->dropDirty = true;
}

/////////////////////////////////////////////////
void GzRenderer::NewMouseEvent(const common::MouseEvent &_e)
{
  std::lock_guard<std::mutex> lock(this->dataPtr->mutex);
  if (this->dataPtr->mouseEvents.size() >= this->dataPtr->kMaxMouseEventSize)
    this->dataPtr->mouseEvents.pop_front();
  this->dataPtr->mouseEvents.push_back(_e);
  this->dataPtr->mouseDirty = true;
}

/////////////////////////////////////////////////
void GzRenderer::TextureId(void* _texturePtr)
{
  this->dataPtr->rhi->TextureId(_texturePtr);
}

/////////////////////////////////////////////////
RenderThread::RenderThread()
{
  // Set default graphics API to OpenGL
  this->SetGraphicsAPI(rendering::GraphicsAPI::OPENGL);

  RenderWindowItem::Implementation::threads << this;
  qRegisterMetaType<RenderSync*>("RenderSync*");
}

/////////////////////////////////////////////////
void RenderThread::SetErrorCb(std::function<void(const QString&)> _cb)
{
  this->errorCb = _cb;
}

/////////////////////////////////////////////////
void RenderThread::RenderNext(RenderSync *_renderSync)
{
  this->rhi->RenderNext(_renderSync);
  emit this->TextureReady(
    this->rhi->TexturePtr(),
    this->rhi->TextureSize());
}

/////////////////////////////////////////////////
void RenderThread::ShutDown()
{
  // The render interface calls Destroy on GzRendering
  this->rhi->ShutDown();

  // Stop event processing, move the thread to GUI and make sure it is deleted.
  this->exit();
  if (this->gzRenderer.initialized)
    this->moveToThread(QGuiApplication::instance()->thread());
}

/////////////////////////////////////////////////
void RenderThread::SizeChanged()
{
  auto item = qobject_cast<QQuickItem *>(this->sender());
  if (!item)
  {
    gzerr << "Internal error, sender is not QQuickItem." << std::endl;
    return;
  }

  if (item->width() <= 0 || item->height() <= 0)
    return;

  this->gzRenderer.textureSize = QSize(item->width(), item->height());
  this->gzRenderer.textureDirty = true;
}

/////////////////////////////////////////////////
QOffscreenSurface *RenderThread::Surface() const
{
  return this->rhi->Surface();
}

/////////////////////////////////////////////////
void RenderThread::SetSurface(QOffscreenSurface *_surface)
{
  this->rhi->SetSurface(_surface);
}

/////////////////////////////////////////////////
QOpenGLContext *RenderThread::Context() const
{
  return this->rhi->Context();
}

/////////////////////////////////////////////////
void RenderThread::SetContext(QOpenGLContext *_context)
{
  this->rhi->SetContext(_context);
}

/////////////////////////////////////////////////
void RenderThread::SetGraphicsAPI(const rendering::GraphicsAPI &_graphicsAPI)
{
  // Set the graphics API for the GzRenderer
  this->gzRenderer.SetGraphicsAPI(_graphicsAPI);

  // Create the render interface
  if (_graphicsAPI == rendering::GraphicsAPI::OPENGL)
  {
    gzdbg << "Creating render thread interface for OpenGL" << std::endl;
    this->rhi = std::make_unique<RenderThreadRhiOpenGL>(&this->gzRenderer);
  }
#ifdef __APPLE__
  else if (_graphicsAPI == rendering::GraphicsAPI::METAL)
  {
    gzdbg << "Creating render thread interface for Metal" << std::endl;
    this->rhi = std::make_unique<RenderThreadRhiMetal>(&this->gzRenderer);
  }
#endif
}

/////////////////////////////////////////////////
std::string RenderThread::Initialize()
{
  auto loadingError = this->rhi->Initialize();
  if (!loadingError.empty())
  {
    this->errorCb(QString::fromStdString(loadingError));
  }
  return loadingError;
}

/////////////////////////////////////////////////
TextureNode::TextureNode(
    QQuickWindow *_window,
    RenderSync &_renderSync,
    const rendering::GraphicsAPI &_graphicsAPI)
    : renderSync(_renderSync) , window(_window)
{
  if (_graphicsAPI == rendering::GraphicsAPI::OPENGL)
  {
    gzdbg << "Creating texture node render interface for OpenGL" << std::endl;
    this->rhi = std::make_unique<TextureNodeRhiOpenGL>(_window);
  }
#ifdef __APPLE__
  else if (_graphicsAPI == rendering::GraphicsAPI::METAL)
  {
    gzdbg << "Creating texture node render interface for Metal" << std::endl;
    this->rhi = std::make_unique<TextureNodeRhiMetal>(_window);
  }
#endif

  this->setTexture(this->rhi->Texture());
}

/////////////////////////////////////////////////
TextureNode::~TextureNode() = default;

/////////////////////////////////////////////////
void TextureNode::NewTexture(void* _texturePtr, const QSize &_size)
{
  this->rhi->NewTexture(_texturePtr, _size);

  // We cannot call QQuickWindow::update directly here, as this is only allowed
  // from the rendering thread or GUI thread.
  emit PendingNewTexture();
}

/////////////////////////////////////////////////
void TextureNode::PrepareNode()
{
  this->rhi->PrepareNode();

  if (this->rhi->HasNewTexture())
  {
    this->setTexture(this->rhi->Texture());

    this->markDirty(DirtyMaterial);

    // This will notify the rendering thread that the texture is now being
    // rendered and it can start rendering to the other one.
    // emit TextureInUse(&this->renderSync); See comment below
  }
  // NOTE: The original code from Qt samples only emitted when
  // newId is not null.
  //
  // This is correct... for their case.
  // However we need to synchronize the threads when resolution changes,
  // and we're also currently doing everything in lockstep (i.e. both Qt
  // and worker thread are serialized,
  // see https://github.com/gazebosim/gz-rendering/issues/304 )
  //
  // We need to emit even if newId == 0 because it's safe as long as both
  // threads are forcefully serialized and otherwise we may get a
  // deadlock (this func. called twice in a row with the worker thread still
  // finishing the 1st iteration, may result in a deadlock for newer versions
  // of Qt; as WaitForWorkerThread will be called with no corresponding
  // WaitForQtThreadAndBlock as the worker thread thinks there are
  // no more jobs to do.
  //
  // If we want these to run in worker thread and stay resolution-synchronized,
  // we probably should use a different method of signals and slots
  // to send work to the worker thread and get results back
  emit TextureInUse(&this->renderSync);

  this->renderSync.WaitForWorkerThread();
}

/////////////////////////////////////////////////
RenderWindowItem::RenderWindowItem(QQuickItem *_parent)
  : QQuickItem(_parent), dataPtr(utils::MakeUniqueImpl<Implementation>())
{
  this->setAcceptedMouseButtons(Qt::AllButtons);
  this->setFlag(ItemHasContents);
  this->dataPtr->renderThread = new RenderThread();
}

/////////////////////////////////////////////////
RenderWindowItem::~RenderWindowItem()
{
  this->StopRendering();
}

/////////////////////////////////////////////////
void RenderWindowItem::StopRendering()
{
  // Disconnect our QT connections.
  for(auto conn : this->dataPtr->connections)
    QObject::disconnect(conn);

  this->dataPtr->renderSync.Shutdown();
  QMetaObject::invokeMethod(this->dataPtr->renderThread,
                            "ShutDown",
                            Qt::QueuedConnection);

  this->dataPtr->renderThread->wait();
}

/////////////////////////////////////////////////
// This slot will run on the main thread
void RenderWindowItem::Ready()
{
  if (this->dataPtr->graphicsAPI == rendering::GraphicsAPI::OPENGL)
  {
    this->dataPtr->renderThread->SetSurface(new QOffscreenSurface());
    this->dataPtr->renderThread->Surface()->setFormat(
        this->dataPtr->renderThread->Context()->format());
    this->dataPtr->renderThread->Surface()->create();
  }

  // Carry out initialization on main thread before moving to render thread
  if (!this->dataPtr->renderThread->Initialize().empty())
  {
    return;
  }

  if (this->dataPtr->graphicsAPI == rendering::GraphicsAPI::OPENGL)
  {
    // Move context to the render thread
    this->dataPtr->renderThread->Context()->moveToThread(
        this->dataPtr->renderThread);
  }

  this->dataPtr->renderThread->moveToThread(this->dataPtr->renderThread);

  this->dataPtr->renderThread->gzRenderer.textureSize =
      QSize(std::max({this->width(), 1.0}), std::max({this->height(), 1.0}));

  this->connect(this, &QQuickItem::widthChanged,
      this->dataPtr->renderThread, &RenderThread::SizeChanged);
  this->connect(this, &QQuickItem::heightChanged,
      this->dataPtr->renderThread, &RenderThread::SizeChanged);

  this->dataPtr->renderThread->start();
  this->dataPtr->initializing = false;
  this->dataPtr->initialized = true;
  this->update();
}

/////////////////////////////////////////////////
QSGNode *RenderWindowItem::updatePaintNode(QSGNode *_node,
    QQuickItem::UpdatePaintNodeData * /*_data*/)
{
  TextureNode *node = static_cast<TextureNode *>(_node);

  if (!this->dataPtr->initialized)
  {
    // Exit immediately if still initializing
    if (this->dataPtr->initializing)
    {
      return nullptr;
    }
    this->dataPtr->initializing = true;

    // Set the render thread's render system
    this->dataPtr->renderThread->SetGraphicsAPI(
        this->dataPtr->graphicsAPI);

    if (this->dataPtr->graphicsAPI == rendering::GraphicsAPI::OPENGL)
    {
      QOpenGLContext *current = this->window()->openglContext();
      // Some GL implementations require that the currently bound context is
      // made non-current before we set up sharing, so we doneCurrent here
      // and makeCurrent down below while setting up our own context.
      current->doneCurrent();

      this->dataPtr->renderThread->SetContext(new QOpenGLContext());
      this->dataPtr->renderThread->Context()->setFormat(current->format());
      this->dataPtr->renderThread->Context()->setShareContext(current);
      this->dataPtr->renderThread->Context()->create();

      // The slot "Ready" runs on the main thread, move the context to match
      this->dataPtr->renderThread->Context()->moveToThread(
          QApplication::instance()->thread());

      current->makeCurrent(this->window());

      // Initialize on main thread
      QMetaObject::invokeMethod(this, "Ready", Qt::QueuedConnection);
    }
    else if (this->dataPtr->graphicsAPI == rendering::GraphicsAPI::METAL)
    {
      // Initialize on main thread
      QMetaObject::invokeMethod(this, "Ready", Qt::QueuedConnection);
    }
    else
    {
      gzerr << "GraphicsAPI ["
             << rendering::GraphicsAPIUtils::Str(this->dataPtr->graphicsAPI)
             << "] is not supported"
             << std::endl;
    }
    return nullptr;
  }

  if (!node)
  {
    node = new TextureNode(this->window(), this->dataPtr->renderSync,
        this->dataPtr->graphicsAPI);

    // Set up connections to get the production of render texture in sync with
    // vsync on the rendering thread.
    //
    // When a new texture is ready on the rendering thread, we use a direct
    // connection to the texture node to let it know a new texture can be used.
    // The node will then emit PendingNewTexture which we bind to
    // QQuickWindow::update to schedule a redraw.
    //
    // When the scene graph starts rendering the next frame, the PrepareNode()
    // function is used to update the node with the new texture. Once it
    // completes, it emits TextureInUse() which we connect to the rendering
    // thread's RenderNext() to have it start producing content into its render
    // texture.
    //
    // This rendering pipeline is throttled by vsync on the scene graph
    // rendering thread.

    this->dataPtr->connections << this->connect(this->dataPtr->renderThread,
        &RenderThread::TextureReady, node, &TextureNode::NewTexture,
        Qt::DirectConnection);
    this->dataPtr->connections << this->connect(node,
        &TextureNode::PendingNewTexture, this->window(),
        &QQuickWindow::update, Qt::QueuedConnection);
    this->dataPtr->connections << this->connect(this->window(),
        &QQuickWindow::beforeRendering, node, &TextureNode::PrepareNode,
        Qt::DirectConnection);
    this->dataPtr->connections << this->connect(node,
        &TextureNode::TextureInUse, this->dataPtr->renderThread,
        &RenderThread::RenderNext, Qt::QueuedConnection);

    // Get the production of FBO textures started..
    QMetaObject::invokeMethod(this->dataPtr->renderThread, "RenderNext",
      Qt::QueuedConnection,
      Q_ARG(RenderSync*, &node->renderSync));
  }

  node->setRect(this->boundingRect());

  return node;
}

/////////////////////////////////////////////////
void RenderWindowItem::SetBackgroundColor(const math::Color &_color)
{
  this->dataPtr->renderThread->gzRenderer.backgroundColor = _color;
}

/////////////////////////////////////////////////
void RenderWindowItem::SetAmbientLight(const math::Color &_ambient)
{
  this->dataPtr->renderThread->gzRenderer.ambientLight = _ambient;
}

/////////////////////////////////////////////////
void RenderWindowItem::SetEngineName(const std::string &_name)
{
  // Deprecated: accept ignition-prefixed engines
  auto name = _name;
  auto pos = name.find("ignition");
  if (pos != std::string::npos)
  {
    name.replace(pos, pos + 8, "gz");
    gzwarn << "Trying to load deprecated plugin [" << _name << "]. Use ["
           << name << "] instead." << std::endl;
  }
  this->dataPtr->renderThread->gzRenderer.engineName = name;
}

/////////////////////////////////////////////////
void RenderWindowItem::SetSceneName(const std::string &_name)
{
  this->dataPtr->renderThread->gzRenderer.sceneName = _name;
}

/////////////////////////////////////////////////
void RenderWindowItem::SetCameraPose(const math::Pose3d &_pose)
{
  this->dataPtr->renderThread->gzRenderer.cameraPose = _pose;
}

/////////////////////////////////////////////////
void RenderWindowItem::SetCameraNearClip(double _near)
{
  this->dataPtr->renderThread->gzRenderer.cameraNearClip = _near;
}

/////////////////////////////////////////////////
void RenderWindowItem::SetCameraFarClip(double _far)
{
  this->dataPtr->renderThread->gzRenderer.cameraFarClip = _far;
}

/////////////////////////////////////////////////
void RenderWindowItem::SetSkyEnabled(const bool &_sky)
{
  this->dataPtr->renderThread->gzRenderer.skyEnable = _sky;
}

/////////////////////////////////////////////////
void RenderWindowItem::SetGraphicsAPI(
    const rendering::GraphicsAPI &_graphicsAPI)
{
  this->dataPtr->graphicsAPI = _graphicsAPI;
  this->dataPtr->renderThread->SetGraphicsAPI(_graphicsAPI);
}

/////////////////////////////////////////////////
void RenderWindowItem::SetCameraHFOV(const math::Angle &_fov)
{
  this->dataPtr->renderThread->gzRenderer.cameraHFOV = _fov;
}

/////////////////////////////////////////////////
void RenderWindowItem::SetCameraViewController(
  const std::string &_view_controller)
{
  this->dataPtr->renderThread->gzRenderer.cameraViewController =
    _view_controller;
}

/////////////////////////////////////////////////
MinimalScene::MinimalScene()
  : Plugin(), dataPtr(utils::MakeUniqueImpl<Implementation>())
{
  qmlRegisterType<RenderWindowItem>("RenderWindow", 1, 0, "RenderWindow");
}

/////////////////////////////////////////////////
void MinimalScene::LoadConfig(const tinyxml2::XMLElement *_pluginElem)
{
  RenderWindowItem *renderWindow =
      this->PluginItem()->findChild<RenderWindowItem *>();
  if (!renderWindow)
  {
    gzerr << "Unable to find Render Window item. "
           << "Render window will not be created" << std::endl;
    return;
  }
  renderWindow->SetErrorCb(std::bind(&MinimalScene::SetLoadingError, this,
      std::placeholders::_1));

  if (this->title.empty())
    this->title = "3D Scene";

  std::string cmdRenderEngine = gui::renderEngineName();
  // Custom parameters
  if (_pluginElem)
  {
    // Only pick engine from XML if none is set on the Window
    if (cmdRenderEngine.empty())
    {
      auto elem = _pluginElem->FirstChildElement("engine");
      if (nullptr != elem && nullptr != elem->GetText())
      {
        cmdRenderEngine = elem->GetText();
      }
    }

    auto elem = _pluginElem->FirstChildElement("scene");
    if (nullptr != elem && nullptr != elem->GetText())
      renderWindow->SetSceneName(elem->GetText());

    elem = _pluginElem->FirstChildElement("ambient_light");
    if (nullptr != elem && nullptr != elem->GetText())
    {
      math::Color ambient;
      std::stringstream colorStr;
      colorStr << std::string(elem->GetText());
      colorStr >> ambient;
      renderWindow->SetAmbientLight(ambient);
    }

    elem = _pluginElem->FirstChildElement("background_color");
    if (nullptr != elem && nullptr != elem->GetText())
    {
      math::Color bgColor;
      std::stringstream colorStr;
      colorStr << std::string(elem->GetText());
      colorStr >> bgColor;
      renderWindow->SetBackgroundColor(bgColor);
    }

    elem = _pluginElem->FirstChildElement("camera_pose");
    if (nullptr != elem && nullptr != elem->GetText())
    {
      math::Pose3d pose;
      std::stringstream poseStr;
      poseStr << std::string(elem->GetText());
      poseStr >> pose;
      renderWindow->SetCameraPose(pose);
    }

    elem = _pluginElem->FirstChildElement("camera_clip");
    if (nullptr != elem && !elem->NoChildren())
    {
      auto child = elem->FirstChildElement("near");
      if (nullptr != child && nullptr != child->GetText())
      {
        double n;
        std::stringstream nearStr;
        nearStr << std::string(child->GetText());
        nearStr >> n;
        if (nearStr.fail())
        {
          gzerr << "Unable to set <near> to '" << nearStr.str()
                 << "' using default near clip distance" << std::endl;
        }
        else
        {
          renderWindow->SetCameraNearClip(n);
        }
      }

      child = elem->FirstChildElement("far");
      if (nullptr != child && nullptr != child->GetText())
      {
        double f;
        std::stringstream farStr;
        farStr << std::string(child->GetText());
        farStr >> f;
        if (farStr.fail())
        {
          gzerr << "Unable to set <far> to '" << farStr.str()
                 << "' using default far clip distance" << std::endl;
        }
        else
        {
          renderWindow->SetCameraFarClip(f);
        }
      }
    }

    elem = _pluginElem->FirstChildElement("sky");
    if (nullptr != elem && nullptr != elem->GetText())
    {
      renderWindow->SetSkyEnabled(true);
      if (!elem->NoChildren())
        gzwarn << "Child elements of <sky> are not supported yet"
                << std::endl;
    }

    elem = _pluginElem->FirstChildElement("graphics_api");
    if (nullptr != elem && nullptr != elem->GetText())
    {
      rendering::GraphicsAPI graphicsAPI =
          rendering::GraphicsAPIUtils::Set(elem->GetText());
      renderWindow->SetGraphicsAPI(graphicsAPI);
    }

    elem = _pluginElem->FirstChildElement("horizontal_fov");
    if (nullptr != elem && nullptr != elem->GetText())
    {
      double fovDeg;
      math::Angle fov;
      std::stringstream fovStr;
      fovStr << std::string(elem->GetText());
      fovStr >> fovDeg;
      if (fovStr.fail())
      {
        gzerr << "Unable to set <horizontal_fov> to '" << fovStr.str()
              << "' using default horizontal field of view" << std::endl;
      }
      else
      {
        fov.SetDegree(fovDeg);
        renderWindow->SetCameraHFOV(fov);
      }
    }

    elem = _pluginElem->FirstChildElement("view_controller");
    if (nullptr != elem && nullptr != elem->GetText())
    {
      renderWindow->SetCameraViewController(elem->GetText());
    }
  }

  renderWindow->SetEngineName(cmdRenderEngine);
  // there is a problem with displaying ogre2 render textures that are in
  // sRGB format. Workaround for now is to apply gamma correction
  // manually.
  // There maybe a better way to solve the problem by making OpenGL calls.
  if (cmdRenderEngine == std::string("ogre2"))
    this->PluginItem()->setProperty("gammaCorrect", true);
}

/////////////////////////////////////////////////
void RenderWindowItem::OnHovered(const gz::math::Vector2i &_hoverPos)
{
  this->dataPtr->renderThread->gzRenderer.NewHoverEvent(_hoverPos);
}

/////////////////////////////////////////////////
void RenderWindowItem::OnDropped(const QString &_drop,
    const gz::math::Vector2i &_dropPos)
{
  this->dataPtr->renderThread->gzRenderer.NewDropEvent(
    _drop.toStdString(), _dropPos);
}

/////////////////////////////////////////////////
void RenderWindowItem::SetErrorCb(std::function<void(const QString&)> _cb)
{
  this->dataPtr->renderThread->SetErrorCb(_cb);
}

/////////////////////////////////////////////////
void RenderWindowItem::mousePressEvent(QMouseEvent *_e)
{
  this->dataPtr->mouseEvent = convert(*_e);
  this->dataPtr->mouseEvent.SetPressPos(this->dataPtr->mouseEvent.Pos());

  this->dataPtr->renderThread->gzRenderer.NewMouseEvent(
      this->dataPtr->mouseEvent);
}

////////////////////////////////////////////////
void RenderWindowItem::keyPressEvent(QKeyEvent *_e)
{
  if (_e->isAutoRepeat())
    return;

  auto event = convert(*_e);
  this->HandleKeyPress(event);
}

////////////////////////////////////////////////
void RenderWindowItem::keyReleaseEvent(QKeyEvent *_e)
{
  if (_e->isAutoRepeat())
    return;

  auto event = convert(*_e);
  this->HandleKeyPress(event);
}

////////////////////////////////////////////////
void RenderWindowItem::mouseReleaseEvent(QMouseEvent *_e)
{
  // Store values that depend on previous events
  auto pressPos = this->dataPtr->mouseEvent.PressPos();
  auto dragging = this->dataPtr->mouseEvent.Dragging();

  this->dataPtr->mouseEvent = convert(*_e);
  this->dataPtr->mouseEvent.SetPressPos(pressPos);
  this->dataPtr->mouseEvent.SetDragging(dragging);

  this->dataPtr->renderThread->gzRenderer.NewMouseEvent(
      this->dataPtr->mouseEvent);
}

////////////////////////////////////////////////
void RenderWindowItem::mouseMoveEvent(QMouseEvent *_e)
{
  // Store values that depend on previous events
  auto pressPos = this->dataPtr->mouseEvent.PressPos();

  this->dataPtr->mouseEvent = convert(*_e);

  if (this->dataPtr->mouseEvent.Dragging())
    this->dataPtr->mouseEvent.SetPressPos(pressPos);

  this->dataPtr->renderThread->gzRenderer.NewMouseEvent(
      this->dataPtr->mouseEvent);
}

////////////////////////////////////////////////
void RenderWindowItem::wheelEvent(QWheelEvent *_e)
{
  this->forceActiveFocus();

  this->dataPtr->mouseEvent = convert(*_e);
  this->dataPtr->renderThread->gzRenderer.NewMouseEvent(
    this->dataPtr->mouseEvent);
}

////////////////////////////////////////////////
void RenderWindowItem::HandleKeyPress(const common::KeyEvent &_e)
{
  this->dataPtr->renderThread->gzRenderer.HandleKeyPress(_e);
}

////////////////////////////////////////////////
void RenderWindowItem::HandleKeyRelease(const common::KeyEvent &_e)
{
  this->dataPtr->renderThread->gzRenderer.HandleKeyRelease(_e);
}

/////////////////////////////////////////////////
void MinimalScene::OnHovered(int _mouseX, int _mouseY)
{
  auto renderWindow = this->PluginItem()->findChild<RenderWindowItem *>();
  renderWindow->OnHovered({_mouseX, _mouseY});
}

/////////////////////////////////////////////////
void MinimalScene::OnDropped(const QString &_drop, int _mouseX, int _mouseY)
{
  auto renderWindow = this->PluginItem()->findChild<RenderWindowItem *>();
  renderWindow->OnDropped(_drop, {_mouseX, _mouseY});
}

/////////////////////////////////////////////////
void MinimalScene::OnFocusWindow()
{
  auto renderWindow = this->PluginItem()->findChild<RenderWindowItem *>();
  renderWindow->forceActiveFocus();
}

/////////////////////////////////////////////////
QString MinimalScene::LoadingError() const
{
  return this->loadingError;
}

/////////////////////////////////////////////////
void MinimalScene::SetLoadingError(const QString &_loadingError)
{
  if (!_loadingError.isEmpty())
  {
    auto renderWindow = this->PluginItem()->findChild<RenderWindowItem *>();
    if (nullptr != renderWindow)
      renderWindow->StopRendering();
  }
  this->loadingError = _loadingError;
  this->LoadingErrorChanged();
}

// Register this plugin
GZ_ADD_PLUGIN(gz::gui::plugins::MinimalScene,
              gz::gui::Plugin)
