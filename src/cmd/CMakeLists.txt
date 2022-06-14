# Generate the ruby script.
# Note that the major version of the library is included in the name.
if (APPLE)
  set(IGN_LIBRARY_NAME lib${PROJECT_NAME_LOWER}.dylib)
else()
  set(IGN_LIBRARY_NAME lib${PROJECT_NAME_LOWER}.so)
endif()
configure_file(
  "cmdgui.rb.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmdgui${PROJECT_VERSION_MAJOR}.rb" @ONLY)

# Install the ruby command line library in an unversioned location.
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/cmdgui${PROJECT_VERSION_MAJOR}.rb DESTINATION lib/ruby/ignition)

# Tack version onto and install the bash completion script
configure_file(
  "gui.bash_completion.sh"
    "${CMAKE_CURRENT_BINARY_DIR}/gui${PROJECT_VERSION_MAJOR}.bash_completion.sh" @ONLY)
install(
  FILES
    ${CMAKE_CURRENT_BINARY_DIR}/gui${PROJECT_VERSION_MAJOR}.bash_completion.sh
  DESTINATION
    ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATAROOTDIR}/gz/gz${IGN_TOOLS_VER}.completion.d)
