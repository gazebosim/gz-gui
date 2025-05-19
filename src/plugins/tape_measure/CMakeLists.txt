gz_gui_add_plugin(TapeMeasure
  SOURCES TapeMeasure.cc
  QT_HEADERS TapeMeasure.hh
  PRIVATE_LINK_LIBS
    gz-rendering::gz-rendering
)
