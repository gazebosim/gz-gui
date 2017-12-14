# No need to add plugins to sources, they will be found when building the
# shared libraries

set (gtest_local_sources
  CameraTrack3D_TEST.cc
  Grid3D_TEST.cc
  ImageDisplay_TEST.cc
  Publisher_TEST.cc
  Requester_TEST.cc
  Responder_TEST.cc
  Scene3D_TEST.cc
  TimePanel_TEST.cc
  TopicEcho_TEST.cc
  TopicInterface_TEST.cc
  TopicsStats_TEST.cc
  TopicViewer_TEST.cc
)

ign_build_tests(SOURCES ${gtest_local_sources}
                LIB_DEPS ${IGNITION-RENDERING_LIBRARIES})

add_subdirectory(plot)
