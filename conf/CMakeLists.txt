set(gz_library_path "${CMAKE_BINARY_DIR}/src/cmd/cmdgui${PROJECT_VERSION_MAJOR}")

# Generate a configuration file for internal testing.
# Note that the major version of the library is included in the name.
# Ex: gui0.yaml
configure_file(
  "gui.yaml.in"
    "${CMAKE_BINARY_DIR}/test/conf/gui${PROJECT_VERSION_MAJOR}.yaml" @ONLY)

set(gz_library_path "${CMAKE_INSTALL_PREFIX}/lib/ruby/ignition/cmdgui${PROJECT_VERSION_MAJOR}")

# Generate a configuration file.
# Note that the major version of the library is included in the name.
# Ex: gui0.yaml
configure_file(
  "gui.yaml.in"
    "${CMAKE_CURRENT_BINARY_DIR}/gui${PROJECT_VERSION_MAJOR}.yaml" @ONLY)

# Install the yaml configuration files in an unversioned location.
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/gui${PROJECT_VERSION_MAJOR}.yaml DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATAROOTDIR}/ignition/)
