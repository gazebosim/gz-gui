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

#ifndef GZ_GUI_PLUGINS_MINIMALSCENE_WAYLANDNATIVESURFACE_HH_
#define GZ_GUI_PLUGINS_MINIMALSCENE_WAYLANDNATIVESURFACE_HH_

#include <memory>

// Opaque forward declarations so this header doesn't require
// <wayland-client.h> (and thus GZ_GUI_HAVE_WAYLAND) to be visible to
// consumers that merely hold a pointer to this class.
struct wl_display;
struct wl_surface;
struct wl_compositor;
struct wl_registry;

namespace gz::gui::plugins
{
  /// \brief Creates and owns a small, throwaway Wayland surface used solely
  /// to give OGRE-Next's native Wayland EGL backend something to bootstrap
  /// an EGL context/window against.
  ///
  /// This is NOT the surface Gazebo's 3D scene is actually displayed on -
  /// gz-rendering's Ogre2 backend renders to an offscreen FBO texture that
  /// is handed directly to Qt Quick's scenegraph (see
  /// Ogre2Camera::RenderTextureGLId()); the "primary" Ogre render window
  /// this surface backs is a 1x1 bootstrap window that OGRE-Next's own
  /// render loop never swaps (verified: every swapBuffers call in OgreMain
  /// is workspace-driven, and no compositor workspace is ever attached to
  /// the primary window). Because of that, this surface deliberately has no
  /// Wayland "role" (no xdg_toplevel) - it is never mapped or shown.
  ///
  /// Uses only Qt6's PUBLIC QNativeInterface::QWaylandApplication API for
  /// the wl_display (the caller obtains and passes that in); this class
  /// itself only needs libwayland-client to bind its own wl_compositor and
  /// create the surface, avoiding any dependency on Qt's private/unstable
  /// QtWaylandClient headers (which would otherwise be needed to extract a
  /// specific QWindow's real wl_surface).
  class WaylandNativeSurface
  {
    /// \brief Constructor.
    /// \param[in] _display A live wl_display connection (not owned - the
    /// caller, e.g. Qt's Wayland QPA plugin, must keep it alive and must
    /// continue dispatching it; this class never dispatches it itself).
    /// Throws std::runtime_error if a wl_compositor can't be bound or the
    /// surface can't be created.
    public: explicit WaylandNativeSurface(wl_display *_display);

    /// \brief Destructor. Destroys the surface and the compositor binding.
    /// Does not touch _display.
    public: ~WaylandNativeSurface();

    /// \brief Copying/moving would complicate ownership of the registry
    /// listener's `this` pointer; not needed, so disabled.
    public: WaylandNativeSurface(const WaylandNativeSurface &) = delete;
    public: WaylandNativeSurface &operator=(
        const WaylandNativeSurface &) = delete;

    /// \brief Returns the created surface.
    public: wl_surface *Surface() const;

    /// \brief Private implementation.
    private: class Implementation;
    private: std::unique_ptr<Implementation> dataPtr;
  };
}  // namespace gz::gui::plugins

#endif  // GZ_GUI_PLUGINS_MINIMALSCENE_WAYLANDNATIVESURFACE_HH_
