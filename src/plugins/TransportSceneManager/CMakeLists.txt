gz_gui_add_plugin(TransportSceneManager
  SOURCES
    TransportSceneManager.cc
  QT_HEADERS
    TransportSceneManager.hh
  TEST_SOURCES
    # TransportSceneManager_TEST.cc
  PUBLIC_LINK_LIBS
    gz-rendering${GZ_RENDERING_VER}::gz-rendering${GZ_RENDERING_VER}
  QML_URI
    TransportSceneManager
)
