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
#include <ignition/math/Color.hh>
#include <ignition/math/Pose3.hh>
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
  class RenderWindowItemPrivate
  {
    public: QQuickWindow *m_quickWindow;
    public: QOpenGLContext* m_ogreContext;
    public: QOpenGLContext* m_qtContext;


    public: QSGTextureMaterial       m_material;
    public: QSGOpaqueTextureMaterial m_materialOpaque;
    public: QSGGeometry              *m_geometry = nullptr; //(QSGGeometry::defaultAttributes_TexturedPoint2D(), 4);
    public: QSize                    size;
    public: QSGTexture               *m_texture = nullptr;

    public: bool initialized = false;

     // Default parameters
    public: std::string engineName{"ogre"};
    public: std::string sceneName{"scene"};
    public: math::Color ambientLight = math::Color(0.3, 0.3, 0.3);
    public: math::Color backgroundColor = math::Color(1.0, 0.3, 0.3);
    public: math::Pose3d cameraPose = math::Pose3d(0, 0, 5, 0, 0, 0);
    /// \brief Pointer to user camera
    public: rendering::CameraPtr camera;
  };


  class Scene3DPrivate
  {
    /// \brief Timer to repaint the widget
    // public: QTimer *updateTimer;

    /// \brief Pointer to user camera
    // public: rendering::CameraPtr camera;

    /// \brief Pointer to render window
    // public: rendering::RenderWindowPtr renderWindow;

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
RenderWindowItem::RenderWindowItem(QQuickItem *_parent)
  : QQuickItem(_parent), dataPtr(new RenderWindowItemPrivate)
{

  this->setFlag(ItemHasContents);
  this->setSmooth(false);
  this->startTimer(16);

  this->dataPtr->m_geometry =
      new QSGGeometry(QSGGeometry::defaultAttributes_TexturedPoint2D(), 4);

  this->connect(this, &QQuickItem::windowChanged, [=](QQuickWindow *_window)
    {
      if (!_window)
      {
        igndbg << "Changed to null window" << std::endl;
        return;
      }

      this->dataPtr->m_quickWindow = _window;
      if (!this->dataPtr->m_quickWindow)
      {
        ignerr << "Null plugin QQuickWindow!" << std::endl;
        return;
      }
      // start Ogre once we are in the rendering thread (Ogre must live in the rendering thread)
      connect(this->dataPtr->m_quickWindow, &QQuickWindow::beforeRendering,
          this, &RenderWindowItem::InitializeEngine, Qt::DirectConnection);
    });
}

/////////////////////////////////////////////////
RenderWindowItem::~RenderWindowItem()
{
  delete this->dataPtr->m_geometry;
}

/////////////////////////////////////////////////
void RenderWindowItem::InitializeEngine()
{
  disconnect(this->dataPtr->m_quickWindow, &QQuickWindow::beforeRendering,
            this, &RenderWindowItem::InitializeEngine);
  this->dataPtr->m_qtContext = QOpenGLContext::currentContext();
  if (!this->dataPtr->m_qtContext)
  {
    ignerr << "Null plugin Qt context! lala" << std::endl;
  }

  // create a new shared OpenGL context to be used exclusively by Ogre
  this->dataPtr->m_ogreContext = new QOpenGLContext();
  this->dataPtr->m_ogreContext->setFormat(this->dataPtr->m_quickWindow->requestedFormat());
  this->dataPtr->m_ogreContext->setShareContext(this->dataPtr->m_qtContext);
  this->dataPtr->m_ogreContext->create();
  ignerr << "created context " << std::endl;

  this->ActivateRenderWindowContext();
  // Render engine
  auto engine = rendering::engine(this->dataPtr->engineName);
  if (!engine)
  {
    ignerr << "Engine [" << this->dataPtr->engineName << "] is not supported" << std::endl;
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
  igndbg << "Create camera" << std::endl;
  this->dataPtr->camera = scene->CreateCamera();
  root->AddChild(this->dataPtr->camera);
  this->dataPtr->camera->SetLocalPose(this->dataPtr->cameraPose);
  this->dataPtr->camera->SetImageWidth(800);
  this->dataPtr->camera->SetImageHeight(600);
  this->dataPtr->camera->SetAntiAliasing(2);
//  this->dataPtr->camera->SetAspectRatio(this->width() / this->height());
  this->dataPtr->camera->SetHFOV(M_PI * 0.5);

  this->DoneRenderWindowContext();
  ignerr << "created engine" << std::endl;
  this->dataPtr->initialized = true;
}

/////////////////////////////////////////////////
void RenderWindowItem::ActivateRenderWindowContext()
{
  glPopAttrib();
  glPopClientAttrib();

  this->dataPtr->m_qtContext->functions()->glUseProgram(0);
  this->dataPtr->m_qtContext->doneCurrent();

  this->dataPtr->m_ogreContext->makeCurrent(this->dataPtr->m_quickWindow);
}


/////////////////////////////////////////////////
void RenderWindowItem::DoneRenderWindowContext()
{
  this->dataPtr->m_ogreContext->functions()->glBindBuffer(GL_ARRAY_BUFFER, 0);
  this->dataPtr->m_ogreContext->functions()->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  this->dataPtr->m_ogreContext->functions()->glBindRenderbuffer(GL_RENDERBUFFER, 0);
  this->dataPtr->m_ogreContext->functions()->glBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);

  // unbind all possible remaining buffers; just to be on safe side
  this->dataPtr->m_ogreContext->functions()->glBindBuffer(GL_ARRAY_BUFFER, 0);
  this->dataPtr->m_ogreContext->functions()->glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
  this->dataPtr->m_ogreContext->functions()->glBindBuffer(GL_COPY_READ_BUFFER, 0);
  this->dataPtr->m_ogreContext->functions()->glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
  this->dataPtr->m_ogreContext->functions()->glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
  this->dataPtr->m_ogreContext->functions()->glBindBuffer(GL_DISPATCH_INDIRECT_BUFFER, 0);
  this->dataPtr->m_ogreContext->functions()->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  this->dataPtr->m_ogreContext->functions()->glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
  this->dataPtr->m_ogreContext->functions()->glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
  this->dataPtr->m_ogreContext->functions()->glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
  this->dataPtr->m_ogreContext->functions()->glBindBuffer(GL_TEXTURE_BUFFER, 0);
  this->dataPtr->m_ogreContext->functions()->glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);
  this->dataPtr->m_ogreContext->functions()->glBindBuffer(GL_UNIFORM_BUFFER, 0);
  this->dataPtr->m_ogreContext->doneCurrent();
  this->dataPtr->m_qtContext->makeCurrent(this->dataPtr->m_quickWindow);
  glPushAttrib(GL_ALL_ATTRIB_BITS);
  glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);
}


