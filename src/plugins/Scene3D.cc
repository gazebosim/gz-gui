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
#include <ignition/common/MeshManager.hh>

#include <ignition/math/Vector2.hh>
#include <ignition/math/Vector3.hh>

#include <ignition/msgs.hh>

#include <ignition/rendering.hh>

#include <ignition/transport/Node.hh>

#include "ignition/gui/Conversions.hh"
#include "ignition/gui/plugins/Scene3D.hh"

namespace ignition
{
namespace gui
{
namespace plugins
{
  /// \brief Scene requester class for making scene service request
  /// and populating the scene based on the response msg.
  class SceneRequester
  {
    /// \brief Constrcutor
    /// \param[in] _service Ign transport service name
    /// \param[in] _scene Pointer to the rendering scene
    public: SceneRequester(const std::string &_service,
        rendering::ScenePtr _scene);

    /// \brief Make the request and populate the scene
    public: void Request();

    /// \brief Load the scene from a scene msg
    /// \param[in] _msg Scene msg
    private: void LoadScene(const msgs::Scene &_msg);

    /// \brief Load the model from a model msg
    /// \param[in] _msg Model msg
    /// \return Model visual created from the msg
    private: rendering::VisualPtr LoadModel(const msgs::Model &_msg);

    /// \brief Load a link from a link msg
    /// \param[in] _msg Link msg
    /// \return Link visual created from the msg
    private: rendering::VisualPtr LoadLink(const msgs::Link &_msg);

    /// \brief Load a visual from a visual msg
    /// \param[in] _msg Visual msg
    /// \return Visual visual created from the msg
    private: rendering::VisualPtr LoadVisual(const msgs::Visual &_msg);

    /// \brief Load a geometry from a geometry msg
    /// \param[in] _msg Geometry msg
    /// \param[out] _scale Geometry scale that will be set based on msg param
    /// \return Geometry object created from the msg
    private: rendering::GeometryPtr LoadGeometry(const msgs::Geometry &_msg,
        math::Vector3d &_scale);

    /// \brief Load a material from a material msg
    /// \param[in] _msg Material msg
    /// \return Material object created from the msg
    private: rendering::MaterialPtr LoadMaterial(const msgs::Material &_msg);

    //// \brief Ign-transport service name
    private: std::string service;

