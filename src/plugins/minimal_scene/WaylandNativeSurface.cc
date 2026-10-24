/*
 * Copyright (C) 2026 Open Source Robotics Foundation
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

#include "WaylandNativeSurface.hh"

#include <cstring>
#include <stdexcept>

#include <wayland-client.h>

namespace gz::gui::plugins
{
class WaylandNativeSurface::Implementation
{
  public: wl_display *display{nullptr};
  public: wl_registry *registry{nullptr};
  public: wl_compositor *compositor{nullptr};
  public: wl_surface *surface{nullptr};

  public: static void RegistryGlobal(void *_data, wl_registry *_registry,
      uint32_t _name, const char *_interface, uint32_t)
  {
    auto *impl = static_cast<Implementation *>(_data);
    if (std::strcmp(_interface, "wl_compositor") == 0)
    {
      impl->compositor = static_cast<wl_compositor *>(wl_registry_bind(
          _registry, _name, &wl_compositor_interface, 4));
    }
  }

  public: static void RegistryGlobalRemove(void *, wl_registry *, uint32_t)
  {
  }
};

/////////////////////////////////////////////////
WaylandNativeSurface::WaylandNativeSurface(wl_display *_display)
  : dataPtr(std::make_unique<Implementation>())
{
  this->dataPtr->display = _display;

  static const wl_registry_listener kRegistryListener = {
    &Implementation::RegistryGlobal,
    &Implementation::RegistryGlobalRemove
  };

  this->dataPtr->registry = wl_display_get_registry(this->dataPtr->display);
  wl_registry_add_listener(
      this->dataPtr->registry, &kRegistryListener, this->dataPtr.get());

  // One-shot, synchronous roundtrip to let the registry_global events for
  // already-advertised globals (including wl_compositor) arrive. This is
  // safe to do here (at startup, before any Ogre swap activity) even
  // though this class never dispatches the display afterwards.
  wl_display_roundtrip(this->dataPtr->display);

  if (!this->dataPtr->compositor)
  {
    throw std::runtime_error(
        "WaylandNativeSurface: wl_compositor was not advertised by the "
        "compositor");
  }

  this->dataPtr->surface =
      wl_compositor_create_surface(this->dataPtr->compositor);
  if (!this->dataPtr->surface)
  {
    throw std::runtime_error(
        "WaylandNativeSurface: wl_compositor_create_surface failed");
  }
}

/////////////////////////////////////////////////
WaylandNativeSurface::~WaylandNativeSurface()
{
  if (this->dataPtr->surface)
    wl_surface_destroy(this->dataPtr->surface);
  if (this->dataPtr->compositor)
    wl_compositor_destroy(this->dataPtr->compositor);
  if (this->dataPtr->registry)
    wl_registry_destroy(this->dataPtr->registry);
  // this->dataPtr->display is not owned - never destroyed/disconnected here.
}

/////////////////////////////////////////////////
wl_surface *WaylandNativeSurface::Surface() const
{
  return this->dataPtr->surface;
}
}  // namespace gz::gui::plugins
