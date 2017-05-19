set (sources
  ${CMAKE_CURRENT_SOURCE_DIR}/Example.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Iface.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/ign.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/MainWindow.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/widgets/Time.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/interfaces/Publisher.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/interfaces/ign/IgnPublisher.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/plugins/TimePlugin.cc
  PARENT_SCOPE
)

set (gtest_sources
  Example_TEST.cc
  Iface_TEST.cc
  MainWindow_TEST.cc
)

ign_build_tests(${gtest_sources})

add_subdirectory(cmd)
#add_subdirectory(interfaces)
#add_subdirectory(plugins)
#add_subdirectory(widgets)
