# No need to add plugins to sources, they will be found when building the
# shared libraries

set (gtest_local_sources
  CameraTrack3D_TEST
  Grid3D_TEST
  ImageDisplay_TEST
  Publisher_TEST
  Requester_TEST
  Responder_TEST
  Scene3D_TEST
  TimePanel_TEST
  TopicEcho_TEST
  TopicInterface_TEST
  TopicsStats_TEST
  TopicViewer_TEST
)

foreach(src ${gtest_local_sources})
  set(gtest_local_cc ${gtest_local_cc} ${src}.cc)
endforeach()

ign_build_tests(TYPE UNIT
  SOURCES ${gtest_local_cc}
  LIB_DEPS ${IGNITION-RENDERING_LIBRARIES}
  INCLUDE_DIRS
    # Used to make internal source file headers visible to the unit tests
    ${CMAKE_CURRENT_SOURCE_DIR}
    # Used to make test-directory headers visible to the unit tests
    ${PROJECT_SOURCE_DIR}
    # Used to make test_config.h visible to the unit tests
    ${PROJECT_BINARY_DIR})

foreach(src ${gtest_local_sources})
  target_link_libraries(UNIT_${src} TINYXML2::TINYXML2)
endforeach()

add_subdirectory(plot)
