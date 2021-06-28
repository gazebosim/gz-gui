ign_gui_add_plugin(CameraTracking
  SOURCES
    CameraTracking.cc
  QT_HEADERS
    CameraTracking.hh
  TEST_SOURCES
    # CameraTracking_TEST.cc
  PUBLIC_LINK_LIBS
   ignition-rendering${IGN_RENDERING_VER}::ignition-rendering${IGN_RENDERING_VER}
   ignition-common${IGN_COMMON_VER}::profiler
)
