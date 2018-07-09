# No need to add plugins to sources, they will be found when building the
# shared libraries

set (gtest_local_sources
#  Grid3D
#  ImageDisplay
#  Publisher
#  Requester
#  Responder
#  Scene3D
#  TopicEcho
#  TopicInterface
#  TopicsStats
#  TopicViewer
  WorldControl
  WorldStats
)

foreach(src ${gtest_local_sources})
  set(gtest_local_cc ${gtest_local_cc} ${src}_TEST.cc)
  set(gtest_local_libs ${gtest_local_libs}
      ${PROJECT_BINARY_DIR}/lib/lib${src}.so)
endforeach()

ign_build_tests(TYPE UNIT
  SOURCES ${gtest_local_cc}
  LIB_DEPS
    ${IGNITION-RENDERING_LIBRARIES}
    TINYXML2::TINYXML2
    ${gtest_local_sources}
  INCLUDE_DIRS
    # Used to make internal source file headers visible to the unit tests
    ${CMAKE_CURRENT_SOURCE_DIR}
    # Used to make test-directory headers visible to the unit tests
    ${PROJECT_SOURCE_DIR}
    # Used to make test_config.h visible to the unit tests
    ${PROJECT_BINARY_DIR})

#add_subdirectory(plot)
