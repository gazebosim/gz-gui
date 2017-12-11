#################################################
# ign_build_tests(SOURCES <sources>
#                 [LIB_DEPS <library_dependencies>])
#
# Build tests for an ignition project. Arguments are as follows:
#
# SOURCES: Required. The names (without the path) of the source files for your
#          tests. Each file will turn into a test.
#
# LIB_DEPS: Optional. Additional library dependencies that every test should
#           link to, not including the library built by this project (it will be
#           linked automatically). gtest and gtest_main will also be linked.
#
macro (ign_build_tests)
  #------------------------------------
  # Define the expected arguments
  set(options)
  set(oneValueArgs)
  set(multiValueArgs SOURCES LIB_DEPS)

  #------------------------------------
  # Parse the arguments
  cmake_parse_arguments(ign_build_tests "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  if(NOT DEFINED ign_build_tests_SOURCES)
    message(STATUS "No tests have been specified")
  else()
    list(LENGTH ign_build_tests_SOURCES num_tests)
    message(STATUS "Adding ${num_tests} tests")
  endif()

  # Find the Python interpreter for running the
  # check_test_ran.py script
  find_package(PythonInterp QUIET)

  # Build all the tests
  foreach(GTEST_SOURCE_file ${ign_build_tests_SOURCES})
    string(REGEX REPLACE ".cc" "" BINARY_NAME ${GTEST_SOURCE_file})
    set(BINARY_NAME ${TEST_TYPE}_${BINARY_NAME})
    if(USE_LOW_MEMORY_TESTS)
      add_definitions(-DUSE_LOW_MEMORY_TESTS=1)
    endif(USE_LOW_MEMORY_TESTS)
    add_executable(${BINARY_NAME} ${GTEST_SOURCE_file})

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
         ${ign_build_tests_LIB_DEPS}
      )
    elseif(WIN32)
      foreach(LIB ${ign_build_tests_LIB_DEPS})
        set(windows_libs ${windows_libs} ${LIB}.lib)
      endforeach()

      target_link_libraries(${BINARY_NAME}
         gtest.lib
         gtest_main.lib
         ${PROJECT_NAME_LOWER}${PROJECT_MAJOR_VERSION}.lib
         ${windows_libs}
      )
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
  endforeach()
endmacro()
