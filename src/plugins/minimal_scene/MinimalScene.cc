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

#include "MinimalScene.hh"

#include <algorithm>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include <ignition/common/Console.hh>
#include <ignition/common/KeyEvent.hh>
#include <ignition/common/MouseEvent.hh>
#include <ignition/math/Vector2.hh>
#include <ignition/math/Vector3.hh>
#include <ignition/plugin/Register.hh>

// TODO(louise) Remove these pragmas once ign-rendering
// is disabling the warnings
#ifdef _MSC_VER
#pragma warning(push, 0)
#endif

#include <ignition/rendering/Camera.hh>
#include <ignition/rendering/RayQuery.hh>
#include <ignition/rendering/RenderEngine.hh>
#include <ignition/rendering/RenderingIface.hh>
#include <ignition/rendering/Scene.hh>
#include <ignition/rendering/Utils.hh>

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include "ignition/gui/Application.hh"
#include "ignition/gui/Conversions.hh"
#include "ignition/gui/GuiEvents.hh"
#include "ignition/gui/Helpers.hh"
#include "ignition/gui/MainWindow.hh"

Q_DECLARE_METATYPE(ignition::gui::plugins::RenderSync*)

/// \brief Private data class for IgnRenderer
class ignition::gui::plugins::IgnRenderer::Implementation
{
  /// \brief Flag to indicate if mouse event is dirty
  public: bool mouseDirty{false};

  /// \brief Flag to indicate if hover event is dirty
  public: bool hoverDirty{false};

  /// \brief Flag to indicate if drop event is dirty
  public: bool dropDirty{false};

  /// \brief Mouse event
  public: common::MouseEvent mouseEvent;

  /// \brief Key event
  public: common::KeyEvent keyEvent;

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
/// https://github.com/ignitionrobotics/ign-rendering/issues/304
class ignition::gui::plugins::RenderSync
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
class ignition::gui::plugins::RenderWindowItem::Implementation
{
  /// \brief Keep latest mouse event
  public: common::MouseEvent mouseEvent;

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
class ignition::gui::plugins::MinimalScene::Implementation
{
};

using namespace ignition;
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
IgnRenderer::IgnRenderer()
  : dataPtr(utils::MakeUniqueImpl<Implementation>())
{
}

/////////////////////////////////////////////////
void IgnRenderer::Render(RenderSync *_renderSync)
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
    this->dataPtr->camera->SetAspectRatio(this->textureSize.width() /
        this->textureSize.height());
    // setting the size should cause the render texture to be rebuilt
    this->dataPtr->camera->PreRender();
    this->textureDirty = false;

    // TODO(anyone) See SwapFromThread comments
    // _renderSync->ReleaseQtThreadFromBlock(lock);
  }

  this->textureId = this->dataPtr->camera->RenderTextureGLId();

  // view control
  this->HandleMouseEvent();

  if (ignition::gui::App())
  {
    ignition::gui::App()->sendEvent(
        ignition::gui::App()->findChild<ignition::gui::MainWindow *>(),
        new gui::events::PreRender());
  }

  // update and render to texture
  this->dataPtr->camera->Update();

  if (ignition::gui::App())
  {
    ignition::gui::App()->sendEvent(
        ignition::gui::App()->findChild<ignition::gui::MainWindow *>(),
        new gui::events::Render());
  }
  _renderSync->ReleaseQtThreadFromBlock(lock);
}

/////////////////////////////////////////////////
void IgnRenderer::HandleMouseEvent()
{
  std::lock_guard<std::mutex> lock(this->dataPtr->mutex);
  this->BroadcastHoverPos();
  this->BroadcastDrag();
  this->BroadcastMousePress();
  this->BroadcastLeftClick();
  this->BroadcastRightClick();
  this->BroadcastScroll();
  this->BroadcastKeyPress();
  this->BroadcastKeyRelease();
  this->BroadcastDrop();
  this->dataPtr->mouseDirty = false;
}

