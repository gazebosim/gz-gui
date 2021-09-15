# Create an IgnGui module
set(module_name IgnGui)

# Add EntityContextMenu QML plugin to IgnGui module
ign_gui_add_library(EntityContextMenu
  SOURCES EntityContextMenu.cc
  QT_HEADERS EntityContextMenu.hh
)

install (TARGETS EntityContextMenu DESTINATION ${IGNITION_GUI_PLUGIN_INSTALL_DIR}/${module_name})
install (FILES qmldir DESTINATION ${IGNITION_GUI_PLUGIN_INSTALL_DIR}/${module_name})
