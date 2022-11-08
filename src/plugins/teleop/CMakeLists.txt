set (teleop_test_sources Teleop_TEST.cc)

# Teleop_TEST requires a screen, which is not available on the Apple CI
# system
if (NOT APPLE)
  list (REMOVE_ITEM teleop_test_sources Teleop_TEST.cc)
endif()

gz_gui_add_plugin(Teleop
  SOURCES
    Teleop.cc
  QT_HEADERS
    Teleop.hh
  TEST_SOURCES
    ${teleop_test_sources}
)
