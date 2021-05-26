ign_gui_add_plugin(CameraControllerManager
  SOURCES
    CameraControllerManager.cc
  QT_HEADERS
    CameraControllerManager.hh
  TEST_SOURCES
    # CameraControllerManager_TEST.cc
  PUBLIC_LINK_LIBS
   ignition-rendering${IGN_RENDERING_VER}::ignition-rendering${IGN_RENDERING_VER}
   ignition-common${IGN_COMMON_VER}::profiler
)
