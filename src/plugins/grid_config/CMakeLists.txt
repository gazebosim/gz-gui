ign_gui_add_plugin(GridConfig
  SOURCES
    GridConfig.cc
  QT_HEADERS
    GridConfig.hh
  PUBLIC_LINK_LIBS
    ${IGNITION-RENDERING_LIBRARIES}
)

# Also install Grid3D (a legacy plugin with a subset of features) by copying the installed GridConfig library. This ensures the correct RUNPATH is used.

install(CODE "
  if(EXISTS \"\${CMAKE_INSTALL_PREFIX}/${IGNITION_GUI_PLUGIN_RELATIVE_INSTALL_DIR}/libGridConfig.so\")
    execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different
    \"\${CMAKE_INSTALL_PREFIX}/${IGNITION_GUI_PLUGIN_RELATIVE_INSTALL_DIR}/libGridConfig.so\"
    \"\${CMAKE_INSTALL_PREFIX}/${IGNITION_GUI_PLUGIN_RELATIVE_INSTALL_DIR}/libGrid3D.so\")
  else()
    message(WARNING \"Source file for Grid3D (legacy plugin) copy not found, skipping copy: \${CMAKE_INSTALL_PREFIX}/${IGNITION_GUI_PLUGIN_RELATIVE_INSTALL_DIR}/libGridConfig.so\")
  endif()
")
