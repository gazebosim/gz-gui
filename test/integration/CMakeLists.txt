gz_get_sources(tests)

find_package(Qt5Test REQUIRED)

gz_build_tests(
  TYPE INTEGRATION
  SOURCES ${tests}
  LIB_DEPS
    ${PROJECT_NAME}_test_helpers
    gz-plugin${GZ_PLUGIN_VER}::loader
    gz-rendering${GZ_RENDERING_VER}::gz-rendering${GZ_RENDERING_VER}
    Qt5::Test
  ENVIRONMENT 
    GZ_GUI_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
)
