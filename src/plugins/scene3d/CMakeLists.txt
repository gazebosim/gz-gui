ign_gui_add_plugin(Scene3D
  SOURCES
    Scene3D.cc
  QT_HEADERS
    Scene3D.hh
  TEST_SOURCES
    # Scene3D_TEST.cc
  PUBLIC_LINK_LIBS
   ${IGNITION-RENDERING_LIBRARIES}
)

