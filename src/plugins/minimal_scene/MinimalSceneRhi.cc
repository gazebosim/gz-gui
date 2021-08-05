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

#include "MinimalSceneRhi.hh"

using namespace ignition;
using namespace gui;
using namespace plugins;

/////////////////////////////////////////////////
IgnCameraTextureRhi::~IgnCameraTextureRhi() = default;

/////////////////////////////////////////////////
RenderThreadRhi::~RenderThreadRhi() = default;

/////////////////////////////////////////////////
QOffscreenSurface *RenderThreadRhi::Surface() const
{
  return reinterpret_cast<QOffscreenSurface *>(0);
}

/////////////////////////////////////////////////
void RenderThreadRhi::SetSurface(QOffscreenSurface *)
{
  /* no-op */
}

/////////////////////////////////////////////////
QOpenGLContext *RenderThreadRhi::Context() const
{
  return reinterpret_cast<QOpenGLContext *>(0);
}

/////////////////////////////////////////////////
void RenderThreadRhi::SetContext(QOpenGLContext *)
{
  /* no-op */
}

/////////////////////////////////////////////////
TextureNodeRhi::~TextureNodeRhi() = default;
