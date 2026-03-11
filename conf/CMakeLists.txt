# Used only for internal testing.
set(gz_library_path "${CMAKE_BINARY_DIR}/test/lib/ruby/gz/cmd${GZ_DESIGNATION}")

# Generate a configuration file for internal testing.
# Note that the major version of the library is included in the name.
# Ex: gui0.yaml
configure_file(
  "${GZ_DESIGNATION}.yaml.in"
    "${CMAKE_BINARY_DIR}/test/conf/${GZ_DESIGNATION}.yaml" @ONLY)

# Used for the installed version.
set(gz_library_path "${CMAKE_INSTALL_PREFIX}/lib/ruby/gz/cmd${GZ_DESIGNATION}")

# Generate the configuration file that is installed.
# Note that the major version of the library is included in the name.
# Ex: gui0.yaml
configure_file(
  "${GZ_DESIGNATION}.yaml.in"
  "${CMAKE_CURRENT_BINARY_DIR}/${GZ_DESIGNATION}.yaml" @ONLY)

# Install the yaml configuration files in an unversioned location.
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${GZ_DESIGNATION}.yaml
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/gz/)
