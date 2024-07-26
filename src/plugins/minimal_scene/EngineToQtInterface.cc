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

#include "EngineToQtInterface.hh"

#include <gz/common/Console.hh>
#include <gz/rendering/Camera.hh>
#include <gz/rendering/RenderEngine.hh>

#include <QOpenGLExtraFunctions>

// clang-format off
namespace gz
{
namespace gui
{
namespace plugins
{
  /// \brief Private data class for EngineToQtInterfacePrivate
  class EngineToQtInterfacePrivate
  {
    /// \brief FBO texture. Stores uploaded GPU -> CPU.
    /// Used only during fallback.
    public: GLuint fallbackTexture = 0;

    /// \brief Stores downloaded GPU -> CPU. Used only during fallback
    public: gz::rendering::ImagePtr fallbackImage;

    /// \brief Qt's OpenGL context. Used only during fallback.
    /// We need it to render using Qt's context.
    public: QOpenGLContext *glContext = nullptr;
  };
}
}
}
// clang-format on

using namespace gz;
using namespace gui;
using namespace plugins;

/////////////////////////////////////////////////
EngineToQtInterface::EngineToQtInterface(QOpenGLContext *_glContext) :
  dataPtr(std::make_unique<EngineToQtInterfacePrivate>())
{
  this->dataPtr->glContext = _glContext;
}

/////////////////////////////////////////////////
EngineToQtInterface::~EngineToQtInterface()
{
  this->DestroyFallbackTexture();
}

/////////////////////////////////////////////////
void EngineToQtInterface::CreateFallbackTexture()
{
  static bool bWarnedOnce = false;

  if (!bWarnedOnce)
  {
    gzwarn
      << "Using fallback method to render to Qt. Things will be SLOW.\n"
         "Try another API (e.g. OpenGL vs Vulkan) or build against a newer "
         "Qt version\n";
    bWarnedOnce = true;
  }

  this->DestroyFallbackTexture();

  GZ_ASSERT(this->dataPtr->fallbackTexture == 0, "Invalid State!");

  QOpenGLFunctions *glFuncs = this->dataPtr->glContext->functions();

  glFuncs->glGenTextures(1u, &this->dataPtr->fallbackTexture);

  glFuncs->glBindTexture(GL_TEXTURE_2D, this->dataPtr->fallbackTexture);
  glFuncs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
  glFuncs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
  glFuncs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glFuncs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFuncs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glFuncs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glFuncs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glFuncs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0u);

#ifndef __APPLE__
  this->dataPtr->glContext->extraFunctions()->glTexStorage2D(
    GL_TEXTURE_2D, 1u, GL_RGBA8, GLsizei(this->dataPtr->fallbackImage->Width()),
    GLsizei(this->dataPtr->fallbackImage->Height()));
#else
  glFuncs->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
                        GLsizei(this->dataPtr->fallbackImage->Width()),
                        GLsizei(this->dataPtr->fallbackImage->Height()), 0,
                        GL_RGBA, GL_BYTE, nullptr);
#endif
}

/////////////////////////////////////////////////
void EngineToQtInterface::DestroyFallbackTexture()
{
  QOpenGLFunctions *glFuncs = this->dataPtr->glContext->functions();
  glFuncs->glDeleteTextures(1, &this->dataPtr->fallbackTexture);
  this->dataPtr->fallbackTexture = 0;
}

/////////////////////////////////////////////////
bool EngineToQtInterface::NeedsFallback(gz::rendering::CameraPtr &_camera)
{
  auto *renderEngine = _camera->Scene()->Engine();
  if (renderEngine->GraphicsAPI() != gz::rendering::GraphicsAPI::OPENGL &&
      renderEngine->GraphicsAPI() != gz::rendering::GraphicsAPI::METAL)
  {
    return true;
  }

  return false;
}

/////////////////////////////////////////////////
GLuint EngineToQtInterface::TextureId(gz::rendering::CameraPtr &_camera)
{
  if (!this->NeedsFallback(_camera))
  {
    auto textureId = _camera->RenderTextureGLId();

    QOpenGLFunctions *glFuncs = this->dataPtr->glContext->functions();
    glFuncs->glBindTexture(GL_TEXTURE_2D, textureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SRGB_DECODE_EXT,
                    GL_SKIP_DECODE_EXT);
    return textureId;
  }
  else
  {
    if (!this->dataPtr->fallbackImage ||
        this->dataPtr->fallbackImage->Width() != _camera->ImageWidth() ||
        this->dataPtr->fallbackImage->Height() != _camera->ImageHeight())
    {
      this->dataPtr->fallbackImage =
        std::make_shared<gz::rendering::Image>(
          _camera->ImageWidth(), _camera->ImageHeight(),
          gz::rendering::PF_R8G8B8A8);

      this->CreateFallbackTexture();
    }

    _camera->Copy(*this->dataPtr->fallbackImage);

    QOpenGLFunctions *glFuncs = this->dataPtr->glContext->functions();

    // We don't care about buffering / performance so we perform a synchronous
    // glTexSubImage2D copy. This is a slow fallback path.
    // We don't want to make the code unnecessary complex.
    glFuncs->glBindTexture(GL_TEXTURE_2D, this->dataPtr->fallbackTexture);

    glFuncs->glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    // RGBA8888 is always naturally aligned to 4 bytes
    glFuncs->glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glFuncs->glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glFuncs->glPixelStorei(GL_UNPACK_IMAGE_HEIGHT, 0);

    glFuncs->glTexSubImage2D(
      GL_TEXTURE_2D, 0, 0, 0, GLsizei(this->dataPtr->fallbackImage->Width()),
      GLsizei(this->dataPtr->fallbackImage->Height()), GL_RGBA,
      GL_UNSIGNED_INT_8_8_8_8_REV, this->dataPtr->fallbackImage->Data());

    return this->dataPtr->fallbackTexture;
  }
}
