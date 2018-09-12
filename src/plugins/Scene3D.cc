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
#include <map>
#include <sstream>
#include <string>

#include <ignition/common/Console.hh>
#include <ignition/common/MouseEvent.hh>
#include <ignition/common/PluginMacros.hh>
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

QList<QThread *> RenderWindowItem::threads;

// TODO(anyone) Remove test code after integration with scene service
bool testScene = true;

/////////////////////////////////////////////////
void IgnRenderer::Render()
{
  // TODO(anyone) Remove test code after integration with scene service
  if (testScene)
  {
    this->camera->SetLocalPosition(this->camera->WorldPosition()
        + ignition::math::Vector3d(0.001, 0.001, 0));
  }

  if (this->textureDirty)
  {
    this->camera->SetImageWidth(this->textureSize.width());
    this->camera->SetImageHeight(this->textureSize.height());
    this->camera->SetAspectRatio(this->textureSize.width() /
        this->textureSize.height());
    // setting the size should cause the render texture to be rebuilt
    this->camera->PreRender();
    this->textureId = this->camera->RenderTextureGLId();
    this->textureDirty = false;
  }

  this->camera->Update();
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
    igndbg << "Create scene [" << this->sceneName << "]" << std::endl;
    scene = engine->CreateScene(this->sceneName);
    scene->SetAmbientLight(this->ambientLight);
    scene->SetBackgroundColor(this->backgroundColor);
  }
  auto root = scene->RootVisual();

  // Camera
  this->camera = scene->CreateCamera();
  root->AddChild(this->camera);
  this->camera->SetLocalPose(this->cameraPose);
  this->camera->SetImageWidth(this->textureSize.width());
  this->camera->SetImageHeight(this->textureSize.height());
  this->camera->SetAntiAliasing(8);
  this->camera->SetHFOV(M_PI * 0.5);
  // setting the size and calling PreRender should cause the render texture to
  //  be rebuilt
  this->camera->PreRender();
  this->textureId = this->camera->RenderTextureGLId();

  // TODO(anyone) Remove test code after integration with scene service
  if (testScene)
  {
    rendering::DirectionalLightPtr light0 = scene->CreateDirectionalLight();
    light0->SetDirection(-0.5, 0.5, -1);
    light0->SetDiffuseColor(0.5, 0.5, 0.5);
    light0->SetSpecularColor(0.5, 0.5, 0.5);
    root->AddChild(light0);

    auto mat = scene->CreateMaterial();
    mat->SetDiffuse(0, 1, 0);
    auto sphere = scene->CreateSphere();
    auto sphereVis = scene->CreateVisual();
    root->AddChild(sphereVis);
    sphereVis->AddGeometry(sphere);
    sphereVis->SetMaterial(mat);
    sphereVis->SetLocalPosition(
        this->camera->LocalPosition() +
        ignition::math::Vector3d(2, 0, 0));
  }

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
  scene->DestroySensor(this->camera);
}

/////////////////////////////////////////////////
RenderThread::RenderThread()
{
  RenderWindowItem::threads << this;
}

/////////////////////////////////////////////////
void RenderThread::RenderNext()
{
  this->context->makeCurrent(surface);

  if (!this->ignRenderer.initialized)
  {
    // Initialize renderer
    this->ignRenderer.Initialize();
  }

  this->ignRenderer.Render();

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
  this->moveToThread(QGuiApplication::instance()->thread());
}


/////////////////////////////////////////////////
void RenderThread::SizeChanged(const QSize &_size)
{
  this->ignRenderer.textureSize = _size;
  this->ignRenderer.textureDirty = true;
}

/////////////////////////////////////////////////
TextureNode::TextureNode(QQuickWindow *_window)
    : window(_window)
{
  // Our texture node must have a texture, so use the default 0 texture.
  this->texture = this->window->createTextureFromId(0, QSize(1, 1));
  this->setTexture(this->texture);
}

/////////////////////////////////////////////////
TextureNode::~TextureNode()
{
  delete this->texture;
}

/////////////////////////////////////////////////
void TextureNode::NewTexture(int _id, const QSize &_size)
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
  int newId = this->id;
  QSize sz = this->size;
  this->id = 0;
  this->mutex.unlock();
  if (newId)
  {
    delete this->texture;
    // note: include QQuickWindow::TextureHasAlphaChannel if the rendered
    // content has alpha.
    this->texture = this->window->createTextureFromId(newId, sz);
    this->setTexture(this->texture);

    this->markDirty(DirtyMaterial);

    // This will notify the rendering thread that the texture is now being
    // rendered and it can start rendering to the other one.
    emit TextureInUse();
  }
}

/////////////////////////////////////////////////
RenderWindowItem::RenderWindowItem(QQuickItem *_parent)
  : QQuickItem(_parent)
{
  this->setFlag(ItemHasContents);
  this->renderThread = new RenderThread();
}

