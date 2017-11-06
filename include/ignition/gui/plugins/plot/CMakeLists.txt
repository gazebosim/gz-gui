include (${project_cmake_dir}/Utils.cmake)

# Just the plot widget.
set (plotWidget
  Plot
)

# The rest of source files.
set (plot_qt
  PlotTracker
)

foreach(src ${plot_qt})
  set(plot_sources ${plot_sources}
    ${PROJECT_SOURCE_DIR}/src/plugins/plot/${src}.cc)
endforeach()

# Generate _MOC files.
QT5_WRAP_CPP(plotWidget_MOC ${plotWidget}.hh qwt_ignition.h)

# Create the plugin.
add_library(${plotWidget} SHARED
  ${PROJECT_SOURCE_DIR}/src/plugins/plot/${plotWidget}.cc
  ${plotWidget_MOC}
  ${plot_sources}
)

MESSAGE(STATUS "qwt lib: ${QWT_LIBRARY}")

target_link_libraries(${plotWidget}
  ${IGNITION-GUI_LIBRARIES}
  ${Qt5Core_LIBRARIES}
  ${Qt5Widgets_LIBRARIES}
  ${QWT_LIBRARY})
install (TARGETS ${plotWidget} DESTINATION
  ${IGNITION_GUI_PLUGIN_INSTALL_DIR})
