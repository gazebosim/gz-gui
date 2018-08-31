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
#include <ignition/common/Image.hh>
#include <ignition/common/MouseEvent.hh>
#include <ignition/common/PluginMacros.hh>
#include <ignition/math/Vector2.hh>
#include <ignition/math/Vector3.hh>
#include <ignition/rendering.hh>

//#include <GL/glext.h>

#include "ignition/gui/Conversions.hh"
#include "ignition/gui/plugins/Scene3D.hh"

bool checkFramebufferStatus(QOpenGLContext *ctx)
{
    if (!ctx)
        return false;   // Context no longer exists.
    GLenum status = ctx->functions()->glCheckFramebufferStatus(GL_FRAMEBUFFER);
    switch(status) {
    case GL_NO_ERROR:
    case GL_FRAMEBUFFER_COMPLETE:
        return true;
    case GL_FRAMEBUFFER_UNSUPPORTED:
        std::cerr << "QOpenGLFramebufferObject: Unsupported framebuffer format." << std::endl;
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
        std::cerr << "QOpenGLFramebufferObject: Framebuffer incomplete attachment."<< std::endl;
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
        std::cerr << "QOpenGLFramebufferObject: Framebuffer incomplete, missing attachment."<< std::endl;
        break;
#ifdef GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT
    case GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT:
        std::cerr << "QOpenGLFramebufferObject: Framebuffer incomplete, duplicate attachment."<< std::endl;
        break;
#endif
#ifdef GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS
    case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
        std::cerr << "QOpenGLFramebufferObject: Framebuffer incomplete, attached images must have same dimensions."<< std::endl;
        break;
#endif
#ifdef GL_FRAMEBUFFER_INCOMPLETE_FORMATS
    case GL_FRAMEBUFFER_INCOMPLETE_FORMATS:
        std::cerr << "QOpenGLFramebufferObject: Framebuffer incomplete, attached images must have same format."<< std::endl;
        break;
#endif
#ifdef GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER
    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
        std::cerr << "QOpenGLFramebufferObject: Framebuffer incomplete, missing draw buffer."<< std::endl;
        break;
#endif
#ifdef GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER
    case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
        std::cerr << "QOpenGLFramebufferObject: Framebuffer incomplete, missing read buffer."<< std::endl;
        break;
#endif
#ifdef GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE
    case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
        std::cerr << "QOpenGLFramebufferObject: Framebuffer incomplete, attachments must have same number of samples per pixel."<< std::endl;
        break;
#endif
    default:
        std::cerr << "QOpenGLFramebufferObject: An undefined error has occurred: "<< status << std::endl;
        break;
    }
    return false;
}



namespace ignition
{
namespace gui
{
namespace plugins
{

  class RenderWindowItemRenderer : public QQuickFramebufferObject::Renderer
  {
    public: RenderWindowItemRenderer(){};
    public: ~RenderWindowItemRenderer()
            {
            };
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


//              this->fbo->bindDefault();

//              this->item->window()->resetOpenGLState();
              this->item->QtContext()->functions()->glUseProgram(0);
              this->item->QtContext()->doneCurrent();
              this->item->RenderWindowContext()->makeCurrent(this->item->window());
//              this->item->RenderWindowContext()->makeCurrent(this->surface);

//                auto ct = QOpenGLContext::currentContext();
//                std::cerr << " renderwindow context " << ct->nativeHandle().toString().toStdString() << std::endl;

/*                // restore original state
                glPopAttrib();
                glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);
                // restore matrices
                glMatrixMode(GL_TEXTURE);
                glPopMatrix();
                glMatrixMode(GL_PROJECTION);
                glPopMatrix();
                glMatrixMode(GL_MODELVIEW);
                glPopMatrix();
*/


              if (!this->initialized)
              {
                auto engine = rendering::engine(this->engineName);
                if (!engine)
                {
                  ignerr << "Engine [" << this->engineName << "] is not supported"
                         << std::endl;
                  return;
                }
                auto c = QOpenGLContext::currentContext();
                std::cerr << " engine context " << c->nativeHandle().typeName() << std::endl;
                this->ctx = QOpenGLContext::currentContext();
                if (!this->ctx)
                  std::cerr << "no contexti !!!!!!!!!!!!! " << std::endl;

                // Scene
                auto scene = engine->SceneByName(this->sceneName);
                if (!scene)
                {
                  igndbg << "Create scene [" << this->sceneName << "]" << std::endl;
                  scene = engine->CreateScene(this->sceneName);
                  scene->SetAmbientLight(0.8, 0.8, 0.8);
                  //scene->SetBackgroundColor(this->dataPtr->backgroundColor);
                }
                auto root = scene->RootVisual();

                rendering::DirectionalLightPtr light0 = scene->CreateDirectionalLight();
                light0->SetDirection(-0.5, 0.5, -1);
                light0->SetDiffuseColor(0.5, 0.5, 0.5);
                light0->SetSpecularColor(0.5, 0.5, 0.5);
                root->AddChild(light0);


                // Camera
                this->camera = scene->CreateCamera();
                root->AddChild(this->camera);
                this->camera->SetLocalPose(this->cameraPose);
                this->camera->SetImageWidth(this->textureSize.width());
                this->camera->SetImageHeight(this->textureSize.height());
                this->camera->SetAntiAliasing(0);
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

                rendering::Image image = this->camera->CreateImage();
                this->img = std::make_shared<rendering::Image>(image);
                this->initialized = true;
              }

