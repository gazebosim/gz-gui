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

#ifndef GZ_GUI_PLUGINS_MINIMALSCENE_MINIMALSCENERHI_HH_
#define GZ_GUI_PLUGINS_MINIMALSCENE_MINIMALSCENERHI_HH_

#include <string>

#include "gz/gui/Plugin.hh"
#include "gz/rendering/RenderTypes.hh"

#include <QSGTexture>
#include <QSize>

namespace gz::gui::plugins
{
/// \brief Render interface class to handle OpenGL / Metal compatibility
/// of camera textures in GzRenderer
//
/// Each supported graphics API must implement this interface
/// to provide access to the underlying render system's texture.
///
/// TODO(anyone): This class doesn't seem to be doing anything at all
/// Anyone should try to remove it, and if nothing bad happens
/// (on Ubuntu & macOS) it should be deprecated and removed
class GzCameraTextureRhi
{
  /// \brief Destructor
  public: virtual ~GzCameraTextureRhi();

  /// \brief Update the texture for a camera
  /// \param[in] _camera Pointer to the camera providing the texture
  public: virtual void Update(rendering::CameraPtr _camera) = 0;
};

/// \brief gz-rendering renderer.
class GzRenderer;
class RenderSync;

/// \brief Render interface class to handle OpenGL / Metal compatibility
/// in RenderThread
class RenderThreadRhi
{
  /// \brief Destructor
  public: virtual ~RenderThreadRhi();

  /// \brief Offscreen surface to render to
  public: virtual QOffscreenSurface *Surface() const;

  /// \brief Set the offscreen surface to render to
  //
  /// \param[in] _surface Off-screen surface format
  public: virtual void SetSurface(QOffscreenSurface *_surface);

  /// \brief OpenGL context to be passed to the render engine
  public: virtual QOpenGLContext *Context() const;

  /// \brief Set the OpenGL context to be passed to the render engine
  //
  /// \param[in] _context OpenGL context
  public: virtual void SetContext(QOpenGLContext *_context);

  /// \brief Carry out initialization
  //
  /// On macOS this must be run on the main thread
  /// \return Error message if initialization failed. If empty, no errors
  /// occurred.
  public: virtual std::string Initialize() = 0;

  /// \brief Render when safe
  /// \param[in] _renderSync RenderSync to safely
  /// synchronize Qt and worker thread (this)
  public: virtual void RenderNext(RenderSync *_renderSync) = 0;

  /// \brief Update the texture for a camera
  /// \param[in] _camera Pointer to the camera providing the texture
  public: virtual void Update(rendering::CameraPtr _camera) = 0;

  /// \brief Return a pointer to the graphics API texture Id
  public: virtual void* TexturePtr() const = 0;

  /// \brief Return the size of the texture
  public: virtual QSize TextureSize() const = 0;

  /// \brief Shutdown the thread and the render engine
  public: virtual void ShutDown() = 0;
};

/// \brief Render interface class to handle OpenGL / Metal compatibility
/// in TextureNode
class TextureNodeRhi
{
  /// \brief Destructor
  public: virtual ~TextureNodeRhi();

  /// \brief Get the Qt scene graph texture
  public: virtual QSGTexture *Texture() const = 0;

  /// \brief Return true if a new texture has been received
  /// from the render thread
  public: virtual bool HasNewTexture() const = 0;

  /// \brief This function gets called on the render thread and will
  ///  store the texture id and size and schedule an update on the window.
  /// \param[in] _texturePtr Pointer to a texture Id
  /// \param[in] _size Texture size
  public: virtual void NewTexture(
      void* _texturePtr, const QSize &_size) = 0;

  /// \brief Before the scene graph starts to render, we update to the
  /// pending texture
  public: virtual void PrepareNode() = 0;
};
}  // namespace gz::gui::plugins
#endif  // GZ_GUI_PLUGINS_MINIMALSCENE_MINIMALSCENERHI_HH_
