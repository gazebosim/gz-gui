#################################################
# gz_gui_add_library (<library_name>
#              SOURCES <sources>
#              [PUBLIC_LINK_LIBS <libraries...>]
#              [PRIVATE_LINK_LIBS <libraries...>])
#
# Add a plugin library to Gazebo GUI.
#
# <library_name> Required. Name of the library
#
# [SOURCES]: Specify the source files for the plugin.
#
# [QT_HEADERS]: Qt headers that need to be MOC'ed
#
# [PUBLIC_LINK_LIBS]: Specify a list of libraries to be publicly linked.
#
# [PRIVATE_LINK_LIBS]: Specify a list of libraries to be privately linked.
#
function(gz_gui_add_library library_name)
  set(options)
  set(oneValueArgs)
  set(multiValueArgs SOURCES QT_HEADERS PUBLIC_LINK_LIBS PRIVATE_LINK_LIBS)
  cmake_parse_arguments(gz_gui_add_library "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  qt_wrap_cpp(${library_name}_headers_MOC ${gz_gui_add_library_QT_HEADERS})
  qt_add_resources(${library_name}_RCC ${library_name}.qrc)

  add_library(${library_name} SHARED
    ${gz_gui_add_library_SOURCES}
    ${${library_name}_headers_MOC}
    ${${library_name}_RCC}
  )
  target_link_libraries(${library_name}
    PUBLIC
      ${PROJECT_LIBRARY_TARGET_NAME}
      TINYXML2::TINYXML2
      ${gz_gui_add_library_PUBLIC_LINK_LIBS}
    PRIVATE
      ${gz_gui_add_library_PRIVATE_LINK_LIBS}
  )
endfunction()


#################################################
# gz_gui_add_plugin(<plugin_name>
#              SOURCES <sources>
#              [PUBLIC_LINK_LIBS <libraries...>]
#              [PRIVATE_LINK_LIBS <libraries...>])
#
# Add a plugin to Gazebo GUI.
#
# <plugin_name> Required. Name of the plugin.
#
# [SOURCES]: Specify the source files for the system.
#
# [QT_HEADERS]: Qt headers that need to be MOC'ed
#
# [TEST_SOURCES]: Source files for unit tests.
#
# [PUBLIC_LINK_LIBS]: Specify a list of libraries to be publicly linked.
#
# [PRIVATE_LINK_LIBS]: Specify a list of libraries to be privately linked.
#
function(gz_gui_add_plugin_qt5 plugin_name)
  set(options)
  set(oneValueArgs)
  set(multiValueArgs
    SOURCES
    QT_HEADERS
    TEST_SOURCES
    PUBLIC_LINK_LIBS
    PRIVATE_LINK_LIBS
  )
  cmake_parse_arguments(gz_gui_add_plugin_qt5 "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  gz_gui_add_library(${plugin_name}
    SOURCES ${gz_gui_add_plugin_qt5_SOURCES}
    QT_HEADERS ${gz_gui_add_plugin_qt5_QT_HEADERS}
    PUBLIC_LINK_LIBS ${gz_gui_add_plugin_qt5_PUBLIC_LINK_LIBS}
    PRIVATE_LINK_LIBS ${gz_gui_add_plugin_qt5_PRIVATE_LINK_LIBS} gz-plugin${GZ_PLUGIN_VER}::register
  )

  if(gz_gui_add_plugin_qt5_TEST_SOURCES)
    gz_build_tests(TYPE UNIT
      SOURCES
        ${gz_gui_add_plugin_qt5_TEST_SOURCES}
      LIB_DEPS
        ${GZ-GUI_LIBRARIES}
        TINYXML2::TINYXML2
        ${plugin_name}
      INCLUDE_DIRS
        # Used to make internal source file headers visible to the unit tests
        ${CMAKE_CURRENT_SOURCE_DIR}
        # Used to make test-directory headers visible to the unit tests
        ${PROJECT_SOURCE_DIR}
        # Used to make test_config.h visible to the unit tests
        ${PROJECT_BINARY_DIR}
      ENVIRONMENT
        GZ_GUI_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX})
  endif()

  if (MSVC)
    # Warning #4251 is the "dll-interface" warning that tells you when types used
    # by a class are not being exported. These generated source files have private
    # members that don't get exported, so they trigger this warning. However, the
    # warning is not important since those members do not need to be interfaced
    # with.
    set_source_files_properties(
        ${gz_gui_add_plugin_qt5_SOURCES}
        ${gz_gui_add_plugin_qt5_TEST_SOURCES}
        COMPILE_FLAGS "/wd4251")
  endif()

  install (TARGETS ${plugin_name} DESTINATION ${GZ_GUI_PLUGIN_INSTALL_DIR})
