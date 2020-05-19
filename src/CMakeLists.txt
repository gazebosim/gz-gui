include_directories(${PROJECT_BINARY_DIR})

set (sources
  ${CMAKE_CURRENT_SOURCE_DIR}/Application.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Conversions.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Dialog.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/DragDropModel.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Helpers.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/ign.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/MainWindow.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Plugin.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/SearchModel.cc
  PARENT_SCOPE
)

set (gtest_sources
  Application_TEST
  Conversions_TEST
  DragDropModel_TEST
  Helpers_TEST
  ign_TEST
  MainWindow_TEST
  Plugin_TEST
  SearchModel_TEST
)

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
