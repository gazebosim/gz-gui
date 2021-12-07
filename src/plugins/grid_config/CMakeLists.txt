ign_gui_add_plugin(GridConfig
  SOURCES
    GridConfig.cc
  QT_HEADERS
    GridConfig.hh
  PUBLIC_LINK_LIBS
    ${IGNITION-RENDERING_LIBRARIES}
)

# Also install as Grid3D, which was a legacy plugin with a subset of features
set_target_properties(GridConfig PROPERTIES OUTPUT_NAME Grid3D)
install (TARGETS GridConfig DESTINATION ${IGNITION_GUI_PLUGIN_INSTALL_DIR})
