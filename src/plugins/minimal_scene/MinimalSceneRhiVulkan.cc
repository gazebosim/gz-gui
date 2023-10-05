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

#include "MinimalSceneRhiVulkan.hh"

#if HAVE_QT_VULKAN

#include "EngineToQtInterface.hh"
#include "MinimalScene.hh"

#include <gz/common/Console.hh>
#include <gz/rendering/Camera.hh>

#include <QMutex>
#include <QQuickWindow>
#include <QSGTexture>
#include <QSize>

#include <vulkan/vulkan_core.h>

#include <memory>
#include <string>

/////////////////////////////////////////////////
namespace gz::gui::plugins
{
class GzCameraTextureRhiVulkanPrivate
{
  public: VkImage textureId = nullptr;
};

class RenderThreadRhiVulkanPrivate
{
  public: GzRenderer *renderer = nullptr;
  public: void *texturePtr = nullptr;
  public: QOffscreenSurface *surface = nullptr;
};

class TextureNodeRhiVulkanPrivate
{
  public: VkImage textureId = nullptr;
  public: VkImage newTextureId = nullptr;
  public: std::weak_ptr<rendering::Camera> lastCamera;
  public: QSize size {0, 0};
  public: QSize newSize {0, 0};
  public: QMutex mutex;
  public: QSGTexture *texture = nullptr;
  public: QQuickWindow *window = nullptr;
};

/////////////////////////////////////////////////
GzCameraTextureRhiVulkan::~GzCameraTextureRhiVulkan() = default;

/////////////////////////////////////////////////
GzCameraTextureRhiVulkan::GzCameraTextureRhiVulkan()
  : dataPtr(std::make_unique<GzCameraTextureRhiVulkanPrivate>())
{
}

/////////////////////////////////////////////////
void GzCameraTextureRhiVulkan::Update(rendering::CameraPtr _camera)
{
  // It says Metal but it also works for Vulkan in the exact same way
  _camera->RenderTextureMetalId(&this->dataPtr->textureId);
}

/////////////////////////////////////////////////
/////////////////////////////////////////////////
RenderThreadRhiVulkan::~RenderThreadRhiVulkan() = default;

/////////////////////////////////////////////////
RenderThreadRhiVulkan::RenderThreadRhiVulkan(GzRenderer *_renderer)
    : dataPtr(std::make_unique<RenderThreadRhiVulkanPrivate>())
{
  this->dataPtr->renderer = _renderer;
}

/////////////////////////////////////////////////
QOffscreenSurface *RenderThreadRhiVulkan::Surface() const
{
  return this->dataPtr->surface;
}

/////////////////////////////////////////////////
void RenderThreadRhiVulkan::SetSurface(QOffscreenSurface *_surface)
{
  this->dataPtr->surface = _surface;
}

/////////////////////////////////////////////////
std::string RenderThreadRhiVulkan::Initialize()
{
  auto loadingError = this->dataPtr->renderer->Initialize(*this);
  if (!loadingError.empty())
  {
    return loadingError;
  }

  return {};
}

/////////////////////////////////////////////////
void RenderThreadRhiVulkan::Update(rendering::CameraPtr _camera)
{
  // It says Metal but it also works for Vulkan in the exact same way
  _camera->RenderTextureMetalId(&this->dataPtr->texturePtr);
}

/////////////////////////////////////////////////
void RenderThreadRhiVulkan::RenderNext(RenderSync *_renderSync)
{
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
}

/////////////////////////////////////////////////
void* RenderThreadRhiVulkan::TexturePtr() const
{
  return this->dataPtr->texturePtr;
}

/////////////////////////////////////////////////
QSize RenderThreadRhiVulkan::TextureSize() const
{
  return this->dataPtr->renderer->textureSize;
}

/////////////////////////////////////////////////
void RenderThreadRhiVulkan::ShutDown()
{
  this->dataPtr->renderer->Destroy();

  this->dataPtr->texturePtr = nullptr;

  // Schedule this to be deleted only after we're done cleaning up
  if (this->dataPtr->surface != nullptr)
  {
    this->dataPtr->surface->deleteLater();
  }
}

/////////////////////////////////////////////////
TextureNodeRhiVulkan::~TextureNodeRhiVulkan()
{
  delete this->dataPtr->texture;
  this->dataPtr->texture = nullptr;
}

/////////////////////////////////////////////////
TextureNodeRhiVulkan::TextureNodeRhiVulkan(QQuickWindow *_window,
                                           rendering::CameraPtr &_camera) :
  dataPtr(std::make_unique<TextureNodeRhiVulkanPrivate>())
{
  this->dataPtr->window = _window;

  // It says Metal but it also works for Vulkan in the exact same way
  _camera->RenderTextureMetalId(&this->dataPtr->textureId);
  this->dataPtr->lastCamera = _camera;

  this->dataPtr->texture = this->dataPtr->window->createTextureFromNativeObject(
    QQuickWindow::NativeObjectTexture,
    static_cast<void *>(&this->dataPtr->textureId),  //
    0,                                               //
    QSize(static_cast<int>(_camera->ImageWidth()),
          static_cast<int>(_camera->ImageHeight())));
}

/////////////////////////////////////////////////
QSGTexture *TextureNodeRhiVulkan::Texture() const
{
  return this->dataPtr->texture;
}

/////////////////////////////////////////////////
bool TextureNodeRhiVulkan::HasNewTexture() const
{
  return (this->dataPtr->newTextureId != nullptr);
}

/////////////////////////////////////////////////
void TextureNodeRhiVulkan::NewTexture(
    void* _texturePtr, const QSize &_size)
{
  this->dataPtr->mutex.lock();
  this->dataPtr->textureId = reinterpret_cast<VkImage>(_texturePtr);
  this->dataPtr->size = _size;
  this->dataPtr->mutex.unlock();
}

/////////////////////////////////////////////////
void TextureNodeRhiVulkan::PrepareNode()
{
  this->dataPtr->mutex.lock();
  this->dataPtr->newTextureId = this->dataPtr->textureId;
  this->dataPtr->newSize = this->dataPtr->size;
  this->dataPtr->textureId = nullptr;
  this->dataPtr->mutex.unlock();

  // Required: PrepareForExternalSampling ensures the texture is ready to
  // be sampled by Qt. Otherwise Qt could attempt to sample the texture
  // while the GPU is still drawing to it, or the caches aren't flushed, etc.
  auto lastCamera = this->dataPtr->lastCamera.lock();
  lastCamera->PrepareForExternalSampling();

  if (this->dataPtr->newTextureId != nullptr)
  {
    delete this->dataPtr->texture;
    this->dataPtr->texture = nullptr;

    this->dataPtr->texture =
        this->dataPtr->window->createTextureFromNativeObject(
            QQuickWindow::NativeObjectTexture,
            static_cast<void*>(&this->dataPtr->newTextureId),
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            this->dataPtr->newSize);
  }
}
}  // namespace gz::gui::plugins
#endif  // HAVE_QT_VULKAN
