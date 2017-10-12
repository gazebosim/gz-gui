include (${project_cmake_dir}/Utils.cmake)

set (plotWidget
  Plot
)

# Generate _MOC files.
QT5_WRAP_CPP(plotWidget_MOC ${plotWidget}.hh)

# Create the plugin.
add_library(${plotWidget} SHARED
  ${PROJECT_SOURCE_DIR}/src/plugins/plot/${plotWidget}.cc
  ${plotWidget_MOC}

)
target_link_libraries(${plotWidget}
  ${IGNITION-GUI_LIBRARIES}
  ${Qt5Core_LIBRARIES}
  ${Qt5Widgets_LIBRARIES}
  ${QWT_LIBRARY})
install (TARGETS ${plotWidget} DESTINATION
  ${IGNITION_GUI_PLUGIN_INSTALL_DIR})
