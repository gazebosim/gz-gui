#===============================================================================
# Generate the ruby script for internal testing.
# Note that the major version of the library is included in the name.
# Ex: cmdgui3.rb
set(cmd_script_generated_test "${CMAKE_BINARY_DIR}/test/lib/ruby/gz/cmd${GZ_DESIGNATION}${PROJECT_VERSION_MAJOR}.rb")
set(cmd_script_configured_test "${cmd_script_generated_test}.configured")

# Set the library_location variable to the full path of the library file within
# the build directory.
set(library_location "$<TARGET_FILE:${PROJECT_LIBRARY_TARGET_NAME}>")

configure_file(
  "cmd${GZ_DESIGNATION}.rb.in"
  "${cmd_script_configured_test}"
  @ONLY)

file(GENERATE
  OUTPUT "${cmd_script_generated_test}"
  INPUT  "${cmd_script_configured_test}")


#===============================================================================
# Used for the installed version.
# Generate the ruby script that gets installed.
# Note that the major version of the library is included in the name.
# Ex: cmdgui3.rb
set(cmd_script_generated "${CMAKE_CURRENT_BINARY_DIR}/cmd${GZ_DESIGNATION}${PROJECT_VERSION_MAJOR}.rb")
set(cmd_script_configured "${cmd_script_generated}.configured")

# Set the library_location variable to the relative path to the library file
# within the install directory structure.
set(library_location "../../../${CMAKE_INSTALL_LIBDIR}/$<TARGET_FILE_NAME:${PROJECT_LIBRARY_TARGET_NAME}>")

configure_file(
  "cmd${GZ_DESIGNATION}.rb.in"
  "${cmd_script_configured}"
  @ONLY)

file(GENERATE
  OUTPUT "${cmd_script_generated}"
  INPUT  "${cmd_script_configured}")

# Install the ruby command line library in an unversioned location.
install(FILES ${cmd_script_generated} DESTINATION lib/ruby/gz)

# Tack version onto and install the bash completion script
configure_file(
  "gui.bash_completion.sh"
    "${CMAKE_CURRENT_BINARY_DIR}/gui${PROJECT_VERSION_MAJOR}.bash_completion.sh" @ONLY)
install(
  FILES
    ${CMAKE_CURRENT_BINARY_DIR}/gui${PROJECT_VERSION_MAJOR}.bash_completion.sh
  DESTINATION
    ${CMAKE_INSTALL_DATAROOTDIR}/gz/gz${GZ_TOOLS_VER}.completion.d)
