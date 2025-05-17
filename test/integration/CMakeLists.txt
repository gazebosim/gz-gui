gz_get_sources(tests)

gz_build_tests(
  TYPE INTEGRATION
  SOURCES ${tests}
  LIB_DEPS
    ${PROJECT_NAME}_test_helpers
    gz-plugin::loader
    gz-rendering::gz-rendering
    Qt::Test
  ENVIRONMENT
    GZ_GUI_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
)
