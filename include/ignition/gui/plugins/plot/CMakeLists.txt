# Just the plot widget.
set (plotWidget
  Plot
)

# Files that require QT moc.
set (plot_qt_others
  IncrementalPlot
)

# The rest of source files.
set (plot_others
  Curve
  TopicCurveHandler
  Tracker
)

foreach(src ${plot_others})
  set(plot_sources ${plot_sources}
    ${PROJECT_SOURCE_DIR}/src/plugins/plot/${src}.cc)
endforeach()

foreach(src ${plot_qt_others})
  set(plot_qt_headers ${plot_qt_headers}
    ${PROJECT_SOURCE_DIR}/include/ignition/gui/plugins/plot/${src}.hh)
  set(plot_qt_sources ${plot_qt_sources}
    ${PROJECT_SOURCE_DIR}/src/plugins/plot/${src}.cc)
endforeach()

# Generate _MOC files.
QT5_WRAP_CPP(plotWidget_MOC ${plotWidget}.hh ${plot_qt_headers})

# Create the plugin.
add_library(${plotWidget} SHARED
  ${PROJECT_SOURCE_DIR}/src/plugins/plot/${plotWidget}.cc
  ${plotWidget_MOC}
  ${plot_sources}
  ${plot_qt_sources}
)

target_link_libraries(${plotWidget}
  ${PROJECT_NAME_LOWER}
  ${Qt5Core_LIBRARIES}
  ${Qt5Widgets_LIBRARIES}
  ${QWT_LIBRARY})
install (TARGETS ${plotWidget} DESTINATION
  ${IGNITION_GUI_PLUGIN_INSTALL_DIR})
