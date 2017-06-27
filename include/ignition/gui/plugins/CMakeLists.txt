include (${project_cmake_dir}/Utils.cmake)

set (qt_headers_plugins ${qt_headers_plugins}
  plugins/PlayButtonPlugin.hh
  PARENT_SCOPE
)

set (qt_headers_local
  PlayButtonPlugin
)

set(IGNITION_GUI_PLUGIN_INSTALL_DIR
  ${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR}/ign-gui-${PROJECT_MAJOR_VERSION}/plugins/
)

# Plugin shared libraries
foreach (src ${qt_headers_local})
  QT5_WRAP_CPP(${src}_MOC ${src}.hh)
  add_library(${src} SHARED ${PROJECT_SOURCE_DIR}/src/plugins/${src}.cc ${${src}_MOC})
  target_link_libraries(${src}
    ${IGNITION-GUI_LIBRARIES}
    ${Qt5Core_LIBRARIES}
    ${Qt5Widgets_LIBRARIES})
  install (TARGETS ${src} DESTINATION ${IGNITION_GUI_PLUGIN_INSTALL_DIR})
endforeach (src ${qt_headers_local})

