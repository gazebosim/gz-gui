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
# runtime opt-in gate). Only needs wayland-client to create a throwaway
# wl_surface; the wl_display and wl_compositor come from Qt's public
# QNativeInterface::QWaylandApplication API.
find_package(PkgConfig QUIET)
if (PkgConfig_FOUND AND UNIX AND NOT APPLE)
  pkg_check_modules(WAYLAND_CLIENT QUIET IMPORTED_TARGET wayland-client)
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

if (WAYLAND_CLIENT_FOUND)
  message(STATUS "MinimalScene: native Wayland support enabled")
  target_compile_definitions(MinimalScene
    PRIVATE GZ_GUI_BUILD_HAVE_WAYLAND_CLIENT=1)
  target_link_libraries(MinimalScene PRIVATE PkgConfig::WAYLAND_CLIENT)
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
