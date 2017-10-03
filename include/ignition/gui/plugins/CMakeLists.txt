include (${project_cmake_dir}/Utils.cmake)

set (qt_headers_local
  ImageDisplay
  Publisher
  Requester
  Responder
  TimePanel
  TopicEcho
  TopicsStats
  TopicViewer
)

set(IGNITION_GUI_PLUGIN_INSTALL_DIR
  ${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR}/ign-gui-${PROJECT_MAJOR_VERSION}/plugins/
)

# Plugin shared libraries
foreach (src ${qt_headers_local})
  QT5_WRAP_CPP(${src}_MOC ${src}.hh)
  add_library(${src} SHARED ${PROJECT_SOURCE_DIR}/src/plugins/${src}.cc ${${src}_MOC})
  target_link_libraries(${src}
    ${IGNITION-COMMON_LIBRARIES}
    ${IGNITION-GUI_LIBRARIES}
    ${IGNITION-MSGS_LIBRARIES}
    ${IGNITION-TRANSPORT_LIBRARIES}
    ${Qt5Core_LIBRARIES}
    ${Qt5Widgets_LIBRARIES})
  install (TARGETS ${src} DESTINATION ${IGNITION_GUI_PLUGIN_INSTALL_DIR})
endforeach (src ${qt_headers_local})

# Files needed for the PlotWindow plugin
set (plot_hdrs
  plot/EditableLabel
)

set (plot_qt_header
  PlotWindow
)

# Plot plugin shared library
QT5_WRAP_CPP(${plot_qt_header}_MOC ${plot_qt_header}.hh)
QT5_WRAP_CPP(EditableLabel_MOC plot/EditableLabel.hh)
add_library(${plot_qt_header} SHARED
  ${PROJECT_SOURCE_DIR}/src/plugins/${plot_qt_header}.cc
  ${PROJECT_SOURCE_DIR}/src/plugins/plot/EditableLabel.cc
  ${PROJECT_SOURCE_DIR}/src/Conversions.cc
  ${PlotWindow_MOC}
  ${EditableLabel_MOC}
)
target_link_libraries(${plot_qt_header}
  ${IGNITION-COMMON_LIBRARIES}
  ${IGNITION-GUI_LIBRARIES}
  ${IGNITION-MSGS_LIBRARIES}
  ${IGNITION-TRANSPORT_LIBRARIES}
  ${Qt5Core_LIBRARIES}
  ${Qt5Widgets_LIBRARIES}
  ${QWT_LIBRARY})
install (TARGETS ${plot_qt_header} DESTINATION
  ${IGNITION_GUI_PLUGIN_INSTALL_DIR})
