# Generate a the ruby script.
# Note that the major version of the library is included in the name.
if (APPLE)
  set(GZ_LIBRARY_NAME lib${PROJECT_NAME_LOWER}.dylib)
else()
  set(GZ_LIBRARY_NAME lib${PROJECT_NAME_LOWER}.so)
endif()

configure_file(
  "cmdgui.rb.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmdgui${PROJECT_VERSION_MAJOR}.rb" @ONLY)

# Install the ruby command line library in an unversioned location.
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/cmdgui${PROJECT_VERSION_MAJOR}.rb DESTINATION lib/ruby/gz)