////////////////////////////////////////////////
void IgnRenderer::HandleKeyPress(const common::KeyEvent &_e)
{
  std::lock_guard<std::mutex> lock(this->dataPtr->mutex);

  this->dataPtr->keyEvent = _e;

  this->dataPtr->mouseEvent.SetControl(this->dataPtr->keyEvent.Control());
  this->dataPtr->mouseEvent.SetShift(this->dataPtr->keyEvent.Shift());
  this->dataPtr->mouseEvent.SetAlt(this->dataPtr->keyEvent.Alt());
}

////////////////////////////////////////////////
void IgnRenderer::HandleKeyRelease(const common::KeyEvent &_e)
{
  std::lock_guard<std::mutex> lock(this->dataPtr->mutex);

  this->dataPtr->keyEvent = _e;

  this->dataPtr->mouseEvent.SetControl(this->dataPtr->keyEvent.Control());
  this->dataPtr->mouseEvent.SetShift(this->dataPtr->keyEvent.Shift());
  this->dataPtr->mouseEvent.SetAlt(this->dataPtr->keyEvent.Alt());
}

/////////////////////////////////////////////////
void IgnRenderer::BroadcastDrop()
{
  if (!this->dataPtr->dropDirty)
    return;
  events::DropOnScene dropOnSceneEvent(
    this->dataPtr->dropText, this->dataPtr->mouseDropPos);
  App()->sendEvent(App()->findChild<MainWindow *>(), &dropOnSceneEvent);
  this->dataPtr->dropDirty = false;
}

/////////////////////////////////////////////////
void IgnRenderer::BroadcastHoverPos()
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
void IgnRenderer::BroadcastDrag()
{
  if (!this->dataPtr->mouseDirty)
    return;

  // Only broadcast drag if dragging
  if (!this->dataPtr->mouseEvent.Dragging())
    return;

  events::DragOnScene dragEvent(this->dataPtr->mouseEvent);
  App()->sendEvent(App()->findChild<MainWindow *>(), &dragEvent);

  this->dataPtr->mouseDirty = false;
}

/////////////////////////////////////////////////
void IgnRenderer::BroadcastLeftClick()
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

  this->dataPtr->mouseDirty = false;
}

/////////////////////////////////////////////////
void IgnRenderer::BroadcastRightClick()
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

  this->dataPtr->mouseDirty = false;
}

/////////////////////////////////////////////////
void IgnRenderer::BroadcastMousePress()
{
  if (!this->dataPtr->mouseDirty)
    return;

  if (this->dataPtr->mouseEvent.Type() != common::MouseEvent::PRESS)
    return;

  events::MousePressOnScene event(this->dataPtr->mouseEvent);
  App()->sendEvent(App()->findChild<MainWindow *>(), &event);

  this->dataPtr->mouseDirty = false;
}

/////////////////////////////////////////////////
void IgnRenderer::BroadcastScroll()
{
  if (!this->dataPtr->mouseDirty)
    return;

  if (this->dataPtr->mouseEvent.Type() != common::MouseEvent::SCROLL)
    return;

  events::ScrollOnScene scrollOnSceneEvent(this->dataPtr->mouseEvent);
  App()->sendEvent(App()->findChild<MainWindow *>(), &scrollOnSceneEvent);

  this->dataPtr->mouseDirty = false;
}

/////////////////////////////////////////////////
void IgnRenderer::BroadcastKeyRelease()
{
  if (this->dataPtr->keyEvent.Type() != common::KeyEvent::RELEASE)
    return;

  events::KeyReleaseOnScene keyRelease(this->dataPtr->keyEvent);
  App()->sendEvent(App()->findChild<MainWindow *>(), &keyRelease);

  this->dataPtr->keyEvent.SetType(common::KeyEvent::NO_EVENT);
}

/////////////////////////////////////////////////
void IgnRenderer::BroadcastKeyPress()
{
  if (this->dataPtr->keyEvent.Type() != common::KeyEvent::PRESS)
    return;

  events::KeyPressOnScene keyPress(this->dataPtr->keyEvent);
  App()->sendEvent(App()->findChild<MainWindow *>(), &keyPress);

  this->dataPtr->keyEvent.SetType(common::KeyEvent::NO_EVENT);
}

