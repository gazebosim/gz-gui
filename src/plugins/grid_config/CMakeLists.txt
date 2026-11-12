gz_gui_add_plugin(GridConfig
  SOURCES
    GridConfig.cc
  QT_HEADERS
    GridConfig.hh
  PUBLIC_LINK_LIBS
    gz-rendering${GZ_RENDERING_VER}::gz-rendering${GZ_RENDERING_VER}
)

# Also install Grid3D (a legacy plugin with a subset of features) by copying
# the installed GridConfig library. This ensures the correct RUNPATH is used.
# DESTDIR is honored so that packaging (e.g. Debian) also gets the copy.
set(GRID3D_PLUGIN_DIR
  "\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${GZ_GUI_PLUGIN_RELATIVE_INSTALL_DIR}")
set(GRID_CONFIG_FILE
  "${CMAKE_SHARED_LIBRARY_PREFIX}GridConfig${CMAKE_SHARED_LIBRARY_SUFFIX}")
set(GRID3D_FILE
  "${CMAKE_SHARED_LIBRARY_PREFIX}Grid3D${CMAKE_SHARED_LIBRARY_SUFFIX}")

install(CODE "
  if(EXISTS \"${GRID3D_PLUGIN_DIR}/${GRID_CONFIG_FILE}\")
    execute_process(COMMAND \"${CMAKE_COMMAND}\" -E copy_if_different
      \"${GRID3D_PLUGIN_DIR}/${GRID_CONFIG_FILE}\"
      \"${GRID3D_PLUGIN_DIR}/${GRID3D_FILE}\")
  else()
    message(WARNING \"Source file for Grid3D (legacy plugin) copy not found, skipping copy: ${GRID3D_PLUGIN_DIR}/${GRID_CONFIG_FILE}\")
  endif()
")
