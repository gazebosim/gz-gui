set (topic_echo_test_sources TopicEcho_TEST.cc)

# TopicEcho_TEST requires a screen, which is not available on the Apple CI
# system
if (NOT APPLE)
  list (REMOVE_ITEM topic_echo_test_sources TopicEcho_TEST.cc)
endif()

gz_gui_add_plugin(TopicEcho
  SOURCES
    TopicEcho.cc
  QT_HEADERS
    TopicEcho.hh
  TEST_SOURCES
    ${topic_echo_test_sources}
)
