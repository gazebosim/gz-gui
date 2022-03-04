set(SOURCES
  MinimalScene.cc
  MinimalSceneRhi.cc
  MinimalSceneRhiOpenGL.cc
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

ign_gui_add_plugin(MinimalScene
  SOURCES
    ${SOURCES}
  QT_HEADERS
    MinimalScene.hh
  PUBLIC_LINK_LIBS
   ignition-rendering${IGN_RENDERING_VER}::ignition-rendering${IGN_RENDERING_VER}
   ${PROJECT_LINK_LIBS}
)

# Enable ARC on selected source files
if (APPLE)
  set_source_files_properties(
      MinimalSceneRhiMetal.mm
      PROPERTIES
      COMPILE_FLAGS
          "-fobjc-arc"
  )
endif()
