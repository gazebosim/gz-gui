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

  //// \brief List of threads
  public: static QList<QThread *> threads;
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
  this->BroadcastLeftClick();
  this->BroadcastRightClick();
  this->BroadcastKeyPress();
  this->BroadcastKeyRelease();
  this->BroadcastDrop();
  this->HandleMouseViewControl();
  this->dataPtr->mouseDirty = false;
}

/////////////////////////////////////////////////
void IgnRenderer::HandleMouseViewControl()
{
  if (!this->dataPtr->mouseDirty)
    return;

  auto pos = this->ScreenToScene(this->dataPtr->mouseEvent.Pos());

  events::LeftClickOnScene leftClickOnSceneEvent(this->dataPtr->mouseEvent);
  events::LeftClickToScene leftClickToSceneEvent(pos);
  App()->sendEvent(App()->findChild<MainWindow *>(), &leftClickOnSceneEvent);
  App()->sendEvent(App()->findChild<MainWindow *>(), &leftClickToSceneEvent);
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

  auto pos = this->ScreenToScene(this->dataPtr->mouseHoverPos);

  events::HoverToScene hoverToSceneEvent(pos);
  App()->sendEvent(App()->findChild<MainWindow *>(), &hoverToSceneEvent);

  common::MouseEvent hoverMouseEvent = this->dataPtr->mouseEvent;
  hoverMouseEvent.SetPos(this->dataPtr->mouseHoverPos);
  hoverMouseEvent.SetDragging(false);
  hoverMouseEvent.SetType(common::MouseEvent::MOVE);
  events::HoverOnScene hoverOnSceneEvent(hoverMouseEvent);
  App()->sendEvent(App()->findChild<MainWindow *>(), &hoverOnSceneEvent);
}

/////////////////////////////////////////////////
void IgnRenderer::BroadcastLeftClick()
{
  if (!this->dataPtr->mouseDirty)
    return;

  if (this->dataPtr->mouseEvent.Dragging())
    return;

  if (this->dataPtr->mouseEvent.Button() != common::MouseEvent::LEFT ||
      this->dataPtr->mouseEvent.Type() != common::MouseEvent::RELEASE)
    return;

  auto pos = this->ScreenToScene(this->dataPtr->mouseEvent.Pos());

  events::LeftClickToScene leftClickToSceneEvent(pos);
  App()->sendEvent(App()->findChild<MainWindow *>(), &leftClickToSceneEvent);
}

/////////////////////////////////////////////////
void IgnRenderer::BroadcastRightClick()
{
  if (!this->dataPtr->mouseDirty)
    return;

  if (this->dataPtr->mouseEvent.Dragging())
    return;

  if (this->dataPtr->mouseEvent.Button() != common::MouseEvent::RIGHT ||
      this->dataPtr->mouseEvent.Type() != common::MouseEvent::RELEASE)
    return;

  auto pos = this->ScreenToScene(this->dataPtr->mouseEvent.Pos());

  events::RightClickToScene rightClickToSceneEvent(pos);
  App()->sendEvent(App()->findChild<MainWindow *>(), &rightClickToSceneEvent);
  events::RightClickOnScene rightClickOnSceneEvent(this->dataPtr->mouseEvent);
  App()->sendEvent(App()->findChild<MainWindow *>(), &rightClickOnSceneEvent);
}


/////////////////////////////////////////////////
void IgnRenderer::BroadcastKeyRelease()
{
  if (this->dataPtr->keyEvent.Type() == common::KeyEvent::RELEASE)
  {
    events::KeyReleaseOnScene keyRelease(this->dataPtr->keyEvent);
    App()->sendEvent(App()->findChild<MainWindow *>(), &keyRelease);
    this->dataPtr->keyEvent.SetType(common::KeyEvent::NO_EVENT);
  }
}

/////////////////////////////////////////////////
void IgnRenderer::BroadcastKeyPress()
{
  if (this->dataPtr->keyEvent.Type() == common::KeyEvent::PRESS)
  {
    events::KeyPressOnScene keyPress(this->dataPtr->keyEvent);
    App()->sendEvent(App()->findChild<MainWindow *>(), &keyPress);
    this->dataPtr->keyEvent.SetType(common::KeyEvent::NO_EVENT);
  }
}

