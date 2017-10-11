include (${project_cmake_dir}/Utils.cmake)

set (widget
  PlotWindow
)

# Generate _MOC files.
QT5_WRAP_CPP(${widget}_MOC
  ${widget}.hh
  ../EditableLabel.hh
)

# Create the plugin.
add_library(${widget} SHARED
  ${PROJECT_SOURCE_DIR}/src/plugins/plot/${widget}.cc
  ${PROJECT_SOURCE_DIR}/src/Conversions.cc
  ${EditableLabel_MOC}
  ${PROJECT_SOURCE_DIR}/src/plugins/EditableLabel.cc
  ${PlotWindow_MOC}

)
target_link_libraries(${widget}
  ${IGNITION-COMMON_LIBRARIES}
  ${IGNITION-GUI_LIBRARIES}
  ${IGNITION-MSGS_LIBRARIES}
  ${IGNITION-TRANSPORT_LIBRARIES}
  ${Qt5Core_LIBRARIES}
  ${Qt5Widgets_LIBRARIES}
  ${QWT_LIBRARY})
install (TARGETS ${widget} DESTINATION
  ${IGNITION_GUI_PLUGIN_INSTALL_DIR})
