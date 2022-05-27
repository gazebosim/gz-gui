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

#include "MinimalSceneRhiMetal.hh"
#include "MinimalScene.hh"

#include <gz/common/Console.hh>
#include <gz/rendering/Camera.hh>

#include <QMutex>
#include <QQuickWindow>
#include <QSGTexture>
#include <QSize>

#include <memory>

#include <Metal/Metal.h>

#if ! __has_feature(objc_arc)
#error "ARC is off"
#endif

/////////////////////////////////////////////////
namespace gz
{
namespace gui
{
namespace plugins
{
  class IgnCameraTextureRhiMetalPrivate
  {
    public: id<MTLTexture> metalTexture = nil;
  };

  class RenderThreadRhiMetalPrivate
  {
    public: IgnRenderer *renderer = nullptr;
    public: void *texturePtr = nullptr;
  };

  class TextureNodeRhiMetalPrivate
  {
    public: id<MTLTexture> metalTexture = nil;
    public: id<MTLTexture> newMetalTexture = nil;
    public: QSize size {0, 0};
    public: QSize newSize {0, 0};
    public: QMutex mutex;
    public: QSGTexture *texture = nullptr;
    public: QQuickWindow *window = nullptr;
  };
}
}
}

using namespace gz;
using namespace gui;
using namespace plugins;

/////////////////////////////////////////////////
IgnCameraTextureRhiMetal::~IgnCameraTextureRhiMetal() = default;

/////////////////////////////////////////////////
IgnCameraTextureRhiMetal::IgnCameraTextureRhiMetal()
  : dataPtr(std::make_unique<IgnCameraTextureRhiMetalPrivate>())
{
}

/////////////////////////////////////////////////
void IgnCameraTextureRhiMetal::Update(rendering::CameraPtr _camera)
{
  void *texturePtr = nullptr;
  _camera->RenderTextureMetalId(&texturePtr);
  this->dataPtr->metalTexture = CFBridgingRelease(texturePtr);
}

/////////////////////////////////////////////////
void IgnCameraTextureRhiMetal::TextureId(void* _texturePtr)
{
  *static_cast<void**>(_texturePtr) =
      (void*)CFBridgingRetain(this->dataPtr->metalTexture);
}

/////////////////////////////////////////////////
/////////////////////////////////////////////////
RenderThreadRhiMetal::~RenderThreadRhiMetal() = default;

/////////////////////////////////////////////////
RenderThreadRhiMetal::RenderThreadRhiMetal(IgnRenderer *_renderer)
    : dataPtr(std::make_unique<RenderThreadRhiMetalPrivate>())
{
  this->dataPtr->renderer = _renderer;
}

/////////////////////////////////////////////////
std::string RenderThreadRhiMetal::Initialize()
{
  return this->dataPtr->renderer->Initialize();
}

/////////////////////////////////////////////////
void RenderThreadRhiMetal::RenderNext(RenderSync *_renderSync)
{
  if (!this->dataPtr->renderer->initialized)
  {
    this->dataPtr->renderer->Initialize();
  }

  // Check if engine has been successfully initialized
  if (!this->dataPtr->renderer->initialized)
  {
    gzerr << "Unable to initialize renderer" << std::endl;
    return;
  }

  // Call the renderer
  this->dataPtr->renderer->Render(_renderSync);

  // Get reference to the rendered texture
  this->dataPtr->texturePtr = nullptr;
  this->dataPtr->renderer->TextureId(&this->dataPtr->texturePtr);
}

/////////////////////////////////////////////////
void* RenderThreadRhiMetal::TexturePtr() const
{
  return this->dataPtr->texturePtr;
}

/////////////////////////////////////////////////
QSize RenderThreadRhiMetal::TextureSize() const
{
  return this->dataPtr->renderer->textureSize;
}

/////////////////////////////////////////////////
void RenderThreadRhiMetal::ShutDown()
{
  this->dataPtr->renderer->Destroy();

  this->dataPtr->texturePtr = nullptr;
}

/////////////////////////////////////////////////
/////////////////////////////////////////////////
TextureNodeRhiMetal::~TextureNodeRhiMetal()
{
  delete this->dataPtr->texture;
  this->dataPtr->texture = nullptr;
}

/////////////////////////////////////////////////
TextureNodeRhiMetal::TextureNodeRhiMetal(QQuickWindow *_window)
    : dataPtr(std::make_unique<TextureNodeRhiMetalPrivate>())
{
  this->dataPtr->window = _window;

  // Our texture node must have a texture, so use the default 0 texture.
  this->dataPtr->texture =
      this->dataPtr->window->createTextureFromNativeObject(
        QQuickWindow::NativeObjectTexture,
        static_cast<void*>(&this->dataPtr->metalTexture),
        0,
        QSize(1, 1));
}

/////////////////////////////////////////////////
QSGTexture *TextureNodeRhiMetal::Texture() const
{
  return this->dataPtr->texture;
}

/////////////////////////////////////////////////
bool TextureNodeRhiMetal::HasNewTexture() const
{
  return (this->dataPtr->newMetalTexture != nil);
}

/////////////////////////////////////////////////
void TextureNodeRhiMetal::NewTexture(
    void* _texturePtr, const QSize &_size)
{
  this->dataPtr->mutex.lock();
  this->dataPtr->metalTexture = CFBridgingRelease(_texturePtr);
  this->dataPtr->size = _size;
  this->dataPtr->mutex.unlock();
}

/////////////////////////////////////////////////
void TextureNodeRhiMetal::PrepareNode()
{
  this->dataPtr->mutex.lock();
  this->dataPtr->newMetalTexture = this->dataPtr->metalTexture;
  this->dataPtr->newSize = this->dataPtr->size;
  this->dataPtr->metalTexture = nil;
  this->dataPtr->mutex.unlock();

  if (this->dataPtr->newMetalTexture)
  {
    delete this->dataPtr->texture;
    this->dataPtr->texture = nullptr;

    this->dataPtr->texture =
        this->dataPtr->window->createTextureFromNativeObject(
            QQuickWindow::NativeObjectTexture,
            static_cast<void*>(&this->dataPtr->newMetalTexture),
            0,
            this->dataPtr->newSize);
  }
}
