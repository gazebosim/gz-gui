# No need to add plugins to sources, they will be found when building the
# shared libraries

set (gtest_local_sources
  ImageDisplay_TEST.cc
  Publisher_TEST.cc
  Requester_TEST.cc
  Responder_TEST.cc
  TimePanel_TEST.cc
  TopicEcho_TEST.cc
  TopicInterface_TEST.cc
  TopicsStats_TEST.cc
  TopicViewer_TEST.cc
)

ign_build_tests(SOURCES ${gtest_local_sources})

if (HAVE_IGN_RENDERING)
  set (gtest_rendering_sources
    Scene3D_TEST.cc
  )
endif()

ign_build_tests(SOURCES ${gtest_rendering_sources}
                LIB_DEPS ${IGNITION-RENDERING_LIBRARIES})

add_subdirectory(plot)
