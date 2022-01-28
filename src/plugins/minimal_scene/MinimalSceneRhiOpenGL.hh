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

#ifndef IGNITION_GUI_PLUGINS_MINIMALSCENE_MINIMALSCENERHIOPENGL_HH_
#define IGNITION_GUI_PLUGINS_MINIMALSCENE_MINIMALSCENERHIOPENGL_HH_

#include "MinimalSceneRhi.hh"
#include "ignition/gui/Plugin.hh"

#include <QQuickWindow>
#include <QSGTexture>
#include <QSize>

#include <memory>

namespace ignition
{
namespace gui
{
namespace plugins
{
  /// \brief Private data for IgnCameraTextureRhiOpenGL
  class IgnCameraTextureRhiOpenGLPrivate;

  /// \brief Implementation of IgnCameraTextureRhi for the OpenGL graphics API
  class IgnCameraTextureRhiOpenGL : public IgnCameraTextureRhi
  {
    // Documentation inherited
    public: virtual ~IgnCameraTextureRhiOpenGL() override;

    /// \brief Constructor
    public: IgnCameraTextureRhiOpenGL();

    // Documentation inherited
    public: virtual void Update(rendering::CameraPtr _camera) override;

    // Documentation inherited
    public: virtual void TextureId(void* _texturePtr) override;

    /// \internal Pointer to private data
    private: std::unique_ptr<IgnCameraTextureRhiOpenGLPrivate> dataPtr;
  };

  /// \brief Private data for RenderThreadRhiOpenGLPrivate
  class RenderThreadRhiOpenGLPrivate;

  /// \brief Implementation of RenderThreadRhi for the OpenGL graphics API
  class RenderThreadRhiOpenGL : public RenderThreadRhi
  {
    // Documentation inherited
    public: virtual ~RenderThreadRhiOpenGL() override;

    /// \brief Constructor
    /// \param[in] _renderer The Ign-rendering renderer
    public: RenderThreadRhiOpenGL(IgnRenderer *_renderer);

    // Documentation inherited
    public: virtual QOffscreenSurface *Surface() const override;

    // Documentation inherited
    public: virtual void SetSurface(QOffscreenSurface *_surface) override;

    // Documentation inherited
    public: virtual QOpenGLContext *Context() const override;

    // Documentation inherited
    public: virtual void SetContext(QOpenGLContext *_context) override;

    // Documentation inherited
    public: virtual void Initialize() override;

    // Documentation inherited
    public: virtual void RenderNext(RenderSync *_renderSync) override;

    // Documentation inherited
    public: virtual void* TexturePtr() const override;

    // Documentation inherited
    public: virtual QSize TextureSize() const override;

    // Documentation inherited
    public: virtual void ShutDown() override;

    /// \internal Prevent copy and assignment
    private: RenderThreadRhiOpenGL(
        const RenderThreadRhiOpenGL &_other) = delete;
    private: RenderThreadRhiOpenGL& operator=(
        const RenderThreadRhiOpenGL &_other) = delete;

    /// \internal Pointer to private data
    private: std::unique_ptr<RenderThreadRhiOpenGLPrivate> dataPtr;
  };

  /// \brief Private data for TextureNodeRhiOpenGL
  class TextureNodeRhiOpenGLPrivate;

  /// \brief Implementation of TextureNodeRhi for the OpenGL graphics API
  class TextureNodeRhiOpenGL : public TextureNodeRhi
  {
    // Documentation inherited
    public: virtual ~TextureNodeRhiOpenGL() override;

    /// \brief Constructor
    /// \param[in] _window Window to display the texture
    public: TextureNodeRhiOpenGL(QQuickWindow *_window);

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
    private: TextureNodeRhiOpenGL(
        const TextureNodeRhiOpenGL &_other) = delete;
    private: TextureNodeRhiOpenGL& operator=(
        const TextureNodeRhiOpenGL &_other) = delete;

    /// \internal Pointer to private data
    private: std::unique_ptr<TextureNodeRhiOpenGLPrivate> dataPtr;
   };
}
}
}

#endif
