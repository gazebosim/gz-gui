include_directories(${PROJECT_BINARY_DIR})

set (sources
  ${CMAKE_CURRENT_SOURCE_DIR}/Application.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Conversions.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Dialog.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/DragDropModel.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/GuiEvents.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Helpers.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/gz.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/MainWindow.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/PlottingInterface.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Plugin.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/SearchModel.cc
  PARENT_SCOPE
)

set (gtest_sources
  Application_TEST.cc
  Conversions_TEST.cc
  DragDropModel_TEST.cc
  Helpers_TEST.cc
  GuiEvents_TEST.cc
  gz_TEST.cc
  MainWindow_TEST.cc
  PlottingInterface_TEST.cc
  Plugin_TEST.cc
  SearchModel_TEST.cc
)

if (MSVC)
  # Warning #4251 is the "dll-interface" warning that tells you when types used
  # by a class are not being exported. These generated source files have private
  # members that don't get exported, so they trigger this warning. However, the
  # warning is not important since those members do not need to be interfaced
  # with.
  set_source_files_properties(${sources} ${gtest_sources} COMPILE_FLAGS "/wd4251 /wd4146")
endif()

# this test currently fails on brew (issue #27)
# so disable it for now
if (NOT HAVE_GZ_TOOLS OR APPLE)
  list(REMOVE_ITEM gtest_sources gz_TEST.cc)
endif()

gz_build_tests(TYPE UNIT
                SOURCES
                  ${gtest_sources}
                LIB_DEPS
                  gz-common${GZ_COMMON_VER}::events
                  gz-math${GZ_MATH_VER}::gz-math${GZ_MATH_VER}
                  TINYXML2::TINYXML2
)

add_subdirectory(cmd)
add_subdirectory(plugins)
