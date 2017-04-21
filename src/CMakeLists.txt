set (sources
  ${CMAKE_CURRENT_SOURCE_DIR}/Example.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Iface.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/ign.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/MainWindow.cc
  PARENT_SCOPE
)

set (gtest_sources
  Example_TEST.cc
  Iface_TEST.cc
  MainWindow_TEST.cc
)

ign_build_tests(${gtest_sources})

add_subdirectory(cmd)
