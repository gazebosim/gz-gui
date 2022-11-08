set (screenshot_test_sources Screenshot_TEST.cc)

# Screenshot_TEST requires a screen, which is not available on the Apple  CI
# system
if (NOT APPLE)
  list (REMOVE_ITEM screenshot_test_sources Screenshot_TEST.cc)
endif()

gz_gui_add_plugin(Screenshot
  SOURCES
    Screenshot.cc
  QT_HEADERS
    Screenshot.hh
  PUBLIC_LINK_LIBS
    gz-rendering${GZ_RENDERING_VER}::gz-rendering${GZ_RENDERING_VER}
  TEST_SOURCES
    ${screenshot_test_sources}
)
