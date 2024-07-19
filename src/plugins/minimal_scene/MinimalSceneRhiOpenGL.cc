/*
 * Copyright (C) 2022 Open Source Robotics Foundation
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

#include "MinimalSceneRhiOpenGL.hh"

#include "EngineToQtInterface.hh"
#include "MinimalScene.hh"

#include <gz/common/Console.hh>
#include <gz/rendering/Camera.hh>

#include <QMutex>
#include <QQuickWindow>
#include <QSGTexture>
#include <QSize>

#include <memory>
#include <string>

/////////////////////////////////////////////////
namespace gz::gui::plugins
{
  class GzCameraTextureRhiOpenGLPrivate
  {
    public: int textureId = 0;
  };

  class RenderThreadRhiOpenGLPrivate
  {
    public: GzRenderer *renderer = nullptr;
    public: void *texturePtr = nullptr;
    public: QOffscreenSurface *surface = nullptr;
    public: QOpenGLContext *context = nullptr;

    /// \brief For fallback support if we can't render directly to Qt API
    public: std::unique_ptr<EngineToQtInterface> engineToQtInterface;
  };

  class TextureNodeRhiOpenGLPrivate
  {
    // Current texture id from the render engine
    public: GLuint textureId = 0;

    // New texture id from the render engine
    public: GLuint newTextureId = 0;

    // Intermediate FBO for copying texture data
    public: GLuint rhiFbo  = 0;

    // Texture id for internal texture that holds texture data from
    // the render engine texture. The texture data is in GL_RGB format
    public: GLuint rhiTextureId = 0;

    public: QSize size {0, 0};
    public: QSize newSize {0, 0};
    public: QMutex mutex;
    public: QSGTexture *texture = nullptr;
    public: QQuickWindow *window = nullptr;
  };

/////////////////////////////////////////////////
GzCameraTextureRhiOpenGL::~GzCameraTextureRhiOpenGL() = default;

/////////////////////////////////////////////////
GzCameraTextureRhiOpenGL::GzCameraTextureRhiOpenGL()
  : dataPtr(std::make_unique<GzCameraTextureRhiOpenGLPrivate>())
{
}

/////////////////////////////////////////////////
void GzCameraTextureRhiOpenGL::Update(rendering::CameraPtr _camera)
{
  this->dataPtr->textureId = _camera->RenderTextureGLId();
}

/////////////////////////////////////////////////
/////////////////////////////////////////////////
RenderThreadRhiOpenGL::~RenderThreadRhiOpenGL() = default;

/////////////////////////////////////////////////
RenderThreadRhiOpenGL::RenderThreadRhiOpenGL(GzRenderer *_renderer)
    : dataPtr(std::make_unique<RenderThreadRhiOpenGLPrivate>())
{
  this->dataPtr->renderer = _renderer;
}

/////////////////////////////////////////////////
QOffscreenSurface *RenderThreadRhiOpenGL::Surface() const
{
  return this->dataPtr->surface;
}

/////////////////////////////////////////////////
void RenderThreadRhiOpenGL::SetSurface(QOffscreenSurface *_surface)
{
  this->dataPtr->surface = _surface;
}

/////////////////////////////////////////////////
QOpenGLContext *RenderThreadRhiOpenGL::Context() const
{
  return this->dataPtr->context;
}

/////////////////////////////////////////////////
void RenderThreadRhiOpenGL::SetContext(QOpenGLContext *_context)
{
  this->dataPtr->context = _context;
}

/////////////////////////////////////////////////
std::string RenderThreadRhiOpenGL::Initialize()
{
  this->dataPtr->context->makeCurrent(this->dataPtr->surface);

  this->dataPtr->engineToQtInterface.reset(
    new EngineToQtInterface(this->dataPtr->context));

  auto loadingError = this->dataPtr->renderer->Initialize(*this);
  if (!loadingError.empty())
  {
    return loadingError;
  }

  this->dataPtr->context->doneCurrent();
  return std::string();
}

/////////////////////////////////////////////////
void RenderThreadRhiOpenGL::Update(rendering::CameraPtr _camera)
{
  const GLuint glId = this->dataPtr->engineToQtInterface->TextureId(_camera);
  this->dataPtr->texturePtr = reinterpret_cast<void *>(
    static_cast<intptr_t>(glId));
}

/////////////////////////////////////////////////
void RenderThreadRhiOpenGL::RenderNext(RenderSync *_renderSync)
{
  this->dataPtr->context->makeCurrent(this->dataPtr->surface);

  if (!this->dataPtr->renderer->initialized)
  {
    this->Initialize();

    if (!this->dataPtr->renderer->initialized)
    {
      gzerr << "Unable to initialize renderer" << std::endl;
      return;
    }
  }

  // Call the renderer
  this->dataPtr->renderer->Render(_renderSync, *this);

  this->dataPtr->context->doneCurrent();
}

/////////////////////////////////////////////////
void* RenderThreadRhiOpenGL::TexturePtr() const
{
  return this->dataPtr->texturePtr;
}

/////////////////////////////////////////////////
QSize RenderThreadRhiOpenGL::TextureSize() const
{
  return this->dataPtr->renderer->textureSize;
}

/////////////////////////////////////////////////
void RenderThreadRhiOpenGL::ShutDown()
{
  this->dataPtr->renderer->Destroy();

  this->dataPtr->texturePtr = nullptr;

  if (this->dataPtr->context)
  {
    this->dataPtr->context->doneCurrent();
    delete this->dataPtr->context;
    this->dataPtr->context = nullptr;
  }

  // Schedule this to be deleted only after we're done cleaning up
  if (this->dataPtr->surface)
  {
    this->dataPtr->surface->deleteLater();
  }
}

/////////////////////////////////////////////////
TextureNodeRhiOpenGL::~TextureNodeRhiOpenGL()
{
  // clean up opengl resources owned by this rhi
  QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
  if (this->dataPtr->rhiFbo)
    f->glDeleteFramebuffers(1, &this->dataPtr->rhiFbo);
  if (this->dataPtr->rhiTextureId)
    f->glDeleteTextures(1, &this->dataPtr->rhiTextureId);

  delete this->dataPtr->texture;
  this->dataPtr->texture = nullptr;
}

/////////////////////////////////////////////////
TextureNodeRhiOpenGL::TextureNodeRhiOpenGL(QQuickWindow *_window)
    : dataPtr(std::make_unique<TextureNodeRhiOpenGLPrivate>())
{
  this->dataPtr->window = _window;

  // Our texture node must have a texture, so use the default 0 texture.
#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
# ifndef _WIN32
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wdeprecated-declarations"
# endif
  this->dataPtr->texture = this->dataPtr->window->createTextureFromId(
      this->dataPtr->textureId,
      QSize(1, 1),
      QQuickWindow::TextureIsOpaque);
# ifndef _WIN32
#   pragma GCC diagnostic pop
# endif
#else
  this->dataPtr->texture =
      this->dataPtr->window->createTextureFromNativeObject(
          QQuickWindow::NativeObjectTexture,
          static_cast<void*>(&this->dataPtr->textureId),
          0,
          QSize(1, 1));
#endif
}

/////////////////////////////////////////////////
QSGTexture *TextureNodeRhiOpenGL::Texture() const
{
  return this->dataPtr->texture;
}

/////////////////////////////////////////////////
bool TextureNodeRhiOpenGL::HasNewTexture() const
{
  return (this->dataPtr->newTextureId != 0);
}

/////////////////////////////////////////////////
void TextureNodeRhiOpenGL::NewTexture(
    void* _texturePtr, const QSize &_size)
{
  this->dataPtr->mutex.lock();
  this->dataPtr->textureId =
    static_cast<GLuint>(reinterpret_cast<size_t>(_texturePtr));
  this->dataPtr->size = _size;
  this->dataPtr->mutex.unlock();
}

/////////////////////////////////////////////////
void TextureNodeRhiOpenGL::PrepareNode()
{
  this->dataPtr->mutex.lock();
  this->dataPtr->newTextureId = this->dataPtr->textureId;
  this->dataPtr->newSize = this->dataPtr->size;
  this->dataPtr->textureId = 0;
  this->dataPtr->mutex.unlock();

  if (this->dataPtr->newTextureId)
  {
    delete this->dataPtr->texture;
    this->dataPtr->texture = nullptr;

    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    // get the currently bound fbo so we can restore it later.
    GLint currentFbo;
    f->glGetIntegerv(GL_FRAMEBUFFER_BINDING, &currentFbo);

    // bind the texture from the rendering engine to our own RHI fbo
    if (!this->dataPtr->rhiFbo)
      f->glGenFramebuffers(1, &this->dataPtr->rhiFbo);
    f->glBindFramebuffer(GL_FRAMEBUFFER, this->dataPtr->rhiFbo);
    f->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D, this->dataPtr->newTextureId, 0);

    // copy the source render engine texture data to our own internal texture
    // The copied texture is the one we will pass to Qt
    if (!this->dataPtr->rhiTextureId)
      f->glGenTextures(1, &this->dataPtr->rhiTextureId);
    f->glBindTexture(GL_TEXTURE_2D, this->dataPtr->rhiTextureId);
    f->glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0,
        this->dataPtr->newSize.width(), this->dataPtr->newSize.height(), 0);

#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
# ifndef _WIN32
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wdeprecated-declarations"
# endif
    this->dataPtr->texture = this->dataPtr->window->createTextureFromId(
        this->dataPtr->rhiTextureId,
        this->dataPtr->newSize,
        QQuickWindow::TextureIsOpaque);
# ifndef _WIN32
#   pragma GCC diagnostic pop
# endif
#else
    this->dataPtr->texture =
        this->dataPtr->window->createTextureFromNativeObject(
            QQuickWindow::NativeObjectTexture,
            static_cast<void*>(&this->dataPtr->rhiTextureId),
            0,
            this->dataPtr->newSize);

     // unbind the render engine texture from RHI fbo and rebind the previously bound fbo.
     f->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
         GL_TEXTURE_2D, 0, 0);
     f->glBindTexture(GL_TEXTURE_2D, 0);
     f->glBindFramebuffer(GL_FRAMEBUFFER, (GLuint) currentFbo);
#endif
  }
}
}  // namespace gz::gui::plugins