/////////////////////////////////////////////////
RenderWindowItem::~RenderWindowItem()
{
}

/////////////////////////////////////////////////
void RenderWindowItem::Ready()
{
  this->renderThread->surface = new QOffscreenSurface();
  this->renderThread->surface->setFormat(this->renderThread->context->format());
  this->renderThread->surface->create();

  this->renderThread->moveToThread(this->renderThread);

  this->connect(this->window(), &QQuickWindow::sceneGraphInvalidated,
      this->renderThread, &RenderThread::ShutDown, Qt::QueuedConnection);

  this->renderThread->start();
  this->update();
}

/////////////////////////////////////////////////
QSGNode *RenderWindowItem::updatePaintNode(QSGNode *_node,
    QQuickItem::UpdatePaintNodeData */*_data*/)
{
  TextureNode *node = static_cast<TextureNode *>(_node);

  if (!this->renderThread->context)
  {
    QOpenGLContext *current = this->window()->openglContext();
    // Some GL implementations require that the currently bound context is
    // made non-current before we set up sharing, so we doneCurrent here
    // and makeCurrent down below while setting up our own context.
    current->doneCurrent();

    this->renderThread->context = new QOpenGLContext();
    this->renderThread->context->setFormat(current->format());
    this->renderThread->context->setShareContext(current);
    this->renderThread->context->create();
    this->renderThread->context->moveToThread(this->renderThread);

    current->makeCurrent(this->window());

    QMetaObject::invokeMethod(this, "Ready");
    return nullptr;
  }

  if (!node)
  {
    node = new TextureNode(this->window());

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

    this->connect(this->renderThread, &RenderThread::TextureReady,
        node, &TextureNode::NewTexture, Qt::DirectConnection);
    this->connect(node, &TextureNode::PendingNewTexture, this->window(),
        &QQuickWindow::update, Qt::QueuedConnection);
    this->connect(this->window(), &QQuickWindow::beforeRendering, node,
        &TextureNode::PrepareNode, Qt::DirectConnection);
    this->connect(node, &TextureNode::TextureInUse, this->renderThread,
        &RenderThread::RenderNext, Qt::QueuedConnection);

    // Get the production of FBO textures started..
    QMetaObject::invokeMethod(this->renderThread, "RenderNext",
        Qt::QueuedConnection);
  }

  node->setRect(this->boundingRect());

  return node;
}

/////////////////////////////////////////////////
void RenderWindowItem::SetBackgroundColor(const math::Color &_color)
{
  this->renderThread->ignRenderer.backgroundColor = _color;
}

/////////////////////////////////////////////////
void RenderWindowItem::SetAmbientLight(const math::Color &_ambient)
{
  this->renderThread->ignRenderer.ambientLight = _ambient;
}

/////////////////////////////////////////////////
void RenderWindowItem::SetEngineName(const std::string &_name)
{
  this->renderThread->ignRenderer.engineName = _name;
}

/////////////////////////////////////////////////
void RenderWindowItem::SetSceneName(const std::string &_name)
{
  this->renderThread->ignRenderer.sceneName = _name;
}

/////////////////////////////////////////////////
void RenderWindowItem::SetCameraPose(const math::Pose3d &_pose)
{
  this->renderThread->ignRenderer.cameraPose = _pose;
}

/////////////////////////////////////////////////
Scene3D::Scene3D()
  : Plugin(), dataPtr(new Scene3DPrivate)
{
  qmlRegisterType<RenderWindowItem>("RenderWindow", 1, 0, "RenderWindow");
}


/////////////////////////////////////////////////
Scene3D::~Scene3D()
{
}

/////////////////////////////////////////////////
void Scene3D::LoadConfig(const tinyxml2::XMLElement *_pluginElem)
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

  // Custom parameters
  if (_pluginElem)
  {
    if (auto elem = _pluginElem->FirstChildElement("engine"))
      renderWindow->SetEngineName(elem->GetText());

    if (auto elem = _pluginElem->FirstChildElement("scene"))
      renderWindow->SetSceneName(elem->GetText());

    if (auto elem = _pluginElem->FirstChildElement("ambient_light"))
    {
      math::Color ambient;
      std::stringstream colorStr;
      colorStr << std::string(elem->GetText());
      colorStr >> ambient;
      renderWindow->SetAmbientLight(ambient);
    }

    if (auto elem = _pluginElem->FirstChildElement("background_color"))
    {
      math::Color bgColor;
      std::stringstream colorStr;
      colorStr << std::string(elem->GetText());
      colorStr >> bgColor;
      renderWindow->SetBackgroundColor(bgColor);
    }

    if (auto elem = _pluginElem->FirstChildElement("camera_pose"))
    {
      math::Pose3d pose;
      std::stringstream poseStr;
      poseStr << std::string(elem->GetText());
      poseStr >> pose;
      renderWindow->SetCameraPose(pose);
    }
  }
}

