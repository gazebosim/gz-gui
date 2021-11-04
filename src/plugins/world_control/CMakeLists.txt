set(test_sources)

if (NOT MSVC)
  set(test_sources WorldControl_TEST.cc)
endif()

ign_gui_add_plugin(WorldControl
  SOURCES
    WorldControl.cc
    WorldControlEventListener.cc
  QT_HEADERS
    WorldControl.hh
    WorldControlEventListener.hh
  TEST_SOURCES ${test_sources}
)
