gz_gui_add_plugin(Scene3D
  SOURCES
    Scene3D.cc
  QT_HEADERS
    Scene3D.hh
  PUBLIC_LINK_LIBS
    gz-rendering${GZ_RENDERING_VER}::gz-rendering${GZ_RENDERING_VER}
)

