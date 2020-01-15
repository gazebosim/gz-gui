# No need to add plugins to sources, they will be found when building the
# shared libraries

# Tests
set (gtest_local_sources
  WorldControl
  WorldStats
)

foreach(src ${gtest_local_sources})
  ign_build_tests(TYPE UNIT
    SOURCES
      ${src}_TEST.cc
    LIB_DEPS
      ${IGNITION-RENDERING_LIBRARIES}
      ${IGNITION-GUI_LIBRARIES}
      TINYXML2::TINYXML2
      ${src}
    INCLUDE_DIRS
      # Used to make internal source file headers visible to the unit tests
      ${CMAKE_CURRENT_SOURCE_DIR}
      # Used to make test-directory headers visible to the unit tests
      ${PROJECT_SOURCE_DIR}
      # Used to make test_config.h visible to the unit tests
      ${PROJECT_BINARY_DIR})
endforeach()

#add_subdirectory(plot)
