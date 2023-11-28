/*
 * Copyright (C) 2017 Open Source Robotics Foundation
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

#include <QtGlobal>
#include <qtgui-config.h>

#define MINIMAL_SCENE_HAVE_OPENGL true

#define MINIMAL_SCENE_HAVE_VULKAN \
  QT_VERSION >= QT_VERSION_CHECK(5, 15, 2) && QT_CONFIG(vulkan)

#if defined(__APPLE__)
#define MINIMAL_SCENE_HAVE_METAL 1
#else
#define MINIMAL_SCENE_HAVE_METAL 0
#endif

constexpr bool kMinimalSceneHaveOpenGl = MINIMAL_SCENE_HAVE_OPENGL;
constexpr bool kMinimalSceneHaveVulkan = MINIMAL_SCENE_HAVE_VULKAN;
constexpr bool kMinimalSceneHaveMETAL = MINIMAL_SCENE_HAVE_METAL;
