ign_gui_add_plugin(ViewControl
  SOURCES
    ViewControl.cc
  QT_HEADERS
    ViewControl.hh
  TEST_SOURCES
    # ViewControl_TEST.cc
  PUBLIC_LINK_LIBS
   ignition-rendering${IGN_RENDERING_VER}::ignition-rendering${IGN_RENDERING_VER}
)
