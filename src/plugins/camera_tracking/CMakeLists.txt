gz_gui_add_plugin(CameraTracking
  SOURCES
    CameraTracking.cc
  QT_HEADERS
    CameraTracking.hh
  TEST_SOURCES
    # CameraTracking_TEST.cc
  PUBLIC_LINK_LIBS
   gz-rendering::gz-rendering
   gz-common::profiler
)
