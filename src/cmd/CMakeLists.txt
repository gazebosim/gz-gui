# Collect source files into the "sources" variable and unit test files into the
# "gtest_sources" variable.
ign_get_libsources_and_unittests(sources gtest_sources)

add_library(ign STATIC ign.cc)
target_include_directories(ign PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(ign PUBLIC
  ${PROJECT_LIBRARY_TARGET_NAME}
  ignition-common${IGN_COMMON_VER}::ignition-common${IGN_COMMON_VER}
)

set(gui_executable ign-gui)
add_executable(${gui_executable} gui_main.cc)
target_link_libraries(${gui_executable}
  ign
  ignition-utils${IGN_UTILS_VER}::cli
  ignition-common${IGN_COMMON_VER}::ignition-common${IGN_COMMON_VER}
)

set(EXE_INSTALL_DIR "${IGN_LIB_INSTALL_DIR}/ignition/${IGN_DESIGNATION}${PROJECT_VERSION_MAJOR}")

install(
  TARGETS
  ${gui_executable}
  DESTINATION
  ${EXE_INSTALL_DIR}
)

# Build the unit tests.
ign_build_tests(TYPE UNIT SOURCES ${gtest_sources}
  TEST_LIST test_list
  LIB_DEPS
		${EXTRA_TEST_LIB_DEPS}
		ignition-common${IGN_COMMON_VER}::ignition-common${IGN_COMMON_VER}
)

foreach(test ${test_list})
  target_link_libraries(${test} ign)

  # Inform each test of its output directory so it knows where to call the
  # auxiliary files from. Using a generator expression here is useful for
  # multi-configuration generators, like Visual Studio.
  target_compile_definitions(${test} PRIVATE
    "DETAIL_IGN_TRANSPORT_TEST_DIR=\"$<TARGET_FILE_DIR:${test}>\""
    "IGN_TEST_LIBRARY_PATH=\"$<TARGET_FILE_DIR:${PROJECT_LIBRARY_TARGET_NAME}>\"")

endforeach()

#===============================================================================
# Generate the ruby script and yaml configuration files for installation.
# Note that the major version of the library is included in the name.
# Ex: gui5.yaml
# Ex: cmdgui5.rb
set(ign_library_path "${CMAKE_INSTALL_PREFIX}/lib/ruby/ignition/cmd${IGN_DESIGNATION}${PROJECT_VERSION_MAJOR}")
# installed yaml file contains no generator expressions, so it can be configured directly
set(conf_yaml_configured "${CMAKE_CURRENT_BINARY_DIR}/${IGN_DESIGNATION}${PROJECT_VERSION_MAJOR}.yaml")
configure_file(
  "${IGN_DESIGNATION}.yaml.in"
  "${conf_yaml_configured}" @ONLY)

# Install the yaml configuration files in an unversioned location.
install(FILES ${conf_yaml_configured}
  DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATAROOTDIR}/ignition/)


# Set the exe_location variable to the relative path to the executable file
# within the install directory structure.
set(gui_exe_location "../../../${EXE_INSTALL_DIR}/$<TARGET_FILE_NAME:${gui_executable}>")

set(cmd_script_configured "${CMAKE_CURRENT_BINARY_DIR}/cmd${IGN_DESIGNATION}${PROJECT_VERSION_MAJOR}.rb.configured")
configure_file(
  "cmd${IGN_DESIGNATION}.rb.in"
  "${cmd_script_configured}"
  @ONLY)

set(cmd_script_generated "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/cmd${IGN_DESIGNATION}${PROJECT_VERSION_MAJOR}.rb")
file(GENERATE
  OUTPUT "${cmd_script_generated}"
  INPUT  "${cmd_script_configured}")

# Install the ruby command line library in an unversioned location.
install(FILES ${cmd_script_generated} DESTINATION lib/ruby/ignition)

#===============================================================================
# Generate the ruby script and yaml configuration files for internal testing.
# Note that the major version of the library is included in the name.
# Ex: gui1.yaml
# Ex: cmdgui1.rb

# test yaml file contains generator expressions
set(ign_library_path "${CMAKE_BINARY_DIR}/test/lib/$<CONFIG>/ruby/ignition/cmd${IGN_DESIGNATION}${PROJECT_VERSION_MAJOR}")
configure_file(
  "${IGN_DESIGNATION}.yaml.in"
  "${CMAKE_CURRENT_BINARY_DIR}/${IGN_DESIGNATION}${PROJECT_VERSION_MAJOR}.yaml.configured" @ONLY)

