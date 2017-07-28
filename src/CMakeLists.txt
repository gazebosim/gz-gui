include_directories(${PROJECT_BINARY_DIR})

set (sources
  ${CMAKE_CURRENT_SOURCE_DIR}/Iface.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/ign.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/MainWindow.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Plugin.cc
  PARENT_SCOPE
)

set (plugin_sources
  ${CMAKE_CURRENT_SOURCE_DIR}/plugins/ImageDisplay.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/plugins/Publisher.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/plugins/Requester.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/plugins/Responder.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/plugins/TimePanel.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/plugins/TopicEcho.cc
  PARENT_SCOPE
)

set (gtest_sources
  Iface_TEST.cc
  MainWindow_TEST.cc
)

if (HAVE_IGN_TOOLS)
  set (gtest_sources ${gtest_sources}
    ign_TEST.cc
  )
endif()

add_subdirectory(cmd)

ign_build_tests(${gtest_sources})