/////////////////////////////////////////////////
// QPaintEngine *Scene3D::paintEngine() const
// {
//   return nullptr;
// }

/////////////////////////////////////////////////
// void Scene3D::paintEvent(QPaintEvent *_e)
// {
//  // Create render window on first paint, so we're sure the window is showing
//  // when we attach to it
//  if (!this->dataPtr->renderWindow)
//  {
//    this->dataPtr->renderWindow = this->dataPtr->camera->CreateRenderWindow();
//    this->dataPtr->renderWindow->SetHandle(
//        std::to_string(static_cast<uint64_t>(this->dataPtr->windowId)));
//    this->dataPtr->renderWindow->SetWidth(this->width());
//    this->dataPtr->renderWindow->SetHeight(this->height());
//  }
//
//  if (this->dataPtr->camera && this->dataPtr->renderWindow)
//    this->dataPtr->camera->Update();
//
//  _e->accept();
// }
//
///////////////////////////////////////////////////
// void Scene3D::resizeEvent(QResizeEvent *_e)
// {
//  if (this->dataPtr->renderWindow)
//  {
//    this->dataPtr->renderWindow->OnResize(_e->size().width(),
//                                          _e->size().height());
//  }
//
//  if (this->dataPtr->camera)
//  {
//    this->dataPtr->camera->SetAspectRatio(
//        static_cast<double>(this->width()) / this->height());
//    this->dataPtr->camera->SetHFOV(M_PI * 0.5);
//  }
// }
//
///////////////////////////////////////////////////
// void Scene3D::mousePressEvent(QMouseEvent *_e)
// {
//  auto event = convert(*_e);
//  event.SetPressPos(event.Pos());
//  this->dataPtr->mouseEvent = event;
//
//  // Update target
//  this->dataPtr->target = this->ScreenToScene(event.PressPos());
// }
//
///////////////////////////////////////////////////
// void Scene3D::mouseReleaseEvent(QMouseEvent *_e)
// {
//  this->dataPtr->mouseEvent = convert(*_e);
//
//  // Clear target
//  this->dataPtr->target = math::Vector3d::Zero;
// }
//
///////////////////////////////////////////////////
// void Scene3D::mouseMoveEvent(QMouseEvent *_e)
// {
//  auto event = convert(*_e);
//  event.SetPressPos(this->dataPtr->mouseEvent.PressPos());
//
//  if (!event.Dragging())
//    return;
//
//  auto dragInt = event.Pos() - this->dataPtr->mouseEvent.Pos();
//  auto dragDistance = math::Vector2d(dragInt.X(), dragInt.Y());
//
//  rendering::OrbitViewController controller;
//  controller.SetCamera(this->dataPtr->camera);
//  controller.SetTarget(this->dataPtr->target);
//
//  // Pan with left button
//  if (event.Buttons() & common::MouseEvent::LEFT)
//    controller.Pan(dragDistance);
//  // Orbit with middle button
//  else if (event.Buttons() & common::MouseEvent::MIDDLE)
//    controller.Orbit(dragDistance);
//
//  this->dataPtr->mouseEvent = event;
// }
//
///////////////////////////////////////////////////
// void Scene3D::wheelEvent(QWheelEvent *_e)
// {
//  // 3D target
//  auto target = this->ScreenToScene(math::Vector2i(_e->x(), _e->y()));
//
//  // Scroll amount
//  double distance = this->dataPtr->camera->WorldPosition().Distance(target);
//  double scroll = (_e->angleDelta().y() > 0) ? -1.0 : 1.0;
//  double amount = -scroll * distance / 5.0;
//
//  // Zoom
//  rendering::OrbitViewController controller;
//  controller.SetCamera(this->dataPtr->camera);
//  controller.SetTarget(target);
//  controller.Zoom(amount);
// }

///////////////////////////////////////////////////
// math::Vector3d RenderWindowItem::ScreenToScene(
//    const math::Vector2i &_screenPos) const
// {
//  // Normalize point on the image
//  double width = this->dataPtr->camera->ImageWidth();
//  double height = this->dataPtr->camera->ImageHeight();
//
//  double nx = 2.0 * _screenPos.X() / width - 1.0;
//  double ny = 1.0 - 2.0 * _screenPos.Y() / height;
//
//  // Make a ray query
//  auto rayQuery = this->dataPtr->camera->Scene()->CreateRayQuery();
//  rayQuery->SetFromCamera(this->dataPtr->camera, math::Vector2d(nx, ny));
//
//  auto result = rayQuery->ClosestPoint();
//  if (result)
//    return result.point;
//
//  // Set point to be 10m away if no intersection found
//  return rayQuery->Origin() + rayQuery->Direction() * 10;
// }

// Register this plugin
IGN_COMMON_REGISTER_SINGLE_PLUGIN(ignition::gui::plugins::Scene3D,
                                  ignition::gui::Plugin)

