include_directories(${PROJECT_BINARY_DIR})

set (sources
  ${CMAKE_CURRENT_SOURCE_DIR}/BoolWidget.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/CollapsibleWidget.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/ColorWidget.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Conversions.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/DragDropModel.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/EditableLabel.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/EnumWidget.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/GeometryWidget.cc
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
  ${CMAKE_CURRENT_SOURCE_DIR}/VariablePill.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/VariablePillContainer.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Vector3dWidget.cc
  PARENT_SCOPE
)

#QT5_WRAP_CPP(header_varPill_MOC VariablePillContainer_TEST.hh)

set (gtest_sources
  BoolWidget_TEST.cc
  ColorWidget_TEST.cc
  CollapsibleWidget_TEST.cc
  Conversions_TEST.cc
  DragDropModel_TEST.cc
  EditableLabel_TEST.cc
  EnumWidget_TEST.cc
  GeometryWidget_TEST.cc
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

set(GTEST_SOURCE_file VariablePillContainer_TEST.cc)
string(REGEX REPLACE ".cc" "" BINARY_NAME ${GTEST_SOURCE_file})
set(BINARY_NAME ${TEST_TYPE}_${BINARY_NAME})
if(USE_LOW_MEMORY_TESTS)
  add_definitions(-DUSE_LOW_MEMORY_TESTS=1)
endif(USE_LOW_MEMORY_TESTS)
add_executable(${BINARY_NAME} ${GTEST_SOURCE_file} ${header_varPill_MOC})

add_dependencies(${BINARY_NAME}
  ${PROJECT_NAME_LOWER}${PROJECT_MAJOR_VERSION}
  gtest gtest_main
)

if (UNIX)
  target_link_libraries(${BINARY_NAME}
     libgtest_main.a
     libgtest.a
     pthread
     ${PROJECT_NAME_LOWER}${PROJECT_MAJOR_VERSION}
     ${header_varPill_MOC}
  )
elseif(WIN32)
  target_link_libraries(${BINARY_NAME}
     gtest.lib
     gtest_main.lib
     ${PROJECT_NAME_LOWER}${PROJECT_MAJOR_VERSION}.lib)
else()
   message(FATAL_ERROR "Unsupported platform")
endif()

add_test(${BINARY_NAME} ${CMAKE_CURRENT_BINARY_DIR}/${BINARY_NAME}
--gtest_output=xml:${CMAKE_BINARY_DIR}/test_results/${BINARY_NAME}.xml)

set_tests_properties(${BINARY_NAME} PROPERTIES TIMEOUT 240)

if(PYTHONINTERP_FOUND)
  # Check that the test produced a result and create a failure if it didn't.
  # Guards against crashed and timed out tests.
  add_test(check_${BINARY_NAME} ${PYTHON_EXECUTABLE} ${PROJECT_SOURCE_DIR}/tools/check_test_ran.py
    ${CMAKE_BINARY_DIR}/test_results/${BINARY_NAME}.xml)
endif()

add_subdirectory(cmd)
add_subdirectory(plugins)
