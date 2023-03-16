ign_gui_add_plugin(CameraFps
  SOURCES
    CameraFps.cc
  QT_HEADERS
    CameraFps.hh
  PUBLIC_LINK_LIBS
   ignition-rendering${IGN_RENDERING_VER}::ignition-rendering${IGN_RENDERING_VER}
)

