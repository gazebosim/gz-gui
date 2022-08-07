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

#ifndef GZ_GUI_PLUGINS_ENGINETOQTINTERFACE_HH_
#define GZ_GUI_PLUGINS_ENGINETOQTINTERFACE_HH_

#include <gz/rendering/RenderTypes.hh>

#include "gz/gui/qt.h"

#include <memory>

namespace gz
{
namespace gui
{
namespace plugins
{
  class EngineToQtInterfacePrivate;

  /// \brief
  ///
  /// This class is in charge of delivering data to Qt in a format
  /// it can understand.
  ///
  /// Ideally, this is a simple call. e.g. if Qt wants an OpenGL
  /// texture handle, and we're rendering to a texture; we simply
  /// give Qt the texture handle (i.e. that's what
  /// gz::rendering::Camera::RenderTextureGLId does)
  ///
  /// If Qt wants an OpenGL texture handle, and we're rendering to Metal,
  /// ask Metal to convert it to an OpenGL handle and give it that.
  ///
  /// However there are cases where this isn's so straightforward, either
  /// because the API doesn't have an interface for converting handles
  /// between rendering systems, the underlying engine (i.e. Ogre) hasn't
  /// yet implemented it, or simply because there's a lot of driver bugs
  /// around it.
  ///
  /// Or perhaps the Qt version being compiled against is too old and doesn't
  /// have support (e.g. Qt is too old to have a Vulkan renderer mode or it's
  /// buggy)
  ///
  /// For all these cases, we use a fallback method where we download the
  /// data from GPU into CPU and then upload it again to GPU using Qt's OpenGL.
  ///
  /// This is slow, but it's almost certain guaranteed to work.
  /// A warning will be issued once when fallback is used, as it has
  /// an important performance impact.
  class EngineToQtInterface
  {
    /// \brief Constructor
    public: explicit EngineToQtInterface(QOpenGLContext *_glContext);

    /// \brief Destructor
    public: ~EngineToQtInterface();

    /// \brief Creates the fallback OpenGL texture we will be
    /// giving to Qt
    private: void CreateFallbackTexture();

    /// \brief Destroys the texture created by CreateFallbackTexture
    private: void DestroyFallbackTexture();

    /// \brief
    /// Returns the texture handle that Qt wants, whether it's the
    /// fallback or native one
    /// \param[in] _camera The camera doing rendering that should be shown
    /// on Qt
    /// \return Texture handle for Qt
    public: GLuint TextureId(gz::rendering::CameraPtr &_camera);

    /// \brief
    /// \param[in] _camera Camera that wants to be rendered
    /// \return False if memory is kept in the GPU and TextureId returns
    /// an API object. True if we must fallback to downloading data
    /// from GPU -> CPU and then uploading again CPU -> GPU using
    /// a different API (very slow)
    public: bool NeedsFallback(gz::rendering::CameraPtr &_camera);

    /// \internal
    /// \brief Pointer to private data.
    private: std::unique_ptr<EngineToQtInterfacePrivate> dataPtr;
  };
}
}
}

#endif
