ign_get_sources(tests)

ign_build_tests(
  TYPE INTEGRATION
  SOURCES ${tests}
  LIB_DEPS
    ${PROJECT_NAME}_test_helpers
    ignition-plugin${IGN_PLUGIN_VER}::loader
    ignition-rendering${IGN_RENDERING_VER}::ignition-rendering${IGN_RENDERING_VER})
