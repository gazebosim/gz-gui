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

#include <QtQuick/QSGGeometryNode>
#include <QtQuick/QSGTextureMaterial>
#include <QtQuick/QSGOpaqueTextureMaterial>
#include <QtQuick/QQuickWindow>

#include <cmath>
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

  class RenderWindowItemRenderer : public QQuickFramebufferObject::Renderer
  {
    public: RenderWindowItemRenderer(){};
    public: void render() override
            {
              if (!this->item || !this->item->window())
                return;

              if (!this->surface)
              {
                this->surface = new QOffscreenSurface();
                this->surface->setFormat(this->item->window()->requestedFormat());
                this->surface->create();
              }
/*              glPopAttrib();
              glPopClientAttrib();
              this->item->QtContext()->functions()->glUseProgram(0);
*/
              //this->
              this->item->window()->resetOpenGLState();
              this->item->QtContext()->doneCurrent();
              //this->item->RenderWindowContext()->makeCurrent(this->item->window());
              this->item->RenderWindowContext()->makeCurrent(this->surface);


              if (!this->initialized)
              {
                auto engine = rendering::engine(this->engineName);
                if (!engine)
                {
                  ignerr << "Engine [" << this->engineName << "] is not supported"
                         << std::endl;
                  return;
                }
                // Scene
                //auto scene = engine->SceneByName(this->dataPtr->sceneName);
                auto scene = engine->SceneByName(this->sceneName);
                if (!scene)
                {
                  igndbg << "Create scene [" << this->sceneName << "]" << std::endl;
                  scene = engine->CreateScene(this->sceneName);
                  //scene->SetAmbientLight(this->dataPtr->ambientLight);
                  //scene->SetBackgroundColor(this->dataPtr->backgroundColor);
                }
                auto root = scene->RootVisual();

                // Camera
                this->camera = scene->CreateCamera();
                root->AddChild(this->camera);
                this->camera->SetLocalPose(this->cameraPose);
                this->camera->SetImageWidth(this->textureSize.width());
                this->camera->SetImageHeight(this->textureSize.height());
                this->camera->SetAntiAliasing(2);
              //  this->dataPtr->camera->SetAspectRatio(this->width() / this->height());
                this->camera->SetHFOV(M_PI * 0.5);
                // setting the size should cause the render texture to be rebuilt
                this->camera->PreRender();
                this->textureId = this->camera->RenderTextureGLId();
                std::cerr << " camera render texture gl id: " << this->textureId << std::endl;

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

                this->initialized = true;
              }

              this->camera->Update();

              std::cerr << "render " << std::endl;
              update();


              this->item->RenderWindowContext()->doneCurrent();
              this->item->QtContext()->makeCurrent(this->item->window());

/*
  this->item->RenderWindowContext()->functions()->glBindBuffer(
      GL_ARRAY_BUFFER, 0);
  this->item->RenderWindowContext()->functions()->glBindBuffer(
      GL_ELEMENT_ARRAY_BUFFER, 0);
  this->item->RenderWindowContext()->functions()->glBindRenderbuffer(
      GL_RENDERBUFFER, 0);
  this->item->RenderWindowContext()->functions()->glBindFramebuffer(
      GL_FRAMEBUFFER_EXT, 0);

  // unbind all possible remaining buffers; just to be on safe side
  this->item->RenderWindowContext()->functions()->glBindBuffer(
      GL_ARRAY_BUFFER, 0);
  this->item->RenderWindowContext()->functions()->glBindBuffer(
      GL_ATOMIC_COUNTER_BUFFER, 0);
  this->item->RenderWindowContext()->functions()->glBindBuffer(
      GL_COPY_READ_BUFFER, 0);
  this->item->RenderWindowContext()->functions()->glBindBuffer(
      GL_COPY_WRITE_BUFFER, 0);
//  this->item->RenderWindowContext()->functions()->glBindBuffer(
//      GL_DRAW_INDIRECT_BUFFER, 0);
  this->item->RenderWindowContext()->functions()->glBindBuffer(
      GL_DISPATCH_INDIRECT_BUFFER, 0);
  this->item->RenderWindowContext()->functions()->glBindBuffer(
      GL_ELEMENT_ARRAY_BUFFER, 0);
  this->item->RenderWindowContext()->functions()->glBindBuffer(
      GL_PIXEL_PACK_BUFFER, 0);
  this->item->RenderWindowContext()->functions()->glBindBuffer(
      GL_PIXEL_UNPACK_BUFFER, 0);
//  this->item->RenderWindowContext()->functions()->glBindBuffer(
//      GL_SHADER_STORAGE_BUFFER, 0);
  this->item->RenderWindowContext()->functions()->glBindBuffer(
      GL_TEXTURE_BUFFER, 0);
  this->item->RenderWindowContext()->functions()->glBindBuffer(
      GL_TRANSFORM_FEEDBACK_BUFFER, 0);
  this->item->RenderWindowContext()->functions()->glBindBuffer(
      GL_UNIFORM_BUFFER, 0);

*/


//              glPushAttrib(GL_ALL_ATTRIB_BITS);
//              glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);


              this->item->window()->resetOpenGLState();

            }
    public: QOpenGLFramebufferObject *createFramebufferObject(const QSize &size) override
            {
              QOpenGLFramebufferObjectFormat format;
              format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
              format.setSamples(4);
              return new QOpenGLFramebufferObject(size, format);
            }

    //public: void SetWindow(QQuickWindow *_window){this->window = _window;};
    public: void SetItem(const RenderWindowItem *_item)
            {
              this->item = const_cast<RenderWindowItem *>(_item);
              if (!this->item)
                 std::cerr << "no item1 " << std::endl;
            };

    public: GLuint GLId() const
            {
              return this->textureId;
            }

    public: QSize Size() const
            {
              return textureSize;
            }

    private: QQuickWindow *window = nullptr;
    private: RenderWindowItem *item = nullptr;
    private: std::string sceneName{"scene"};
    private: std::string engineName{"ogre2"};
    private: rendering::CameraPtr camera;
    private: math::Pose3d cameraPose = math::Pose3d(0, 0, 5, 0, 0, 0);
    private: bool initialized = false;
    private: GLuint textureId = 0u;
    private: QSize textureSize = QSize(800, 600);
    private: QOffscreenSurface *surface = nullptr;
  };

  /// \brief Private data class for RendereWindowItem
  class RenderWindowItemPrivate
  {
    /// brief Parent window
    public: QQuickWindow *quickWindow;

    /// brief Parent window
    public: QQuickFramebufferObject::Renderer *renderer = nullptr;

    /// \brief Render window OpenGL Context
    public: QOpenGLContext* renderWindowContext = nullptr;

    /// \brief Qt OpenGL Context
    public: QOpenGLContext* qtContext = nullptr;


    /// \brief Flag to indicate if render window context has been initialized
    /// or not
    public: bool initialized = false;

    /// \brief Pointer to user camera
    public: rendering::CameraPtr camera;

    /// \brief Engine Name
    public: std::string engineName{"ogre2"};

    /// \brief Scene Name
    public: std::string sceneName{"scene"};

    /// \brief Ambient light color
    public: math::Color ambientLight = math::Color(0.3, 0.3, 0.3);

    /// \brief Background color
    public: math::Color backgroundColor = math::Color(0.3, 0.3, 0.3);

    /// \brief Initial camera pose
    public: math::Pose3d cameraPose = math::Pose3d(0, 0, 5, 0, 0, 0);

    public: QSGTexture *texture = nullptr;
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
  : QQuickFramebufferObject(_parent), dataPtr(new RenderWindowItemPrivate)
{
   std::cerr << "render item !!!!!!!!!!!!!!!!! " << std::endl;
//  this->setFlag(ItemHasContents);
//  this->setSmooth(false);
//  this->startTimer(16);

//  this->dataPtr->geometry =
//      new QSGGeometry(QSGGeometry::defaultAttributes_TexturedPoint2D(), 4);

  this->connect(this, &QQuickItem::windowChanged, [=](QQuickWindow *_window)
    {
      if (!_window)
      {
        igndbg << "Changed to null window" << std::endl;
        return;
      }
      this->dataPtr->quickWindow = _window;
      std::cerr << "got window" << std::endl;
      if (this->dataPtr->renderer)
      {
        auto r = dynamic_cast<RenderWindowItemRenderer *>(this->dataPtr->renderer);
        r->SetItem(this);
      }

      // start Ogre once we are in the rendering thread (Ogre must live in the
      // rendering thread)
      this->connect(this->dataPtr->quickWindow, &QQuickWindow::beforeRendering,
          this, &RenderWindowItem::InitializeEngine, Qt::DirectConnection);
    });
}

/////////////////////////////////////////////////
QQuickFramebufferObject::Renderer *RenderWindowItem::createRenderer() const
{
  std::cerr << "create renderer" << std::endl;
  auto r = new RenderWindowItemRenderer();
  r->SetItem(this);
  this->dataPtr->renderer = r;
  return this->dataPtr->renderer;
}

/////////////////////////////////////////////////
RenderWindowItem::~RenderWindowItem()
{
  igndbg << "Destroy camera [" << this->dataPtr->camera->Name() << "]"
         << std::endl;
  // Destroy camera
 /* auto scene = this->dataPtr->camera->Scene();
  scene->DestroyNode(this->dataPtr->camera);
  this->dataPtr->camera.reset();

  // If that was the last sensor, destroy scene
  if (scene->SensorCount() == 0)
  {
    igndbg << "Destroy scene [" << scene->Name() << "]" << std::endl;
    auto engine = scene->Engine();
    engine->DestroyScene(scene);

    // TODO(anyone): If that was the last scene, terminate engine?
  }
*/

//  delete this->dataPtr->geometry;
//  delete this->dataPtr->texture;
}

/////////////////////////////////////////////////
void RenderWindowItem::InitializeEngine()
{

  this->disconnect(this->dataPtr->quickWindow, &QQuickWindow::beforeRendering,
            this, &RenderWindowItem::InitializeEngine);
  std::cerr << "initialize engine" << std::endl;

/*
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
*/
}

/*
/////////////////////////////////////////////////
void RenderWindowItem::ActivateRenderWindowContext()
{
  glPopAttrib();
  glPopClientAttrib();

//  this->dataPtr->qtContext->functions()->glUseProgram(0);
  this->dataPtr->qtContext->doneCurrent();

  this->dataPtr->renderWindowContext->makeCurrent(this->dataPtr->quickWindow);
  this->dataPtr->quickWindow->resetOpenGLState();
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
//  this->dataPtr->renderWindowContext->functions()->glBindBuffer(
//      GL_DRAW_INDIRECT_BUFFER, 0);
  this->dataPtr->renderWindowContext->functions()->glBindBuffer(
      GL_DISPATCH_INDIRECT_BUFFER, 0);
  this->dataPtr->renderWindowContext->functions()->glBindBuffer(
      GL_ELEMENT_ARRAY_BUFFER, 0);
  this->dataPtr->renderWindowContext->functions()->glBindBuffer(
      GL_PIXEL_PACK_BUFFER, 0);
  this->dataPtr->renderWindowContext->functions()->glBindBuffer(
      GL_PIXEL_UNPACK_BUFFER, 0);
//  this->dataPtr->renderWindowContext->functions()->glBindBuffer(
//      GL_SHADER_STORAGE_BUFFER, 0);
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
*/

/////////////////////////////////////////////////
QOpenGLContext *RenderWindowItem::RenderWindowContext() const
{
  return this->dataPtr->renderWindowContext;
}

/////////////////////////////////////////////////
QOpenGLContext *RenderWindowItem::QtContext() const
{
  return this->dataPtr->qtContext;
}

/////////////////////////////////////////////////
QSGNode *RenderWindowItem::updatePaintNode(QSGNode *_node,
    QQuickItem::UpdatePaintNodeData *_data)
{
  std::cerr << "update paint node " << std::endl;
  QSGNode *out = QQuickFramebufferObject::updatePaintNode(_node, _data);

  if (!this->dataPtr->renderWindowContext)
  {
    this->dataPtr->qtContext = QOpenGLContext::currentContext();
    if (!this->dataPtr->qtContext)
    {
      ignerr << "Null plugin Qt context!" << std::endl;
    }
    this->dataPtr->qtContext->doneCurrent();
    // create a new shared OpenGL context to be used exclusively by Ogre
    this->dataPtr->renderWindowContext = new QOpenGLContext();
    this->dataPtr->renderWindowContext->setFormat(
        this->dataPtr->quickWindow->requestedFormat());
    this->dataPtr->renderWindowContext->setShareContext(this->dataPtr->qtContext);
    this->dataPtr->renderWindowContext->create();
    this->dataPtr->qtContext->makeCurrent(this->dataPtr->quickWindow);
  }



//  std::cerr << "node type: " << static_cast<int>(_node->type()) << std::endl;

  QSGSimpleTextureNode *n = dynamic_cast<QSGSimpleTextureNode *>(out);
  if (!n && (width() <= 0 || height() <= 0))
    return nullptr;
/*
  std::cerr << "update paint node r1 " << std::endl;

  if (!this->dataPtr->texture && this->dataPtr->renderer)
  {

  std::cerr << "update paint node r2 " << std::endl;
    auto r = dynamic_cast<RenderWindowItemRenderer *>(this->dataPtr->renderer);
  std::cerr << "update paint node r3 " << r->GLId() << std::endl;
    if (r && r->GLId() > 0)
    {
      std::cerr << "creating render gl texture with id: " << r->GLId() << " " << r->Size().width() << " " << r->Size().height() <<  std::endl;
      this->dataPtr->texture = window()->createTextureFromId(
        r->GLId(), r->Size());
      std::cerr << "created texture " << this->dataPtr->texture <<  " vs " << n << std::endl;

      // this should cause the original texture to be deleted
      n->setTexture(this->dataPtr->texture);
    }
    else
    {
      this->update();
    }
  }
  std::cerr << "update paint node done " << std::endl;
*/
  return n;
}

/*
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

   std::cerr << "render texture id : " << this->dataPtr->camera->RenderTextureGLId() << std::endl;

  this->dataPtr->material.setTexture(this->dataPtr->texture);
  this->dataPtr->materialOpaque.setTexture(this->dataPtr->texture);
}
*/

/*
RenderWindowNode::RenderWindowNode()
  : dataPtr(new RenderWindowNodePrivate)
{
  this->dataPtr->geometry =
      new QSGGeometry(QSGGeometry::defaultAttributes_TexturedPoint2D(), 4);
  this->setMaterial(&this->dataPtr->material);
  this->setOpaqueMaterial(&this->dataPtr->materialOpaque);
  this->setGeometry(this->dataPtr->geometry);
  this->setFlag(UsePreprocess);
}

RenderWindowNode::~RenderWindowNode()
{
  delete this->dataPtr->geometry;
  delete this->dataPtr->texture;
}

void RenderWindowNode::ActivateContext()
{
  if (!this->dataPtr->renderWindowItem)
    return;
  this->dataPtr->renderWindowItem->ActivateRenderWindowContext();
  this->dataPtr->renderWindowItem->RenderWindowContext()->functions()->glBindFramebuffer(
      GL_FRAMEBUFFER_EXT, this->dataPtr->glId);
}

void RenderWindowNode::DoneContext()
{
  if (!this->dataPtr->renderWindowItem)
    return;
  this->dataPtr->renderWindowItem->DoneRenderWindowContext();

}

void RenderWindowNode::SetRenderWindowItem(RenderWindowItem *_item)
{
  this->dataPtr->renderWindowItem = _item;
}

void RenderWindowNode::SetCamera(rendering::CameraPtr _camera)
{
  this->dataPtr->camera = _camera;
}

void RenderWindowNode::SetSize(QSize _size)
{
  if (_size == this->dataPtr->textureSize)
  {
    return;
  }
  this->dataPtr->textureSize = _size;
  this->dataPtr->fboDirty = true;
  this->markDirty(QSGNode::DirtyGeometry);
}

void RenderWindowNode::update()
{
  if (this->dataPtr->fboDirty)
  {
    this->ActivateContext();
    this->UpdateFBO();
    this->dataPtr->fboDirty = false;
    this->DoneContext();
  }
}

void RenderWindowNode::UpdateFBO()
{
  if (!this->dataPtr->camera)
    return;

  if (this->dataPtr->textureSize.width() == 0 ||
      this->dataPtr->textureSize.height() == 0)
    return;

  // setting the size should cause the render texture to be rebuilt
  this->dataPtr->camera->SetImageWidth(this->dataPtr->textureSize.width());
  this->dataPtr->camera->SetImageHeight(this->dataPtr->textureSize.height());
  this->dataPtr->camera->PreRender();


  QSGGeometry::updateTexturedRectGeometry(this->dataPtr->geometry,
      QRectF(0.0, 0.0, this->dataPtr->textureSize.width(),
      this->dataPtr->textureSize.height()),
      QRectF(0.0, 0.0, 1.0, 1.0));

  delete this->dataPtr->texture;

  this->dataPtr->glId = this->dataPtr->camera->RenderTextureGLId();
  this->dataPtr->texture =
      this->dataPtr->renderWindowItem->window()->createTextureFromId(
      this->dataPtr->glId,
      this->dataPtr->textureSize, QQuickWindow::TextureIsOpaque);

  this->dataPtr->material.setTexture(this->dataPtr->texture);
  this->dataPtr->materialOpaque.setTexture(this->dataPtr->texture);

  this->dataPtr->camera->PostRender();
}

void RenderWindowNode::preprocess()
{
  this->ActivateContext();
  static bool done = false;
  if (!done)
  {
    auto scene = this->dataPtr->camera->Scene();
    auto root = scene->RootVisual();
    auto grid = scene->CreateGrid();
    grid->SetCellCount(200);
    grid->SetVerticalCellCount(0);
    grid->SetCellLength(1.0);

    auto gridVis = scene->CreateVisual();
    root->AddChild(gridVis);
    gridVis->AddGeometry(grid);

    auto mat = scene->CreateMaterial();
    gridVis->SetMaterial(mat);

    auto box = scene->CreateBox();
    auto boxVis = scene->CreateVisual();
    root->AddChild(boxVis);
    boxVis->AddGeometry(box);
    boxVis->SetLocalPosition(
        this->dataPtr->camera->LocalPosition() +
        ignition::math::Vector3d(2, 0, 0));

    done = true;
  }


  this->dataPtr->camera->Update();
  this->DoneContext();
}
*/

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