/////////////////////////////////////////////////
void RenderWindowItem::timerEvent(QTimerEvent *)
{
  this->update();
}


/////////////////////////////////////////////////
QSGNode *RenderWindowItem::updatePaintNode(QSGNode *_oldNode, QQuickItem::UpdatePaintNodeData *)
{
  if (!this->dataPtr->initialized)
  {
    return nullptr;
  }

  this->ActivateRenderWindowContext();
  this->dataPtr->camera->Update();
  this->UpdateFBO();
  this->DoneRenderWindowContext();



  if (this->width() <= 0 || this->height() <= 0 || !this->dataPtr->m_texture)
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
    node->setGeometry(this->dataPtr->m_geometry);
    node->setMaterial(&this->dataPtr->m_material);
    node->setOpaqueMaterial(&this->dataPtr->m_materialOpaque);
  }

  node->markDirty(QSGNode::DirtyGeometry);
  node->markDirty(QSGNode::DirtyMaterial);

  std::cerr << "done update paint node " << std::endl;

  return node;
}


/////////////////////////////////////////////////
void RenderWindowItem::UpdateFBO()
{
  std::cerr << " update fbo" << std::endl;

  QSize wsz(static_cast<qint32>(this->width()),
      static_cast<qint32>(this->height()));

  std::cerr << "  wh " << this->width() << " " << this->height() << std::endl;
  std::cerr << "  size " << this->dataPtr->size.width() << " "
      << this->dataPtr->size.height() << std::endl;

  if (this->width() <= 0 || this->height() <= 0 || (wsz == this->dataPtr->size))
  {
    return;
  }

  this->dataPtr->size = wsz;

  // setting the size should cause the render texture to be rebuilt
  this->dataPtr->camera->SetImageWidth(this->dataPtr->size.width());
  this->dataPtr->camera->SetImageHeight(this->dataPtr->size.height());
  this->dataPtr->camera->PreRender();


  QSGGeometry::updateTexturedRectGeometry(this->dataPtr->m_geometry,
      QRectF(0.0, 0.0, this->dataPtr->size.width(),
      this->dataPtr->size.height()),
      QRectF(0.0, 0.0, 1.0, 1.0));

  // Ogre::GLTexture *native_texture = static_cast<Ogre::GLTexture *>(rtt.get());

  delete this->dataPtr->m_texture;


  std::cerr << " render texture gl id " << std::to_string(this->dataPtr->camera->RenderTextureGLId()) <<std::endl;
  this->dataPtr->m_texture = window()->createTextureFromId(
      this->dataPtr->camera->RenderTextureGLId(), /*native_texture->getGLID(),*/
      this->dataPtr->size);

  this->dataPtr->m_material.setTexture(this->dataPtr->m_texture);
  this->dataPtr->m_materialOpaque.setTexture(this->dataPtr->m_texture);

  std::cerr << " done update fbo" << std::endl;
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
/*  igndbg << "Destroy camera [" << this->dataPtr->camera->Name() << "]"
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
  */
}

