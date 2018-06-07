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

#include <QtQuick/QSGGeometryNode>
#include <QtQuick/QSGTextureMaterial>
#include <QtQuick/QSGOpaqueTextureMaterial>
#include <QtQuick/QQuickWindow>


#include <ignition/common/Console.hh>
#include <ignition/common/MouseEvent.hh>
#include <ignition/common/PluginMacros.hh>
#include <ignition/math/Vector2.hh>
#include <ignition/math/Vector3.hh>
#include <ignition/rendering.hh>

#include "ignition/gui/Conversions.hh"
#include "ignition/gui/Iface.hh"
#include "ignition/gui/plugins/Scene3D.hh"

namespace ignition
{
namespace gui
{
namespace plugins
{
  /// \brief Private data class for RendereWindowItem
  class RenderWindowItemPrivate
  {
    /// brief Parent window
    public: QQuickWindow *quickWindow;

    /// \brief Render window OpenGL Context
    public: QOpenGLContext* renderWindowContext;

    /// \brief Qt OpenGL Context
    public: QOpenGLContext* qtContext;

    /// \brief Qt scene graph texture material
    public: QSGTextureMaterial material;

    /// \brief Qt scene graph texture opaque material
    public: QSGOpaqueTextureMaterial materialOpaque;

    /// \brief Qt scene graph texture
    public: QSGTexture *texture = nullptr;

    /// \brief Qt scene graph geometry
    public: QSGGeometry *geometry = nullptr;

    /// \brief Qt scene graph texture size
    public: QSize textureSize;

    /// \brief Flag to indicate if render window context has been initialized
    /// or not
    public: bool initialized = false;

    /// \brief Pointer to user camera
    public: rendering::CameraPtr camera;

    /// \brief Engine Name
    public: std::string engineName{"ogre"};

    /// \brief Scene Name
    public: std::string sceneName{"scene"};

    /// \brief Ambient light color
    public: math::Color ambientLight = math::Color(0.3, 0.3, 0.3);

    /// \brief Background color
    public: math::Color backgroundColor = math::Color(1.0, 0.3, 0.3);