    //// \brief Pointer to the rendering scene
    private: rendering::ScenePtr scene;
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

QList<QThread *> RenderWindowItem::threads;

/////////////////////////////////////////////////
SceneRequester::SceneRequester(const std::string &_service,
    rendering::ScenePtr _scene)
{
  this->service = _service;
  this->scene = _scene;
}

/////////////////////////////////////////////////
void SceneRequester::Request()
{
  // Create a transport node.
  ignition::transport::Node node;

  bool result{false};
  unsigned int timeout{5000};

  msgs::Scene res;

  // \todo(anyone) Look into using an asynchronous request, or an
  // alternative Request function that is asynchronous. This could be used
  // to make `Initialize` non-blocking.
  if (node.Request(this->service, timeout, res, result) && result)
  {
    this->LoadScene(res);
  }
  else
  {
    ignerr << "Error making service request to " << this->service
           << std::endl;
  }
}

/////////////////////////////////////////////////
void SceneRequester::LoadScene(const msgs::Scene &_msg)
{
  rendering::VisualPtr rootVis = this->scene->RootVisual();

  for (int i = 0; i < _msg.model_size(); ++i)
  {
    rendering::VisualPtr modelVis = this->LoadModel(_msg.model(i));
    if (modelVis)
      rootVis->AddChild(modelVis);
  }
}

/////////////////////////////////////////////////
rendering::VisualPtr SceneRequester::LoadModel(const msgs::Model &_msg)
{
  rendering::VisualPtr modelVis = this->scene->CreateVisual();
  if (_msg.has_pose())
    modelVis->SetLocalPose(msgs::Convert(_msg.pose()));

  for (int i = 0; i < _msg.link_size(); ++i)
  {
    rendering::VisualPtr linkVis = this->LoadLink(_msg.link(i));
    if (linkVis)
      modelVis->AddChild(linkVis);
  }
  return modelVis;
}

/////////////////////////////////////////////////
rendering::VisualPtr SceneRequester::LoadLink(const msgs::Link &_msg)
{
  rendering::VisualPtr linkVis = this->scene->CreateVisual();
  if (_msg.has_pose())
    linkVis->SetLocalPose(msgs::Convert(_msg.pose()));

  for (int i = 0; i < _msg.visual_size(); ++i)
  {
    rendering::VisualPtr visualVis = this->LoadVisual(_msg.visual(i));
    if (visualVis)
      linkVis->AddChild(visualVis);
  }
  return linkVis;
}

/////////////////////////////////////////////////
rendering::VisualPtr SceneRequester::LoadVisual(const msgs::Visual &_msg)
{
  if (!_msg.has_geometry())
    return rendering::VisualPtr();

  rendering::VisualPtr visualVis = this->scene->CreateVisual();
  if (_msg.has_pose())
    visualVis->SetLocalPose(msgs::Convert(_msg.pose()));

  math::Vector3d scale = math::Vector3d::One;
  rendering::GeometryPtr geom = this->LoadGeometry(_msg.geometry(), scale);
  if (geom)
  {
    visualVis->AddGeometry(geom);
    visualVis->SetLocalScale(scale);

    // set material
    rendering::MaterialPtr material;
    if (_msg.has_material())
    {
      material = this->LoadMaterial(_msg.material());
    }
    else
    {
      // create default material
      material = this->scene->Material("ign-grey");
      if (!material)
      {
        material = this->scene->CreateMaterial("ign-grey");
        material->SetAmbient(0.3, 0.3, 0.3);
        material->SetDiffuse(0.7, 0.7, 0.7);
        material->SetSpecular(0.4, 0.4, 0.4);
      }
    }
    geom->SetMaterial(material);
  }


  return visualVis;
}

/////////////////////////////////////////////////
rendering::GeometryPtr SceneRequester::LoadGeometry(const msgs::Geometry &_msg,
    math::Vector3d &_scale)
{
  math::Vector3d scale = math::Vector3d::One;
  rendering::GeometryPtr geom;
  if (_msg.has_box())
  {
    geom = this->scene->CreateBox();
    if (_msg.box().has_size())
      scale = msgs::Convert(_msg.box().size());
  }
  else if (_msg.has_cylinder())
  {
    geom = this->scene->CreateCylinder();
    scale.X() = _msg.cylinder().radius() * 2;
    scale.Y() = scale.X();
    scale.Z() = _msg.cylinder().length();
  }
  else if (_msg.has_sphere())
  {
    geom = this->scene->CreateSphere();
    scale.X() = _msg.sphere().radius() * 2;
    scale.Y() = scale.X();
    scale.Z() = scale.X();
  }
  else if (_msg.has_mesh())
  {
    if (_msg.mesh().filename().empty())
    {
      ignerr << "Mesh geometry missing filename" << std::endl;
      return geom;
    }
    rendering::MeshDescriptor descriptor;
    // TODO(anyone) resolve filename path?
    // currently assumes absolute path to mesh file
    descriptor.meshName = _msg.mesh().filename();

    ignition::common::MeshManager* meshManager =
        ignition::common::MeshManager::Instance();
    descriptor.mesh = meshManager->Load(descriptor.meshName);
    geom = this->scene->CreateMesh(descriptor);
  }
  else
  {
    ignerr << "Unsupported geometry type" << std::endl;
  }
  _scale = scale;
  return geom;
}

/////////////////////////////////////////////////
rendering::MaterialPtr SceneRequester::LoadMaterial(const msgs::Material &_msg)
{
  rendering::MaterialPtr material = this->scene->CreateMaterial();
  if (_msg.has_ambient())
  {
    material->SetAmbient(_msg.ambient().r(),
                         _msg.ambient().g(),
                         _msg.ambient().b());
  }
  if (_msg.has_diffuse())
  {
    material->SetDiffuse(_msg.diffuse().r(),
                         _msg.diffuse().g(),
                         _msg.diffuse().b());
  }
  if (_msg.has_specular())
  {
    material->SetDiffuse(_msg.specular().r(),
                         _msg.specular().g(),
                         _msg.specular().b());
  }
  if (_msg.has_emissive())
  {
    material->SetEmissive(_msg.emissive().r(),
                          _msg.emissive().g(),
                          _msg.emissive().b());
  }

  // TODO(anyone) ign-msgs material.proto is missing the transparency field
  // material->SetTransparent(_msg.transparency());
  return material;
}

/////////////////////////////////////////////////
void IgnRenderer::Render()
{
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

  rendering::DirectionalLightPtr light0 = scene->CreateDirectionalLight();
  light0->SetDirection(-0.5, 0.5, -1);
  light0->SetDiffuseColor(0.5, 0.5, 0.5);
  light0->SetSpecularColor(0.5, 0.5, 0.5);
  root->AddChild(light0);

  // Make service call to populate scene
  if (!this->sceneService.empty())
  {
    SceneRequester sq(this->sceneService, scene);
    sq.Request();
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

  // If that was the last sensor, destroy scene
  if (scene->SensorCount() == 0)
  {
    igndbg << "Destroy scene [" << scene->Name() << "]" << std::endl;
    engine->DestroyScene(scene);

    // TODO(anyone) If that was the last scene, terminate engine?
  }
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

  connect(this, &QObject::destroyed,
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
void RenderWindowItem::SetSceneService(const std::string &_service)
{
  this->renderThread->ignRenderer.sceneService = _service;
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

    if (auto elem = _pluginElem->FirstChildElement("service"))
    {
      std::string service = elem->GetText();
      renderWindow->SetSceneService(service);
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

