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
    public: GLuint textureId = 0;
    public: GLuint newTextureId = 0;
    public: QSize size {0, 0};
    public: QSize newSize {0, 0};
    public: QMutex mutex;
    public: QSGTexture *texture = nullptr;
    public: QQuickWindow *window = nullptr;

  public: void CreateTexture(GLuint *_id, QSize _size) {
    delete this->texture;
    this->texture = nullptr;


#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    this->texture = QNativeInterface::QSGOpenGLTexture::fromNative(
      *_id,
      this->window,
      _size);
#else
  this->texture =
      this->window->createTextureFromNativeObject(
          QQuickWindow::NativeObjectTexture,
          static_cast<void*>(_id),
          0,
          _size);
#endif
    }

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
    this->dataPtr->surface = nullptr;
  }
}

/////////////////////////////////////////////////
TextureNodeRhiOpenGL::~TextureNodeRhiOpenGL()
{
  delete this->dataPtr->texture;
  this->dataPtr->texture = nullptr;
}

/////////////////////////////////////////////////
TextureNodeRhiOpenGL::TextureNodeRhiOpenGL(QQuickWindow *_window)
    : dataPtr(std::make_unique<TextureNodeRhiOpenGLPrivate>())
{
  this->dataPtr->window = _window;

  // Our texture node must have a texture, so use the default 0 texture.
  this->dataPtr->CreateTexture(
    &this->dataPtr->textureId, QSize(1, 1));
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

  if (this->dataPtr->newTextureId != 0)
  {
    this->dataPtr->CreateTexture(
      &this->dataPtr->newTextureId, this->dataPtr->newSize);
  }
}
}  // namespace gz::gui::plugins
