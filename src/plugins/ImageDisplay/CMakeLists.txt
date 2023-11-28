gz_gui_add_plugin(ImageDisplay
  SOURCES
    ImageDisplay.cc
  QT_HEADERS
    ImageDisplay.hh
  QML_SOURCES
    ImageDisplay.qml
  QML_URI
    ImageDisplay
  TEST_SOURCES
    ImageDisplay_TEST.cc
  PUBLIC_LINK_LIBS
   gz-common${GZ_COMMON_VER}::graphics
)

