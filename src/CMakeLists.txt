include_directories(${PROJECT_BINARY_DIR})

ign_get_libsources_and_unittests(sources_no_path gtest_sources)

set(sources)
foreach(src ${sources_no_path})
  list(APPEND sources ${CMAKE_CURRENT_SOURCE_DIR}/${src})
endforeach()
set(sources ${sources} PARENT_SCOPE)

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
add_subdirectory(display_plugins)
add_subdirectory(plugins)