/////////////////////////////////////////////////
void IgnRenderer::Initialize()
{
  if (this->initialized)
    return;

  std::map<std::string, std::string> params;
  params["useCurrentGLContext"] = "1";
  params["winID"] = std::to_string(
    ignition::gui::App()->findChild<ignition::gui::MainWindow *>()->
      QuickWindow()->winId());
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
    igndbg << "Create scene [" << this->sceneName << "]" << std::endl;
    scene = engine->CreateScene(this->sceneName);
    scene->SetAmbientLight(this->ambientLight);
    scene->SetBackgroundColor(this->backgroundColor);
  }

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
  this->dataPtr->camera->SetImageWidth(this->textureSize.width());
  this->dataPtr->camera->SetImageHeight(this->textureSize.height());
  this->dataPtr->camera->SetAntiAliasing(8);
  this->dataPtr->camera->SetHFOV(M_PI * 0.5);
  // setting the size and calling PreRender should cause the render texture to
  // be rebuilt
  this->dataPtr->camera->PreRender();
  this->textureId = this->dataPtr->camera->RenderTextureGLId();

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
math::Vector3d IgnRenderer::ScreenToScene(
    const math::Vector2i &_screenPos) const
{
  // TODO(ahcorde): Replace this code with function in ign-rendering
  // Require this commit
  // https://github.com/ignitionrobotics/ign-rendering/pull/363
  // in ign-rendering6

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

/////////////////////////////////////////////////
RenderThread::RenderThread()
{
  RenderWindowItem::Implementation::threads << this;
  qRegisterMetaType<RenderSync*>("RenderSync*");
}

/////////////////////////////////////////////////
void RenderThread::RenderNext(RenderSync *_renderSync)
{
  this->context->makeCurrent(this->surface);

  if (!this->ignRenderer.initialized)
  {
    // Initialize renderer
    this->ignRenderer.Initialize();
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
  this->context->makeCurrent(this->surface);

  this->ignRenderer.Destroy();

  this->context->doneCurrent();
  delete this->context;

  // schedule this to be deleted only after we're done cleaning up
  this->surface->deleteLater();

  // Stop event processing, move the thread to GUI and make sure it is deleted.
  this->exit();
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

    this->connect(this->dataPtr->renderThread, &RenderThread::TextureReady,
        node, &TextureNode::NewTexture, Qt::DirectConnection);
    this->connect(node, &TextureNode::PendingNewTexture, this->window(),
        &QQuickWindow::update, Qt::QueuedConnection);
    this->connect(this->window(), &QQuickWindow::beforeRendering, node,
        &TextureNode::PrepareNode, Qt::DirectConnection);
    this->connect(node, &TextureNode::TextureInUse, this->dataPtr->renderThread,
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
void RenderWindowItem::SetSceneService(const std::string &_service)
{
  this->dataPtr->renderThread->ignRenderer.sceneService = _service;
}

/////////////////////////////////////////////////
void RenderWindowItem::SetPoseTopic(const std::string &_topic)
{
  this->dataPtr->renderThread->ignRenderer.poseTopic = _topic;
}

/////////////////////////////////////////////////
void RenderWindowItem::SetDeletionTopic(const std::string &_topic)
{
  this->dataPtr->renderThread->ignRenderer.deletionTopic = _topic;
}

/////////////////////////////////////////////////
void RenderWindowItem::SetSceneTopic(const std::string &_topic)
{
  this->dataPtr->renderThread->ignRenderer.sceneTopic = _topic;
}

/////////////////////////////////////////////////
void RenderWindowItem::SetSkyEnabled(const bool &_sky)
{
  this->dataPtr->renderThread->ignRenderer.skyEnable = _sky;
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

    elem = _pluginElem->FirstChildElement("service");
    if (nullptr != elem && nullptr != elem->GetText())
    {
      std::string service = elem->GetText();
      renderWindow->SetSceneService(service);
    }

    elem = _pluginElem->FirstChildElement("pose_topic");
    if (nullptr != elem && nullptr != elem->GetText())
    {
      std::string topic = elem->GetText();
      renderWindow->SetPoseTopic(topic);
    }

    elem = _pluginElem->FirstChildElement("deletion_topic");
    if (nullptr != elem && nullptr != elem->GetText())
    {
      std::string topic = elem->GetText();
      renderWindow->SetDeletionTopic(topic);
    }

    elem = _pluginElem->FirstChildElement("scene_topic");
    if (nullptr != elem && nullptr != elem->GetText())
    {
      std::string topic = elem->GetText();
      renderWindow->SetSceneTopic(topic);
    }

    elem = _pluginElem->FirstChildElement("sky");
    if (nullptr != elem && nullptr != elem->GetText())
    {
      renderWindow->SetSkyEnabled(true);
      if (!elem->NoChildren())
        ignwarn << "Child elements of <sky> are not supported yet" << std::endl;
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
void RenderWindowItem::mousePressEvent(QMouseEvent *_e)
{
  auto pressPos = this->dataPtr->mouseEvent.PressPos();
  this->dataPtr->mouseEvent = convert(*_e);
  this->dataPtr->mouseEvent.SetPressPos(pressPos);

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
  this->dataPtr->mouseEvent = convert(*_e);
  this->dataPtr->mouseEvent.SetPressPos(_e->pos().x(), _e->pos().y());

  this->dataPtr->renderThread->ignRenderer.NewMouseEvent(
      this->dataPtr->mouseEvent);
}

////////////////////////////////////////////////
void RenderWindowItem::mouseMoveEvent(QMouseEvent *_e)
{
  auto event = convert(*_e);
  event.SetPressPos(this->dataPtr->mouseEvent.PressPos());

  if (!event.Dragging())
    return;

  this->dataPtr->renderThread->ignRenderer.NewMouseEvent(event);
  this->dataPtr->mouseEvent = event;
}

////////////////////////////////////////////////
void RenderWindowItem::wheelEvent(QWheelEvent *_e)
{
  this->dataPtr->mouseEvent.SetType(common::MouseEvent::SCROLL);
#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
  this->dataPtr->mouseEvent.SetPos(_e->x(), _e->y());
#else
  this->dataPtr->mouseEvent.SetPos(_e->position().x(), _e->position().y());
#endif
  double scroll = (_e->angleDelta().y() > 0) ? -1.0 : 1.0;
  this->dataPtr->renderThread->ignRenderer.NewMouseEvent(
    this->dataPtr->mouseEvent);
  this->dataPtr->mouseEvent.SetScroll(scroll, scroll);
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

// Register this plugin
IGNITION_ADD_PLUGIN(ignition::gui::plugins::MinimalScene,
                    ignition::gui::Plugin)
