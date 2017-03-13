include (${project_cmake_dir}/Utils.cmake)

# Generate a the ruby script.
# Note that the major version of the library is included in the name.
if (APPLE)
  set(IGN_LIBRARY_NAME lib${PROJECT_NAME_LOWER}${PROJECT_MAJOR_VERSION}.dylib)
else()
  set(IGN_LIBRARY_NAME lib${PROJECT_NAME_LOWER}${PROJECT_MAJOR_VERSION}.so)
endif()
configure_file(
  "cmdgui.rb.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmdgui${PROJECT_MAJOR_VERSION}.rb" @ONLY)

# Install the ruby command line library in an unversioned location.
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/cmdgui${PROJECT_MAJOR_VERSION}.rb DESTINATION lib/ruby/ignition)
