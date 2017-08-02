# No need to add plugins to sources, they will be found when building the
# shared libraries

set (gtest_local_sources
  ImageDisplay_TEST.cc
  Publisher_TEST.cc
  Requester_TEST.cc
  Responder_TEST.cc
  TimePanel_TEST.cc
  TopicEcho_TEST.cc
)

ign_build_tests(${gtest_local_sources})