file(GENERATE
  OUTPUT "${CMAKE_BINARY_DIR}/test/conf/$<CONFIG>/${IGN_DESIGNATION}${PROJECT_VERSION_MAJOR}.yaml"
  INPUT "${CMAKE_CURRENT_BINARY_DIR}/${IGN_DESIGNATION}${PROJECT_VERSION_MAJOR}.yaml.configured")


# Set the exe_location variable to the full path of the executable file within
# the build directory.
set(gui_exe_location "$<TARGET_FILE:${gui_executable}>")

set(cmd_script_configured_test "${CMAKE_CURRENT_BINARY_DIR}/test_cmd${IGN_DESIGNATION}${PROJECT_VERSION_MAJOR}.rb.configured")
configure_file(
  "cmd${IGN_DESIGNATION}.rb.in"
  "${cmd_script_configured_test}"
  @ONLY)

set(cmd_script_generated_test "${ign_library_path}.rb")
file(GENERATE
  OUTPUT "${cmd_script_generated_test}"
  INPUT  "${cmd_script_configured_test}")


if (TARGET UNIT_ign_TEST)
  add_dependencies(UNIT_ign_TEST
    ${gui_executable}
    Publisher
    TopicEcho
  )

  set(_env_vars)
  list(APPEND _env_vars "IGN_CONFIG_PATH=${CMAKE_BINARY_DIR}/test/conf/$<CONFIG>")
  list(APPEND _env_vars "IGN_GUI_PLUGIN_PATH=$<TARGET_FILE_DIR:Publisher>")
  if(WIN32)
    list(APPEND _env_vars "QT_QPA_PLATFORM_PLUGIN_PATH=C:/vcpkg/installed/x64-windows/plugins/platforms")
    list(APPEND _env_vars "QT_QPA_PLATFORM=offscreen")
  endif()
  set_tests_properties(
    UNIT_ign_TEST
    PROPERTIES
    ENVIRONMENT "${_env_vars}"
  )
endif()

if(Qt5_FOUND AND WIN32 AND TARGET Qt5::qmake AND NOT TARGET Qt5::windeployqt)
    get_target_property(_qt5_qmake_location
        Qt5::qmake IMPORTED_LOCATION
    )

    execute_process(
        COMMAND "${_qt5_qmake_location}" -query QT_INSTALL_PREFIX
        RESULT_VARIABLE return_code
        OUTPUT_VARIABLE qt5_install_prefix
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    # Anaconda3 Location
    set(imported_location "${qt5_install_prefix}/bin/windeployqt.exe")
    # vcpkg location
    set(imported_location2 "${qt5_install_prefix}/tools/qt5/bin/windeployqt.exe")

    if(EXISTS ${imported_location})
        set(QML_DIR ${qt5_install_prefix}/qml)
        add_executable(Qt5::windeployqt IMPORTED)

        set_target_properties(Qt5::windeployqt PROPERTIES
            IMPORTED_LOCATION ${imported_location}
        )
    elseif(EXISTS ${imported_location2})
        set(QML_DIR  ${qt5_install_prefix}/qml)
        add_executable(Qt5::windeployqt IMPORTED)

        set_target_properties(Qt5::windeployqt PROPERTIES
            IMPORTED_LOCATION ${imported_location2}
        )
    else()
        message("Not able to locate windeployqt.exe")
    endif()
endif()

if(TARGET Qt5::windeployqt)
    # execute windeployqt in a tmp directory after build
    add_custom_command(TARGET ${gui_executable}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_CURRENT_BINARY_DIR}/windeployqt"
        COMMAND set PATH=%PATH%$<SEMICOLON>${qt5_install_prefix}/bin
        COMMAND Qt5::windeployqt --dir "${CMAKE_CURRENT_BINARY_DIR}/windeployqt" --qmldir ${QML_DIR} ${CMAKE_BINARY_DIR}/bin/$<CONFIG>/ign-gui.exe --quick --qml
    )

    # copy deployment directory during installation
    install(
        DIRECTORY
        "${CMAKE_CURRENT_BINARY_DIR}/windeployqt/"
        DESTINATION ${IGN_LIB_INSTALL_DIR}/ignition/${IGN_DESIGNATION}${PROJECT_VERSION_MAJOR}/
    )
endif()
