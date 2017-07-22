include_directories(${PROJECT_BINARY_DIR})

set (sources
  ${CMAKE_CURRENT_SOURCE_DIR}/Example.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Iface.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/ign.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/MainWindow.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Plugin.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/plugins/ImageDisplay.cc
  PARENT_SCOPE
)

set (gtest_sources
  Example_TEST.cc
  Iface_TEST.cc
  MainWindow_TEST.cc
)

if (HAVE_IGN_TOOLS)
  set (gtest_sources ${gtest_sources}
    ign_TEST.cc
  )
endif()

ign_build_tests(${gtest_sources})

add_subdirectory(cmd)