    /// \brief Initial camera pose
    public: math::Pose3d cameraPose = math::Pose3d(0, 0, 5, 0, 0, 0);
  };


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

/////////////////////////////////////////////////
RenderWindowItem::RenderWindowItem(QQuickItem *_parent)
  : QQuickItem(_parent), dataPtr(new RenderWindowItemPrivate)
{

  this->setFlag(ItemHasContents);
  this->setSmooth(false);
  this->startTimer(16);

  this->dataPtr->geometry =
      new QSGGeometry(QSGGeometry::defaultAttributes_TexturedPoint2D(), 4);

  this->connect(this, &QQuickItem::windowChanged, [=](QQuickWindow *_window)
    {
      if (!_window)
      {
        igndbg << "Changed to null window" << std::endl;
        return;
      }

      this->dataPtr->quickWindow = _window;
      if (!this->dataPtr->quickWindow)
      {
        ignerr << "Null plugin QQuickWindow!" << std::endl;
        return;
      }
      // start Ogre once we are in the rendering thread (Ogre must live in the rendering thread)
      connect(this->dataPtr->quickWindow, &QQuickWindow::beforeRendering,
          this, &RenderWindowItem::InitializeEngine, Qt::DirectConnection);


    });
}

/////////////////////////////////////////////////
RenderWindowItem::~RenderWindowItem()
{
  igndbg << "Destroy camera [" << this->dataPtr->camera->Name() << "]"
         << std::endl;
  // Destroy camera
  auto scene = this->dataPtr->camera->Scene();
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

  delete this->dataPtr->geometry;
  delete this->dataPtr->texture;
}

/////////////////////////////////////////////////
void RenderWindowItem::InitializeEngine()
{
  disconnect(this->dataPtr->quickWindow, &QQuickWindow::beforeRendering,
            this, &RenderWindowItem::InitializeEngine);
  this->dataPtr->qtContext = QOpenGLContext::currentContext();
  if (!this->dataPtr->qtContext)
  {
    ignerr << "Null plugin Qt context!" << std::endl;
  }

  std::cerr << "init engine " << std::endl;

  // create a new shared OpenGL context to be used exclusively by Ogre
  this->dataPtr->renderWindowContext = new QOpenGLContext();
  this->dataPtr->renderWindowContext->setFormat(
      this->dataPtr->quickWindow->requestedFormat());
  this->dataPtr->renderWindowContext->setShareContext(this->dataPtr->qtContext);
  this->dataPtr->renderWindowContext->create();

  this->ActivateRenderWindowContext();
  // Render engine
  auto engine = rendering::engine(this->dataPtr->engineName);
  if (!engine)
  {
    ignerr << "Engine [" << this->dataPtr->engineName << "] is not supported"
           << std::endl;
    return;
  }

  // Scene
  auto scene = engine->SceneByName(this->dataPtr->sceneName);
  if (!scene)
  {
    igndbg << "Create scene [" << this->dataPtr->sceneName << "]" << std::endl;
    scene = engine->CreateScene(this->dataPtr->sceneName);
    scene->SetAmbientLight(this->dataPtr->ambientLight);
    scene->SetBackgroundColor(this->dataPtr->backgroundColor);
  }
  auto root = scene->RootVisual();

  // Camera
  this->dataPtr->camera = scene->CreateCamera();
  root->AddChild(this->dataPtr->camera);
  this->dataPtr->camera->SetLocalPose(this->dataPtr->cameraPose);
  this->dataPtr->camera->SetImageWidth(800);
  this->dataPtr->camera->SetImageHeight(600);
  this->dataPtr->camera->SetAntiAliasing(2);
//  this->dataPtr->camera->SetAspectRatio(this->width() / this->height());
  this->dataPtr->camera->SetHFOV(M_PI * 0.5);

  this->DoneRenderWindowContext();
  this->dataPtr->initialized = true;
}

/////////////////////////////////////////////////
void RenderWindowItem::ActivateRenderWindowContext()
{
  glPopAttrib();
  glPopClientAttrib();

  this->dataPtr->qtContext->functions()->glUseProgram(0);
  this->dataPtr->qtContext->doneCurrent();

  this->dataPtr->renderWindowContext->makeCurrent(this->dataPtr->quickWindow);
}


/////////////////////////////////////////////////
void RenderWindowItem::DoneRenderWindowContext()
{
  this->dataPtr->renderWindowContext->functions()->glBindBuffer(
      GL_ARRAY_BUFFER, 0);
  this->dataPtr->renderWindowContext->functions()->glBindBuffer(
      GL_ELEMENT_ARRAY_BUFFER, 0);
  this->dataPtr->renderWindowContext->functions()->glBindRenderbuffer(
      GL_RENDERBUFFER, 0);
  this->dataPtr->renderWindowContext->functions()->glBindFramebuffer(
      GL_FRAMEBUFFER_EXT, 0);

  // unbind all possible remaining buffers; just to be on safe side
  this->dataPtr->renderWindowContext->functions()->glBindBuffer(
      GL_ARRAY_BUFFER, 0);
  this->dataPtr->renderWindowContext->functions()->glBindBuffer(
      GL_ATOMIC_COUNTER_BUFFER, 0);
  this->dataPtr->renderWindowContext->functions()->glBindBuffer(
      GL_COPY_READ_BUFFER, 0);
  this->dataPtr->renderWindowContext->functions()->glBindBuffer(
      GL_COPY_WRITE_BUFFER, 0);
  this->dataPtr->renderWindowContext->functions()->glBindBuffer(
      GL_DRAW_INDIRECT_BUFFER, 0);
  this->dataPtr->renderWindowContext->functions()->glBindBuffer(
      GL_DISPATCH_INDIRECT_BUFFER, 0);
  this->dataPtr->renderWindowContext->functions()->glBindBuffer(
      GL_ELEMENT_ARRAY_BUFFER, 0);
  this->dataPtr->renderWindowContext->functions()->glBindBuffer(
      GL_PIXEL_PACK_BUFFER, 0);
  this->dataPtr->renderWindowContext->functions()->glBindBuffer(
      GL_PIXEL_UNPACK_BUFFER, 0);
  this->dataPtr->renderWindowContext->functions()->glBindBuffer(
      GL_SHADER_STORAGE_BUFFER, 0);
  this->dataPtr->renderWindowContext->functions()->glBindBuffer(
      GL_TEXTURE_BUFFER, 0);
  this->dataPtr->renderWindowContext->functions()->glBindBuffer(
      GL_TRANSFORM_FEEDBACK_BUFFER, 0);
  this->dataPtr->renderWindowContext->functions()->glBindBuffer(
      GL_UNIFORM_BUFFER, 0);
  this->dataPtr->renderWindowContext->doneCurrent();
  this->dataPtr->qtContext->makeCurrent(this->dataPtr->quickWindow);
  glPushAttrib(GL_ALL_ATTRIB_BITS);
  glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);
}

/////////////////////////////////////////////////
void RenderWindowItem::timerEvent(QTimerEvent *)
{
  this->update();
}

/////////////////////////////////////////////////
QSGNode *RenderWindowItem::updatePaintNode(QSGNode *_oldNode,
    QQuickItem::UpdatePaintNodeData *)
{
  if (!this->dataPtr->initialized)
  {
    return nullptr;
  }

  // update render window
  this->ActivateRenderWindowContext();
  this->dataPtr->camera->Update();
  this->UpdateFBO();
  this->DoneRenderWindowContext();

  if (this->width() <= 0 || this->height() <= 0 || !this->dataPtr->texture)
  {
    if (_oldNode)
    {
      delete _oldNode;
    }
    return nullptr;
  }

  QSGGeometryNode *node = static_cast<QSGGeometryNode *>(_oldNode);

  if (!node)
  {
    node = new QSGGeometryNode();
    node->setGeometry(this->dataPtr->geometry);
    node->setMaterial(&this->dataPtr->material);
    node->setOpaqueMaterial(&this->dataPtr->materialOpaque);
  }

  node->markDirty(QSGNode::DirtyGeometry);
  node->markDirty(QSGNode::DirtyMaterial);

  return node;
}


/////////////////////////////////////////////////
void RenderWindowItem::UpdateFBO()
{
  QSize s(static_cast<qint32>(this->width()),
      static_cast<qint32>(this->height()));

  if (this->width() <= 0 || this->height() <= 0 ||
      (s == this->dataPtr->textureSize))
  {
    return;
  }

  this->dataPtr->textureSize = s;

  // setting the size should cause the render texture to be rebuilt
  this->dataPtr->camera->SetImageWidth(this->dataPtr->textureSize.width());
  this->dataPtr->camera->SetImageHeight(this->dataPtr->textureSize.height());
  this->dataPtr->camera->PreRender();


  QSGGeometry::updateTexturedRectGeometry(this->dataPtr->geometry,
      QRectF(0.0, 0.0, this->dataPtr->textureSize.width(),
      this->dataPtr->textureSize.height()),
      QRectF(0.0, 0.0, 1.0, 1.0));

  delete this->dataPtr->texture;

  this->dataPtr->texture = window()->createTextureFromId(
      this->dataPtr->camera->RenderTextureGLId(),
      this->dataPtr->textureSize);

  this->dataPtr->material.setTexture(this->dataPtr->texture);
  this->dataPtr->materialOpaque.setTexture(this->dataPtr->texture);
}

/////////////////////////////////////////////////
void RenderWindowItem::SetBackgroundColor(const math::Color &_color)
{
  this->dataPtr->backgroundColor = _color;
}

/////////////////////////////////////////////////
void RenderWindowItem::SetAmbientLight(const math::Color &_ambient)
{
  this->dataPtr->ambientLight = _ambient;
}

/////////////////////////////////////////////////
void RenderWindowItem::SetEngineName(const std::string &_name)
{
  this->dataPtr->engineName = _name;
}

/////////////////////////////////////////////////
void RenderWindowItem::SetSceneName(const std::string &_name)
{
  this->dataPtr->sceneName = _name;
}

/////////////////////////////////////////////////
void RenderWindowItem::SetCameraPose(const math::Pose3d &_pose)
{
  this->dataPtr->cameraPose = _pose;
}

/////////////////////////////////////////////////
Scene3D::Scene3D()
  : Plugin(), dataPtr(new Scene3DPrivate)
{
  qmlRegisterType<RenderWindowItem>("RenderWindow", 1, 0, "RenderWindow");

  QQmlComponent component(qmlEngine(),
      QString(":/Scene3D/Scene3D.qml"), QQmlComponent::PreferSynchronous);
  this->item = qobject_cast<QQuickItem *>(component.create());
  if (!this->item)
  {
    ignerr << "Null plugin QQuickItem!" << std::endl;
    return;
  }

  qmlEngine()->rootContext()->setContextProperty("Scene3D", this);
}


/////////////////////////////////////////////////
Scene3D::~Scene3D()
{
}

/////////////////////////////////////////////////
QQuickItem *Scene3D::Item() const
{
  return this->item;
}

/////////////////////////////////////////////////
void Scene3D::LoadConfig(const tinyxml2::XMLElement *_pluginElem)
{
  RenderWindowItem *renderWindow = this->findChild<RenderWindowItem *>();
  if (!renderWindow)
  {
    ignerr << "Unable to find Render Window item. "
           << "Render window will not be created" << std::endl;
    return;
  }

  if (this->title.empty())
    this->title = "3D Scene";

  std::cerr << "load config " << std::endl;
    renderWindow->SetBackgroundColor(math::Color::Black);
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
//QPaintEngine *Scene3D::paintEngine() const
//{
//  return nullptr;
//}

/////////////////////////////////////////////////
//void Scene3D::paintEvent(QPaintEvent *_e)
//{
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
//}
//
///////////////////////////////////////////////////
//void Scene3D::resizeEvent(QResizeEvent *_e)
//{
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
//}
//
///////////////////////////////////////////////////
//void Scene3D::mousePressEvent(QMouseEvent *_e)
//{
//  auto event = convert(*_e);
//  event.SetPressPos(event.Pos());
//  this->dataPtr->mouseEvent = event;
//
//  // Update target
//  this->dataPtr->target = this->ScreenToScene(event.PressPos());
//}
//
///////////////////////////////////////////////////
//void Scene3D::mouseReleaseEvent(QMouseEvent *_e)
//{
//  this->dataPtr->mouseEvent = convert(*_e);
//
//  // Clear target
//  this->dataPtr->target = math::Vector3d::Zero;
//}
//
///////////////////////////////////////////////////
//void Scene3D::mouseMoveEvent(QMouseEvent *_e)
//{
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
//}
//
///////////////////////////////////////////////////
//void Scene3D::wheelEvent(QWheelEvent *_e)
//{
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
//}

///////////////////////////////////////////////////
//math::Vector3d RenderWindowItem::ScreenToScene(
//    const math::Vector2i &_screenPos) const
//{
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
//}

// Register this plugin
IGN_COMMON_REGISTER_SINGLE_PLUGIN(ignition::gui::plugins::Scene3D,
                                  ignition::gui::Plugin)