              static bool donee = false;
              if (!donee)
              {
                donee = true;
              for (unsigned int i = 0; i < 50; ++i)
              {
//              this->ctx->makeCurrent(this->surface);

//              this->item->RenderWindowContext()->functions()->glBindFramebuffer(
//                  GL_FRAMEBUFFER_EXT, this->textureId);

//               QOpenGLContext::currentContext()->functions()->glViewport(
//                   0, 0, this->img->Width(), this->img->Height());
              this->camera->SetLocalPosition(this->camera->WorldPosition() + ignition::math::Vector3d(0.01, 0.01, 0));
              std::cerr << "camera pose " << this->camera->LocalPosition() << std::endl;
              this->camera->Update();
//              this->item->RenderWindowContext()->functions()->glFlush();
              // TODO why do we need to render and blit 2 times for it to render properly??!?!
              this->camera->Capture(*this->img);

              const unsigned char *d = this->img->Data<unsigned char>();
              common::Image im;
              im.SetFromData(d, this->img->Width(), this->img->Height(), common::Image::RGB_INT8);
              im.SavePNG("bbb" + std::to_string(i) + ".png");
              }
              }

//              this->item->RenderWindowContext()->swapBuffers(this->surface);
//              this->camera->Capture(*this->img);
//              this->camera->Copy(*this->img);
//              this->camera->Update();
//              this->camera->Copy(*this->img);

/*
              static bool donee = false;
              if (!donee)
              {
              const unsigned char *d = this->img->Data<unsigned char>();
              common::Image im;
              im.SetFromData(d, this->img->Width(), this->img->Height(), common::Image::RGB_INT8);
              im.SavePNG("blit.png");
              donee = true;
              }
*/

/*
              // blit?
              GLuint fboId = 4;
              QOpenGLContext *ctx = QOpenGLContext::currentContext();
              if (!ctx)
                std::cerr << "not ctx " << std::endl;;
             // QOpenGLExtensions ext(ctx);
              QOpenGLExtraFunctions ext(ctx);
//              if (!ext.hasOpenGLExtension(QOpenGLExtensions::FramebufferBlit))
//                std::cerr << "no blit!!!" << std::endl;;
              ext.glBindFramebuffer(GL_READ_FRAMEBUFFER, fboId);
              ext.glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->fbo->handle());
              std::cerr << "blitting " << fboId << " " << this->fbo->handle() << std::endl;
              ext.glBlitFramebuffer(0, 0, this->textureSize.width(), this->textureSize.height(),
                                   0, 0, this->textureSize.width(), this->textureSize.height(),
                                   GL_COLOR_BUFFER_BIT, GL_NEAREST);

              bool status = checkFramebufferStatus(ctx);
              std::cerr << "status " << status << std::endl;

              ext.glBindFramebuffer(GL_FRAMEBUFFER, this->fbo->handle());

//              std::cerr << "render " << std::endl;

              update();


              this->item->RenderWindowContext()->doneCurrent();
              this->item->QtContext()->makeCurrent(this->item->window());
*/

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

//              this->fbo->bind();


/*           	  glMatrixMode(GL_MODELVIEW);
           	  glPushMatrix();
           	  glMatrixMode(GL_PROJECTION);
           	  glPushMatrix();
           	  glMatrixMode(GL_TEXTURE);
           	  glPushMatrix();
           	  glLoadIdentity(); //Texture addressing should start out as direct.
              glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);
              glPushAttrib(GL_ALL_ATTRIB_BITS);
*/


            }

    public: void synchronize(QQuickFramebufferObject *_item) override
            {
              this->item = static_cast<RenderWindowItem *>(_item);
              if (this->img)
              {
                this->item->SetImage(QSize(this->img->Width(), this->img->Height()),
                    this->img->Data<unsigned char>());
              }
            }