endfunction()

function(gz_gui_add_plugin_qt6 plugin_name)
  set(options)
  set(oneValueArgs)
  set(multiValueArgs
    SOURCES
    QML_SOURCES
    QML_URI
    QT_HEADERS
    TEST_SOURCES
    PUBLIC_LINK_LIBS
    PRIVATE_LINK_LIBS
  )
  cmake_parse_arguments(gz_gui_add_plugin_qt6 "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  qt_add_qml_module(${plugin_name}
    URI ${gz_gui_add_plugin_QML_URI}
    VERSION 1.0
    SOURCES ${gz_gui_add_plugin_qt6_SOURCES}  ${qt_gui_add_plugin_qt6_QT_HEADERS}
    QML_FILES ${gz_gui_add_plugin_qt6_QML_SOURCES}
  )

  target_link_libraries(${plugin_name}
    PUBLIC
      ${PROJECT_LIBRARY_TARGET_NAME}
      ${gz_gui_add_plugin_qt6_PUBLIC_LINK_LIBS}
    PRIVATE
      ${gz_gui_add_plugin_qt6_PRIVATE_LINK_LIBS}
  )
  install (TARGETS ${plugin_name} DESTINATION ${GZ_GUI_PLUGIN_INSTALL_DIR})
endfunction()


function(gz_gui_add_plugin plugin_name)
  set(options)
  set(oneValueArgs)
  set(multiValueArgs
    SOURCES
    QML_SOURCES
    QML_URI
    QT_HEADERS
    TEST_SOURCES
    PUBLIC_LINK_LIBS
    PRIVATE_LINK_LIBS
  )
  cmake_parse_arguments(gz_gui_add_plugin "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  if (QT_MAJOR_VERSION EQUAL 6)
    gz_gui_add_plugin_qt6(${plugin_name}
      SOURCES ${gz_gui_add_plugin_SOURCES}
      QML_URI ${gz_gui_add_plugin_QML_URI}
      QML_SOURCES ${gz_gui_add_plugin_QML_SOURCES}
      QT_HEADERS ${gz_gui_add_plugin_QT_HEADERS}
      TEST_SOURCES ${gz_gui_add_plugin_TEST_SOURCES}
      PUBLIC_LINK_LIBS ${gz_gui_add_plugin_PUBLIC_LINK_LIBS}
      PRIVATE_LINK_LIBS ${gz_gui_add_plugin_PRIVATE_LINK_LIBS}
    )
  else()
    gz_gui_add_plugin_qt5(${plugin_name}
      SOURCES ${gz_gui_add_plugin_SOURCES}
      QT_HEADERS ${gz_gui_add_plugin_QT_HEADERS}
      TEST_SOURCES ${gz_gui_add_plugin_TEST_SOURCES}
      PUBLIC_LINK_LIBS ${gz_gui_add_plugin_PUBLIC_LINK_LIBS}
      PRIVATE_LINK_LIBS ${gz_gui_add_plugin_PRIVATE_LINK_LIBS}
    )
  endif()
endfunction()


# Plugins
add_subdirectory(camera_fps)
add_subdirectory(camera_tracking)
add_subdirectory(publisher)
