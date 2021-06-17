ign_gui_add_plugin(MinimalScene
  SOURCES
    MinimalScene.cc
  QT_HEADERS
    MinimalScene.hh
  TEST_SOURCES
    # MinimalScene_TEST.cc
  PUBLIC_LINK_LIBS
   ignition-rendering${IGN_RENDERING_VER}::ignition-rendering${IGN_RENDERING_VER}
)

