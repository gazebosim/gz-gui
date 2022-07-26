gz_gui_add_plugin(InteractiveViewControl
  SOURCES
    InteractiveViewControl.cc
  QT_HEADERS
    InteractiveViewControl.hh
  TEST_SOURCES
    # ViewControl_TEST.cc
  PUBLIC_LINK_LIBS
   gz-rendering${GZ_RENDERING_VER}::gz-rendering${GZ_RENDERING_VER}
)
