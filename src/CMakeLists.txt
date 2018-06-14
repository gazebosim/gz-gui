include_directories(${PROJECT_BINARY_DIR})

#ign_get_libsources_and_unittests(sources_no_path gtest_sources)

set (sources
#  ${CMAKE_CURRENT_SOURCE_DIR}/BoolWidget.cc
#  ${CMAKE_CURRENT_SOURCE_DIR}/CollapsibleWidget.cc
#  ${CMAKE_CURRENT_SOURCE_DIR}/ColorWidget.cc
#  ${CMAKE_CURRENT_SOURCE_DIR}/Conversions.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Dialog.cc
#  ${CMAKE_CURRENT_SOURCE_DIR}/DragDropModel.cc
#  ${CMAKE_CURRENT_SOURCE_DIR}/EditableLabel.cc
#  ${CMAKE_CURRENT_SOURCE_DIR}/EnumWidget.cc
#  ${CMAKE_CURRENT_SOURCE_DIR}/GeometryWidget.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Helpers.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Iface.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/ign.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/MainWindow.cc
#  ${CMAKE_CURRENT_SOURCE_DIR}/MessageWidget.cc
#  ${CMAKE_CURRENT_SOURCE_DIR}/NumberWidget.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Plugin.cc
#  ${CMAKE_CURRENT_SOURCE_DIR}/Pose3dWidget.cc
#  ${CMAKE_CURRENT_SOURCE_DIR}/PropertyWidget.cc
#  ${CMAKE_CURRENT_SOURCE_DIR}/SearchModel.cc
#  ${CMAKE_CURRENT_SOURCE_DIR}/StringWidget.cc
#  ${CMAKE_CURRENT_SOURCE_DIR}/VariablePill.cc
#  ${CMAKE_CURRENT_SOURCE_DIR}/VariablePillContainer.cc
#  ${CMAKE_CURRENT_SOURCE_DIR}/Vector3dWidget.cc
  PARENT_SCOPE
)

set (gtest_sources
#  BoolWidget_TEST
#  ColorWidget_TEST
#  CollapsibleWidget_TEST
#  Conversions_TEST
#  DragDropModel_TEST
#  EditableLabel_TEST
#  EnumWidget_TEST
#  GeometryWidget_TEST
  Helpers_TEST
  Iface_TEST
  ign_TEST
  MainWindow_TEST
#  MessageWidget_TEST
#  NumberWidget_TEST
  Plugin_TEST
#  Pose3dWidget_TEST
#  SearchModel_TEST
#  StringWidget_TEST
#  VariablePillContainer_TEST
#  Vector3dWidget_TEST
)

#set(sources)
#foreach(src ${sources_no_path})
#  list(APPEND sources ${CMAKE_CURRENT_SOURCE_DIR}/${src})
#endforeach()
#set(sources ${sources} PARENT_SCOPE)

# this test currently fails on brew (issue #27)
# so disable it for now
if (NOT HAVE_IGN_TOOLS OR APPLE)
  list(REMOVE_ITEM gtest_sources ign_TEST.cc)
endif()

ign_build_tests(TYPE UNIT
                SOURCES
                  ${gtest_sources}
                LIB_DEPS
                  ${IGNITION-MATH_LIBRARIES}
                  TINYXML2::TINYXML2
)

add_subdirectory(cmd)
add_subdirectory(plugins)
