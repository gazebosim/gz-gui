ign_gui_add_plugin(Scene3D
  SOURCES
    Scene3D.cc
  QT_HEADERS
    Scene3D.hh
  PUBLIC_LINK_LIBS
    ignition-rendering${IGN_RENDERING_VER}::ignition-rendering${IGN_RENDERING_VER}
)

