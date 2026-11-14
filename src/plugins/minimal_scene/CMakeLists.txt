set(SOURCES
  MinimalScene.cc
  MinimalSceneRhi.cc
  MinimalSceneRhiOpenGL.cc
  MinimalSceneRhiVulkan.cc
  EngineToQtInterface.cc
)

set(PROJECT_LINK_LIBS "")

# Objective-C sources for macOS
if (APPLE)
  set(SOURCES
    ${SOURCES}
    MinimalSceneRhiMetal.mm
  )

  set(PROJECT_LINK_LIBS
    "-framework AppKit"
    "-framework Metal"
  )
endif()

# Native Wayland support (experimental, opt-in - see gz-sim's Gui.cc for the
# runtime opt-in gate). Only needs wayland-client to bind a throwaway
# wl_compositor/wl_surface; the wl_display itself comes from Qt6's public
# QNativeInterface::QWaylandApplication API, not from this dependency.
find_package(PkgConfig QUIET)
set(GZ_GUI_BUILD_HAVE_WAYLAND_CLIENT FALSE)
if (PkgConfig_FOUND AND UNIX AND NOT APPLE)
  pkg_check_modules(WAYLAND_CLIENT QUIET wayland-client)
  if (WAYLAND_CLIENT_FOUND)
    set(GZ_GUI_BUILD_HAVE_WAYLAND_CLIENT TRUE)
    set(SOURCES
      ${SOURCES}
      WaylandNativeSurface.cc
    )
  endif()
endif()

gz_gui_add_plugin(MinimalScene
  SOURCES
    ${SOURCES}
    EngineToQtInterface.hh
  QT_HEADERS
    MinimalScene.hh
  PUBLIC_LINK_LIBS
   gz-rendering::gz-rendering
   gz-transport::gz-transport
   ${PROJECT_LINK_LIBS}
)

if (GZ_GUI_BUILD_HAVE_WAYLAND_CLIENT)
  target_compile_definitions(MinimalScene
    PRIVATE GZ_GUI_BUILD_HAVE_WAYLAND_CLIENT=1)
  target_include_directories(MinimalScene
    PRIVATE ${WAYLAND_CLIENT_INCLUDE_DIRS})
  target_link_libraries(MinimalScene
    PRIVATE ${WAYLAND_CLIENT_LIBRARIES})
endif()

# Enable ARC on selected source files
if (APPLE)
  set_source_files_properties(
      MinimalSceneRhiMetal.mm
      PROPERTIES
      COMPILE_FLAGS
          "-fobjc-arc -Wno-nullability-completeness"
  )
endif()
