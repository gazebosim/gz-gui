gz_gui_add_plugin(MarkerManager
  SOURCES
    MarkerManager.cc
  QT_HEADERS
    MarkerManager.hh
  PUBLIC_LINK_LIBS
   gz-rendering${GZ_RENDERING_VER}::gz-rendering${GZ_RENDERING_VER}
)
