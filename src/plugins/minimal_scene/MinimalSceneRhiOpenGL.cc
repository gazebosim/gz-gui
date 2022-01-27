/*
 * Copyright (C) 2021 Open Source Robotics Foundation
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
#include "MinimalScene.hh"

#include <ignition/common/Console.hh>
#include <ignition/rendering/Camera.hh>

#include <QMutex>
#include <QQuickWindow>
#include <QSGTexture>
#include <QSize>

#include <memory>

/////////////////////////////////////////////////
namespace ignition
{
namespace gui
{
namespace plugins
{
  class IgnCameraTextureRhiOpenGLPrivate
  {
    public: int textureId = 0;
  };

  class RenderThreadRhiOpenGLPrivate
  {
    public: IgnRenderer *renderer = nullptr;
    public: void *texturePtr = nullptr;
    public: QOffscreenSurface *surface = nullptr;
    public: QOpenGLContext *context = nullptr;
  };

  class TextureNodeRhiOpenGLPrivate
  {
    public: int textureId = 0;
    public: int newTextureId = 0;
    public: QSize size {0, 0};
    public: QSize newSize {0, 0};
    public: QMutex mutex;
    public: QSGTexture *texture = nullptr;
    public: QQuickWindow *window = nullptr;
  };
}
}
}

using namespace ignition;
using namespace gui;
using namespace plugins;

/////////////////////////////////////////////////
IgnCameraTextureRhiOpenGL::~IgnCameraTextureRhiOpenGL() = default;

/////////////////////////////////////////////////
IgnCameraTextureRhiOpenGL::IgnCameraTextureRhiOpenGL()
  : dataPtr(std::make_unique<IgnCameraTextureRhiOpenGLPrivate>())
{
}

/////////////////////////////////////////////////
void IgnCameraTextureRhiOpenGL::Update(rendering::CameraPtr _camera)
{
  this->dataPtr->textureId = _camera->RenderTextureGLId();
}

/////////////////////////////////////////////////
void IgnCameraTextureRhiOpenGL::TextureId(void* _texturePtr)
{
  *reinterpret_cast<void**>(_texturePtr) = (void*)&this->dataPtr->textureId; //NOLINT
}

/////////////////////////////////////////////////
/////////////////////////////////////////////////
RenderThreadRhiOpenGL::~RenderThreadRhiOpenGL() = default;

/////////////////////////////////////////////////
RenderThreadRhiOpenGL::RenderThreadRhiOpenGL(IgnRenderer *_renderer)
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
void RenderThreadRhiOpenGL::Initialize()
{
  this->dataPtr->context->makeCurrent(this->dataPtr->surface);

  this->dataPtr->renderer->Initialize();

  this->dataPtr->context->doneCurrent();
}

/////////////////////////////////////////////////
void RenderThreadRhiOpenGL::RenderNext(RenderSync *_renderSync)
{
  this->dataPtr->context->makeCurrent(this->dataPtr->surface);

  if (!this->dataPtr->renderer->initialized)
  {
    this->dataPtr->renderer->Initialize();
  }

  if (!this->dataPtr->renderer->initialized)
  {
    ignerr << "Unable to initialize renderer" << std::endl;
    return;
  }

  // Call the renderer
  this->dataPtr->renderer->Render(_renderSync);

  // Get reference to the rendered texture
  this->dataPtr->texturePtr = nullptr;
  this->dataPtr->renderer->TextureId(&this->dataPtr->texturePtr);

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

  this->dataPtr->context->doneCurrent();
  delete this->dataPtr->context;
  this->dataPtr->context = nullptr;

  // Schedule this to be deleted only after we're done cleaning up
  this->dataPtr->surface->deleteLater();
}

/////////////////////////////////////////////////
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
    void* _texturePtr /*[in]*/, const QSize &_size)
{
  this->dataPtr->mutex.lock();
  this->dataPtr->textureId = *static_cast<int*>(_texturePtr);
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

#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
# ifndef _WIN32
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wdeprecated-declarations"
# endif
    this->dataPtr->texture = this->dataPtr->window->createTextureFromId(
        this->dataPtr->newTextureId,
        this->dataPtr->newSize,
        QQuickWindow::TextureIsOpaque);
# ifndef _WIN32
#   pragma GCC diagnostic pop
# endif
#else
    this->dataPtr->texture =
        this->dataPtr->window->createTextureFromNativeObject(
            QQuickWindow::NativeObjectTexture,
            static_cast<void*>(&this->dataPtr->newTextureId),
            0,
            this->dataPtr->newSize);
#endif
  }
}
