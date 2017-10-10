include_directories(${PROJECT_BINARY_DIR})

set (sources
  ${CMAKE_CURRENT_SOURCE_DIR}/BoolWidget.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/CollapsibleWidget.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/ColorWidget.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Conversions.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/DragDropModel.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Helpers.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Iface.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/ign.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/MainWindow.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/MessageWidget.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/NumberWidget.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Plugin.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Pose3dWidget.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/PropertyWidget.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/SearchModel.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/StringWidget.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Vector3dWidget.cc
  PARENT_SCOPE
)

set (gtest_sources
  BoolWidget_TEST.cc
  ColorWidget_TEST.cc
  CollapsibleWidget_TEST.cc
  Conversions_TEST.cc
  DragDropModel_TEST.cc
  Helpers_TEST.cc
  Iface_TEST.cc
  MainWindow_TEST.cc
  MessageWidget_TEST.cc
  NumberWidget_TEST.cc
  Pose3dWidget_TEST.cc
  SearchModel_TEST.cc
  StringWidget_TEST.cc
  Vector3dWidget_TEST.cc
)

if (HAVE_IGN_TOOLS)
  set (gtest_sources ${gtest_sources}
    ign_TEST.cc
  )
endif()

ign_build_tests(${gtest_sources})

add_subdirectory(cmd)
add_subdirectory(plugins)