    public: QOpenGLFramebufferObject *createFramebufferObject(const QSize &size) override
            {
              QOpenGLFramebufferObjectFormat format;
//              format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
              format.setAttachment(QOpenGLFramebufferObject::Depth);
              format.setSamples(0);
              this->fbo = new QOpenGLFramebufferObject(size, format);
              return this->fbo;
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

    public: rendering::ImagePtr Image() const
            {
              return this->img;
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
    private: QOpenGLFramebufferObject *fbo = nullptr;
    private: rendering::ImagePtr img;
    private: QOpenGLContext *ctx = nullptr;
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
    public: QImage qimg;
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
  this->setFlag(ItemHasContents);
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
  delete this->dataPtr->texture;
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
void RenderWindowItem::SetImage(QSize _size, unsigned char *_data)
{
  if (this->dataPtr->qimg.isNull())
    this->dataPtr->qimg = QImage(_size.width(), _size.height(), QImage::Format_RGB888);

  for (int i = 0; i < this->dataPtr->qimg.height(); ++i)
  {
    memcpy(this->dataPtr->qimg.scanLine(i), _data + i * this->dataPtr->qimg.bytesPerLine(),
    this->dataPtr->qimg.bytesPerLine());
  }

/*  this->dataPtr->qimg.save("qsync.png");
  common::Image im;
  im.SetFromData(_data, _size.width(), _size.height(), common::Image::RGB_INT8);
  im.SavePNG("sync.png");
*/
}

/////////////////////////////////////////////////
QSGNode *RenderWindowItem::updatePaintNode(QSGNode *_node,
    QQuickItem::UpdatePaintNodeData *_data)
{
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


//    std::cerr << this->dataPtr->quickWindow->requestedFormat().samples() << std::endl;;

  std::cerr << "update paint node " << std::endl;
  QSGNode *out = QQuickFramebufferObject::updatePaintNode(_node, _data);
//  return out;

/*  FboNode *n = static_cast<FboNode *>(_node);
  if (!_n)
  {
    n = new FboNode;
  }

  if (!n->renderer)
  {
    n->window = window();
    n->renderer = createRenderer();
    n->renderer->data = n;
    n->quickFbo = this;
    connect(window(), SIGNAL(beforeRendering()), n, SLOT(render()));
    connect(window(), SIGNAL(screenChanged(QScreen*)), n, SLOT(handleScreenChange()));
  }

   n->renderer->synchronize(this);

//   QSize minFboSize = d->sceneGraphContext()->minimumFBOSize();
   QSize minFboSize = QSize(1,1);
   QSize desiredFboSize(qMax<int>(minFboSize.width(), width()),
                        qMax<int>(minFboSize.height(), height()));

   n->devicePixelRatio = window()->effectiveDevicePixelRatio();
   desiredFboSize *= n->devicePixelRatio;

   if (n->fbo && ((this->textureFollowsItemSize() && n->fbo->size() != desiredFboSize) || n->invalidatePending)) {
       delete n->texture();
       delete n->fbo;
       n->fbo = nullptr;
       delete n->msDisplayFbo;
       n->msDisplayFbo = nullptr;
       n->invalidatePending = false;
   }

   if (!n->fbo) {
       n->fbo = n->renderer->createFramebufferObject(desiredFboSize);

       GLuint displayTexture = n->fbo->texture();

       if (n->fbo->format().samples() > 0) {
           n->msDisplayFbo = new QOpenGLFramebufferObject(n->fbo->size());
           displayTexture = n->msDisplayFbo->texture();
       }

       n->setTexture(window()->createTextureFromId(displayTexture,
                                                   n->fbo->size(),
                                                   QQuickWindow::TextureHasAlphaChannel));
   }

   n->setTextureCoordinatesTransform(d->mirrorVertically ? QSGSimpleTextureNode::MirrorVertically : QSGSimpleTextureNode::NoTransform);
   n->setFiltering(d->smooth ? QSGTexture::Linear : QSGTexture::Nearest);
   n->setRect(0, 0, width(), height());

   n->scheduleRender();

   return n;

*/


//  std::cerr << "node type: " << static_cast<int>(_node->type()) << std::endl;

/*  QSGSimpleTextureNode *n = dynamic_cast<QSGSimpleTextureNode *>(out);
  if (!n && (width() <= 0 || height() <= 0))
    return nullptr;

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
      n->markDirty(QSGNode::DirtyMaterial);
    }
    else
    {
      this->update();
    }
  }

  std::cerr << "update paint node done " << std::endl;
  return n;
*/


  QSGSimpleTextureNode *n = dynamic_cast<QSGSimpleTextureNode *>(out);
  if (!n && (width() <= 0 || height() <= 0))
    return nullptr;

  std::cerr << "update paint node r1 " << std::endl;

  if ( this->dataPtr->renderer)
  {

  std::cerr << "update paint node r2 " << std::endl;
    auto r = dynamic_cast<RenderWindowItemRenderer *>(this->dataPtr->renderer);
  std::cerr << "update paint node r3 " << r->GLId() << std::endl;
    if (r && r->GLId() > 0)
    {
      std::cerr << "creating render gl texture with id: " << r->GLId() << " " << r->Size().width() << " " << r->Size().height() <<  std::endl;
//      this->dataPtr->qimg.save("qimg.png");
//      std::cerr << " save qimg " << std::endl;
      //common::Image im;
      //im.SetFromData(r->Image()->Data<unsigned char>(), r->Image()->Width(), r->Image()->Height(), common::Image::RGB_INT8);
      //im.SavePNG("qblit.png");



      delete this->dataPtr->texture;
//      this->dataPtr->texture = window()->createTextureFromId(
//        r->GLId(), r->Size());

      this->dataPtr->texture = window()->createTextureFromImage(this->dataPtr->qimg);
//      std::cerr << "created texture " << this->dataPtr->texture <<  " vs " << n << std::endl;

      // this should cause the original texture to be deleted
      n->setTexture(this->dataPtr->texture);
      n->markDirty(QSGNode::DirtyMaterial);
    }
    else
    {
//      this->update();
    }
  }

  this->update();


//  return out;
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

