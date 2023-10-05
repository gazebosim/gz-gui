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

#ifndef GZ_GUI_PLUGINS_MINIMALSCENE_MINIMALSCENERHIVULKAN_HH_
#define GZ_GUI_PLUGINS_MINIMALSCENE_MINIMALSCENERHIVULKAN_HH_

#include "MinimalSceneRhi.hh"
#include "gz/gui/Plugin.hh"

#include <QQuickWindow>
#include <QSGTexture>
#include <QSize>

#include <memory>
#include <string>

#define HAVE_QT_VULKAN \
  QT_VERSION >= QT_VERSION_CHECK(5, 15, 2) && QT_CONFIG(vulkan)

#if HAVE_QT_VULKAN
namespace gz::gui::plugins
{
/// \brief Private data for GzCameraTextureRhiVulkan
class GzCameraTextureRhiVulkanPrivate;

/// \brief Implementation of GzCameraTextureRhi for the Vulkan graphics API
class GzCameraTextureRhiVulkan : public GzCameraTextureRhi
{
  // Documentation inherited
  public: virtual ~GzCameraTextureRhiVulkan() override;

  /// \brief Constructor
  public: GzCameraTextureRhiVulkan();

  // Documentation inherited
  public: virtual void Update(rendering::CameraPtr _camera) override;

  /// \internal Pointer to private data
  private: std::unique_ptr<GzCameraTextureRhiVulkanPrivate> dataPtr;
};

/// \brief Private data for RenderThreadRhiVulkanPrivate
class RenderThreadRhiVulkanPrivate;

/// \brief Implementation of RenderThreadRhi for the Vulkan graphics API
class RenderThreadRhiVulkan : public RenderThreadRhi
{
  // Documentation inherited
  public: virtual ~RenderThreadRhiVulkan() override;

  /// \brief Constructor
  /// \param[in] _renderer The gz-rendering renderer
  public: explicit RenderThreadRhiVulkan(GzRenderer *_renderer);

  // Documentation inherited
  public: virtual QOffscreenSurface *Surface() const override;

  // Documentation inherited
  public: virtual void SetSurface(QOffscreenSurface *_surface) override;

  // Documentation inherited
  public: virtual std::string Initialize() override;

  // Documentation inherited
  public: virtual void Update(rendering::CameraPtr _camera) override;

  // Documentation inherited
  public: virtual void RenderNext(RenderSync *_renderSync) override;

  // Documentation inherited
  public: virtual void* TexturePtr() const override;

  // Documentation inherited
  public: virtual QSize TextureSize() const override;

  // Documentation inherited
  public: virtual void ShutDown() override;

  /// \internal Prevent copy and assignment
  private: RenderThreadRhiVulkan(
      const RenderThreadRhiVulkan &_other) = delete;
  private: RenderThreadRhiVulkan& operator=(
      const RenderThreadRhiVulkan &_other) = delete;

  /// \internal Pointer to private data
  private: std::unique_ptr<RenderThreadRhiVulkanPrivate> dataPtr;
};

/// \brief Private data for TextureNodeRhiVulkan
class TextureNodeRhiVulkanPrivate;

/// \brief Implementation of TextureNodeRhi for the Vulkan graphics API
class TextureNodeRhiVulkan : public TextureNodeRhi
{
  // Documentation inherited
  public: virtual ~TextureNodeRhiVulkan() override;

  /// \brief Constructor
  /// \param[in] _window Window to display the texture
  /// \param[in] _camera Camera owning the API texture handle
  public: explicit TextureNodeRhiVulkan(QQuickWindow *_window,
                                        rendering::CameraPtr &_camera);

  // Documentation inherited
  public: virtual QSGTexture *Texture() const override;

  // Documentation inherited
  public: virtual bool HasNewTexture() const override;

  // Documentation inherited
  public: virtual void NewTexture(
      void* _texturePtr, const QSize &_size) override;

  // Documentation inherited
  public: virtual void PrepareNode() override;

  /// \internal Prevent copy and assignment
  private: TextureNodeRhiVulkan(
      const TextureNodeRhiVulkan &_other) = delete;
  private: TextureNodeRhiVulkan& operator=(
      const TextureNodeRhiVulkan &_other) = delete;

  /// \internal Pointer to private data
  private: std::unique_ptr<TextureNodeRhiVulkanPrivate> dataPtr;
};
}  // namespace gz::gui::plugins
#endif  // HAVE_QT_VULKAN
#endif  // GZ_GUI_PLUGINS_MINIMALSCENE_MINIMALSCENERHIVULKAN_HH_