/////////////////////////////////////////////////
std::string IgnRenderer::Initialize()
{
  if (this->initialized)
    return std::string();

  // Currently only support one engine at a time
  rendering::RenderEngine *engine{nullptr};
  auto loadedEngines = rendering::loadedEngines();

  // Load engine if there's no engine yet
  if (loadedEngines.empty())
  {
    std::map<std::string, std::string> params;
    params["useCurrentGLContext"] = "1";
    params["winID"] = std::to_string(
        ignition::gui::App()->findChild<ignition::gui::MainWindow *>()->
        QuickWindow()->winId());
    engine = rendering::engine(this->engineName, params);
  }
  else
  {
    if (!this->engineName.empty() && loadedEngines.front() != this->engineName)
    {
      ignwarn << "Failed to load engine [" << this->engineName
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

  igndbg << "Create scene [" << this->sceneName << "]" << std::endl;
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
  this->textureId = this->dataPtr->camera->RenderTextureGLId();

  // Ray Query
  this->dataPtr->rayQuery = this->dataPtr->camera->Scene()->CreateRayQuery();

  this->initialized = true;
  return std::string();
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
void IgnRenderer::NewHoverEvent(const math::Vector2i &_hoverPos)
{
  std::lock_guard<std::mutex> lock(this->dataPtr->mutex);
  this->dataPtr->mouseHoverPos = _hoverPos;
  this->dataPtr->hoverDirty = true;
}

/////////////////////////////////////////////////
void IgnRenderer::NewDropEvent(const std::string &_dropText,
  const math::Vector2i &_dropPos)
{
  std::lock_guard<std::mutex> lock(this->dataPtr->mutex);
  this->dataPtr->dropText = _dropText;
  this->dataPtr->mouseDropPos = _dropPos;
  this->dataPtr->dropDirty = true;
}

/////////////////////////////////////////////////
void IgnRenderer::NewMouseEvent(const common::MouseEvent &_e)
{
  std::lock_guard<std::mutex> lock(this->dataPtr->mutex);
  this->dataPtr->mouseEvent = _e;
  this->dataPtr->mouseDirty = true;
}

/////////////////////////////////////////////////
RenderThread::RenderThread()
{
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
  this->context->makeCurrent(this->surface);

  if (!this->ignRenderer.initialized)
  {
    // Initialize renderer
    auto loadingError = this->ignRenderer.Initialize();
    if (!loadingError.empty())
    {
      this->errorCb(QString::fromStdString(loadingError));
      return;
    }
  }

  // check if engine has been successfully initialized
  if (!this->ignRenderer.initialized)
  {
    ignerr << "Unable to initialize renderer" << std::endl;
    return;
  }

  this->ignRenderer.Render(_renderSync);

  emit TextureReady(this->ignRenderer.textureId, this->ignRenderer.textureSize);
}

/////////////////////////////////////////////////
void RenderThread::ShutDown()
{
  if (this->context && this->surface)
    this->context->makeCurrent(this->surface);

  this->ignRenderer.Destroy();

  if (this->context)
  {
    this->context->doneCurrent();
    delete this->context;
  }

  // schedule this to be deleted only after we're done cleaning up
  if (this->surface)
    this->surface->deleteLater();

  // Stop event processing, move the thread to GUI and make sure it is deleted.
  this->exit();
  if (this->ignRenderer.initialized)
    this->moveToThread(QGuiApplication::instance()->thread());
}

/////////////////////////////////////////////////
void RenderThread::SizeChanged()
{
  auto item = qobject_cast<QQuickItem *>(this->sender());
  if (!item)
  {
    ignerr << "Internal error, sender is not QQuickItem." << std::endl;
    return;
  }

  if (item->width() <= 0 || item->height() <= 0)
    return;

  this->ignRenderer.textureSize = QSize(item->width(), item->height());
  this->ignRenderer.textureDirty = true;
}

/////////////////////////////////////////////////
TextureNode::TextureNode(QQuickWindow *_window, RenderSync &_renderSync)
    : renderSync(_renderSync), window(_window)
{
  // Our texture node must have a texture, so use the default 0 texture.
#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
  this->texture = this->window->createTextureFromId(0, QSize(1, 1));
#else
  void * nativeLayout;
  this->texture = this->window->createTextureFromNativeObject(
      QQuickWindow::NativeObjectTexture, &nativeLayout, 0, QSize(1, 1),
      QQuickWindow::TextureIsOpaque);
#endif
  this->setTexture(this->texture);
}

/////////////////////////////////////////////////
TextureNode::~TextureNode()
{
  delete this->texture;
}

/////////////////////////////////////////////////
void TextureNode::NewTexture(uint _id, const QSize &_size)
{
  this->mutex.lock();
  this->id = _id;
  this->size = _size;
  this->mutex.unlock();

  // We cannot call QQuickWindow::update directly here, as this is only allowed
  // from the rendering thread or GUI thread.
  emit PendingNewTexture();
}

/////////////////////////////////////////////////
void TextureNode::PrepareNode()
{
  this->mutex.lock();
  uint newId = this->id;
  QSize sz = this->size;
  this->id = 0;
  this->mutex.unlock();
  if (newId)
  {
    delete this->texture;
    // note: include QQuickWindow::TextureHasAlphaChannel if the rendered
    // content has alpha.
#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
    this->texture = this->window->createTextureFromId(
        newId, sz, QQuickWindow::TextureIsOpaque);
#else
    // TODO(anyone) Use createTextureFromNativeObject
    // https://github.com/ignitionrobotics/ign-gui/issues/113
#ifndef _WIN32
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
    this->texture = this->window->createTextureFromId(
        newId, sz, QQuickWindow::TextureIsOpaque);
#ifndef _WIN32
# pragma GCC diagnostic pop
#endif

#endif
    this->setTexture(this->texture);

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
  // see https://github.com/ignitionrobotics/ign-rendering/issues/304 )
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
void RenderWindowItem::Ready()
{
  this->dataPtr->renderThread->surface = new QOffscreenSurface();
  this->dataPtr->renderThread->surface->setFormat(
      this->dataPtr->renderThread->context->format());
  this->dataPtr->renderThread->surface->create();

  this->dataPtr->renderThread->ignRenderer.textureSize =
      QSize(std::max({this->width(), 1.0}), std::max({this->height(), 1.0}));

  this->dataPtr->renderThread->moveToThread(this->dataPtr->renderThread);

  this->connect(this, &QQuickItem::widthChanged,
      this->dataPtr->renderThread, &RenderThread::SizeChanged);
  this->connect(this, &QQuickItem::heightChanged,
      this->dataPtr->renderThread, &RenderThread::SizeChanged);

  this->dataPtr->renderThread->start();
  this->update();
}

/////////////////////////////////////////////////
QSGNode *RenderWindowItem::updatePaintNode(QSGNode *_node,
    QQuickItem::UpdatePaintNodeData * /*_data*/)
{
  TextureNode *node = static_cast<TextureNode *>(_node);

  if (!this->dataPtr->renderThread->context)
  {
    QOpenGLContext *current = this->window()->openglContext();
    // Some GL implementations require that the currently bound context is
    // made non-current before we set up sharing, so we doneCurrent here
    // and makeCurrent down below while setting up our own context.
    current->doneCurrent();

    this->dataPtr->renderThread->context = new QOpenGLContext();
    this->dataPtr->renderThread->context->setFormat(current->format());
    this->dataPtr->renderThread->context->setShareContext(current);
    this->dataPtr->renderThread->context->create();
    this->dataPtr->renderThread->context->moveToThread(
        this->dataPtr->renderThread);

    current->makeCurrent(this->window());

    QMetaObject::invokeMethod(this, "Ready");
    return nullptr;
  }

  if (!node)
  {
    node = new TextureNode(this->window(), this->dataPtr->renderSync);

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
  this->dataPtr->renderThread->ignRenderer.backgroundColor = _color;
}

/////////////////////////////////////////////////
void RenderWindowItem::SetAmbientLight(const math::Color &_ambient)
{
  this->dataPtr->renderThread->ignRenderer.ambientLight = _ambient;
}

/////////////////////////////////////////////////
void RenderWindowItem::SetEngineName(const std::string &_name)
{
  this->dataPtr->renderThread->ignRenderer.engineName = _name;
}

/////////////////////////////////////////////////
void RenderWindowItem::SetSceneName(const std::string &_name)
{
  this->dataPtr->renderThread->ignRenderer.sceneName = _name;
}

/////////////////////////////////////////////////
void RenderWindowItem::SetCameraPose(const math::Pose3d &_pose)
{
  this->dataPtr->renderThread->ignRenderer.cameraPose = _pose;
}

/////////////////////////////////////////////////
void RenderWindowItem::SetCameraNearClip(double _near)
{
  this->dataPtr->renderThread->ignRenderer.cameraNearClip = _near;
}

/////////////////////////////////////////////////
void RenderWindowItem::SetCameraFarClip(double _far)
{
  this->dataPtr->renderThread->ignRenderer.cameraFarClip = _far;
}

/////////////////////////////////////////////////
void RenderWindowItem::SetSkyEnabled(const bool &_sky)
{
  this->dataPtr->renderThread->ignRenderer.skyEnable = _sky;
}

/////////////////////////////////////////////////
void RenderWindowItem::SetCameraHFOV(const math::Angle &_fov)
{
  this->dataPtr->renderThread->ignRenderer.cameraHFOV = _fov;
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
    ignerr << "Unable to find Render Window item. "
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
          ignerr << "Unable to set <near> to '" << nearStr.str()
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
          ignerr << "Unable to set <far> to '" << farStr.str()
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
        ignwarn << "Child elements of <sky> are not supported yet" << std::endl;
    }
  
    elem = _pluginElem->FirstChildElement("fov");
    if (nullptr != elem && nullptr != elem->GetText())
    {
	double fovDeg;
	math::Angle fov;
        std::stringstream fovStr;
        fovStr << std::string(elem->GetText());
        fovStr >> fovDeg;
	fov.SetDegree(fovDeg);
	renderWindow->SetCameraHFOV(fov);	
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
void RenderWindowItem::OnHovered(const ignition::math::Vector2i &_hoverPos)
{
  this->dataPtr->renderThread->ignRenderer.NewHoverEvent(_hoverPos);
}

/////////////////////////////////////////////////
void RenderWindowItem::OnDropped(const QString &_drop,
    const ignition::math::Vector2i &_dropPos)
{
  this->dataPtr->renderThread->ignRenderer.NewDropEvent(
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

  this->dataPtr->renderThread->ignRenderer.NewMouseEvent(
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

  this->dataPtr->renderThread->ignRenderer.NewMouseEvent(
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

  this->dataPtr->renderThread->ignRenderer.NewMouseEvent(
      this->dataPtr->mouseEvent);
}

////////////////////////////////////////////////
void RenderWindowItem::wheelEvent(QWheelEvent *_e)
{
  this->forceActiveFocus();

  this->dataPtr->mouseEvent = convert(*_e);
  this->dataPtr->renderThread->ignRenderer.NewMouseEvent(
    this->dataPtr->mouseEvent);
}

////////////////////////////////////////////////
void RenderWindowItem::HandleKeyPress(const common::KeyEvent &_e)
{
  this->dataPtr->renderThread->ignRenderer.HandleKeyPress(_e);
}

////////////////////////////////////////////////
void RenderWindowItem::HandleKeyRelease(const common::KeyEvent &_e)
{
  this->dataPtr->renderThread->ignRenderer.HandleKeyRelease(_e);
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
IGNITION_ADD_PLUGIN(ignition::gui::plugins::MinimalScene,
                    ignition::gui::Plugin)
