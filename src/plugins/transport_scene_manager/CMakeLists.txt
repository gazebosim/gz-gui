gz_gui_add_plugin(TransportSceneManager
  SOURCES
    TransportSceneManager.cc
  QT_HEADERS
    TransportSceneManager.hh
  TEST_SOURCES
    # TransportSceneManager_TEST.cc
  PUBLIC_LINK_LIBS
   ignition-rendering${IGN_RENDERING_VER}::ignition-rendering${IGN_RENDERING_VER}
)

