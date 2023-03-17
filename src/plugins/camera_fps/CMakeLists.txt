gz_gui_add_plugin(CameraFps
  SOURCES
    CameraFps.cc
  QT_HEADERS
    CameraFps.hh
  PUBLIC_LINK_LIBS
   gz-rendering${GZ_RENDERING_VER}::gz-rendering${GZ_RENDERING_VER}
)