/////////////////////////////////////////////////
void Scene3D::LoadConfig(const tinyxml2::XMLElement *_pluginElem)
{
  if (this->title.empty())
    this->title = "3D Scene";

  // Custom parameters
  /*if (_pluginElem)
  {
    if (auto elem = _pluginElem->FirstChildElement("engine"))
      this->dataPtr->engineName = elem->GetText();

    if (auto elem = _pluginElem->FirstChildElement("scene"))
      this->dataPtr->sceneName = elem->GetText();

    if (auto elem = _pluginElem->FirstChildElement("ambient_light"))
    {
      std::stringstream colorStr;
      colorStr << std::string(elem->GetText());
      colorStr >> this->dataPtr->ambientLight;
    }

    if (auto elem = _pluginElem->FirstChildElement("background_color"))
    {
      std::stringstream colorStr;
      colorStr << std::string(elem->GetText());
      colorStr >> this->dataPtr->backgroundColor;
    }

    if (auto elem = _pluginElem->FirstChildElement("camera_pose"))
    {
      std::stringstream poseStr;
      poseStr << std::string(elem->GetText());
      poseStr >> this->dataPtr->cameraPose;
    }
  }
  */

  // Layout
//  this->setLayout(new QVBoxLayout());
//
//  this->setMinimumWidth(300);
//  this->setMinimumHeight(300);
//  this->setAttribute(Qt::WA_OpaquePaintEvent, true);
//  this->setAttribute(Qt::WA_PaintOnScreen, true);
//  this->setAttribute(Qt::WA_NoSystemBackground, true);

  // Store window id
//  this->dataPtr->windowId = this->winId();

/*  // Render engine
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
//  this->dataPtr->camera->SetAspectRatio(this->width() / this->height());
  this->dataPtr->camera->SetHFOV(M_PI * 0.5);

  // Timer to repaint
  this->dataPtr->updateTimer = new QTimer(this);
  this->connect(this->dataPtr->updateTimer, SIGNAL(timeout()),
      this->item, SLOT(update()));
  this->dataPtr->updateTimer->start(std::round(1000.0 / 60.0));
  */
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

/////////////////////////////////////////////////
math::Vector3d Scene3D::ScreenToScene(const math::Vector2i &_screenPos) const
{
/*  // Normalize point on the image
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
  */
}

// Register this plugin
IGN_COMMON_REGISTER_SINGLE_PLUGIN(ignition::gui::plugins::Scene3D,
                                  ignition::gui::Plugin)

