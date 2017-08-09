include (${project_cmake_dir}/Utils.cmake)

set (qt_headers_local
  ImageDisplay
  Publisher
  Requester
  Responder
  TimePanel
  TopicEcho
  TopicViewer
)

set (plot_hdrs
  EditableLabel
  ExportDialog
  IncrementalPlot
  PlotCanvas
  PlotCurve
  PlotManager
  PlotTracker
  TopicCurveHandler
  VariablePill
  VariablePillContainer
)

set (plot_qt_header
  PlotWindow
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


# Plot plugin shared library
QT5_WRAP_CPP(${plot_qt_header}_MOC ${plot_qt_header}.hh)
QT5_WRAP_CPP(EditableLabel_MOC EditableLabel.hh)
QT5_WRAP_CPP(ExportDialog_MOC ExportDialog.hh)
QT5_WRAP_CPP(IncrementalPlot_MOC IncrementalPlot.hh)
QT5_WRAP_CPP(PlotCanvas_MOC PlotCanvas.hh)
#QT5_WRAP_CPP(PlotCurve_MOC PlotCurve.hh)
#QT5_WRAP_CPP(PlotManager_MOC PlotManager.hh)
#QT5_WRAP_CPP(PlotTracker_MOC PlotTracker.hh)
#QT5_WRAP_CPP(TopicCurveHandler_MOC TopicCurveHandler.hh)
QT5_WRAP_CPP(VariablePill_MOC VariablePill.hh)
QT5_WRAP_CPP(VariablePillContainer_MOC VariablePillContainer.hh)
add_library(${plot_qt_header} SHARED
  ${PROJECT_SOURCE_DIR}/src/plugins/${plot_qt_header}.cc
  ${PROJECT_SOURCE_DIR}/src/plugins/EditableLabel.cc
  ${PROJECT_SOURCE_DIR}/src/plugins/ExportDialog.cc
  ${PROJECT_SOURCE_DIR}/src/plugins/IncrementalPlot.cc
  ${PROJECT_SOURCE_DIR}/src/plugins/PlotCanvas.cc
  ${PROJECT_SOURCE_DIR}/src/plugins/PlotCurve.cc
  ${PROJECT_SOURCE_DIR}/src/plugins/PlotManager.cc
  ${PROJECT_SOURCE_DIR}/src/plugins/PlotTracker.cc
  ${PROJECT_SOURCE_DIR}/src/plugins/TopicCurveHandler.cc
  ${PROJECT_SOURCE_DIR}/src/plugins/VariablePill.cc
  ${PROJECT_SOURCE_DIR}/src/plugins/VariablePillContainer.cc
  ${PROJECT_SOURCE_DIR}/src/Conversions.cc
  ${PlotWindow_MOC}
  ${EditableLabel_MOC}
  ${ExportDialog_MOC}
  ${IncrementalPlot_MOC}
  ${PlotCanvas_MOC}
  ${PlotCurve_MOC}
  ${PlotManager_MOC}
  ${PlotTracker_MOC}
  ${TopicCurveHandler_MOC}
  ${VariablePill_MOC}
  ${VariablePillContainer_MOC}
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
