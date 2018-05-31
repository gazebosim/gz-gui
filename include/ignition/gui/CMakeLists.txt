include_directories(
  ${Qt5Core_INCLUDE_DIRS}
  ${tinyxml_INCLUDE_DIRS}
  ${Qt5Qml_INCLUDE_DIRS}
  ${Qt5Quick_INCLUDE_DIRS}
  ${Qt5QuickControls2_INCLUDE_DIRS}
  ${Qt5QuickWidgets_INCLUDE_DIRS}
)

set (CMAKE_AUTOMOC ON)

add_definitions(
  ${Qt5Core_DEFINITIONS}
  ${Qt5Qml_DEFINITIONS}
  ${Qt5Quick_DEFINITIONS}
  ${Qt5QuickControls2_DEFINITIONS}
  ${Qt5QuickWidgets_DEFINITIONS}
)

set (qt_headers
  BoolWidget.hh
  CollapsibleWidget.hh
  ColorWidget.hh
  Dialog.hh
  Dock.hh
  EditableLabel.hh
  EnumWidget.hh
  GeometryWidget.hh
  MainWindow.hh
  MessageWidget.hh
  NumberWidget.hh
  Plugin.hh
  Pose3dWidget.hh
  PropertyWidget.hh
  StringWidget.hh
  VariablePill.hh
  VariablePillContainer.hh
  Vector3dWidget.hh
)

set (headers
  Conversions.hh
  DragDropModel.hh
  Enums.hh
  Helpers.hh
  Iface.hh
  ign.hh
  qt.h
  qwt.h
  QtMetatypes.hh
  SearchModel.hh
  System.hh
)

set (resources resources.qrc)

QT5_WRAP_CPP(headers_MOC ${qt_headers})
QT5_ADD_RESOURCES(resources_RCC ${resources})

ign_create_core_library(SOURCES
  ${sources}
  ${headers_MOC}
  ${resources_RCC}
)

target_link_libraries(${PROJECT_LIBRARY_TARGET_NAME}
  ${IGNITION-COMMON_LIBRARIES}
  ${IGNITION-MATH_LIBRARIES}
  ${IGNITION-MSGS_LIBRARIES}
  ${IGNITION-TRANSPORT_LIBRARIES}
  ${Qt5Core_LIBRARIES}
  ${Qt5Widgets_LIBRARIES}
  ${Qt5Qml_LIBRARIES}
  ${Qt5Quick_LIBRARIES}
  ${Qt5QuickControls2_LIBRARIES}
  ${Qt5QuickWidgets_LIBRARIES}
  TINYXML2::TINYXML2
)

#add_subdirectory(plugins)

#ign_install_includes("" ${CMAKE_CURRENT_BINARY_DIR}/${IGN_DESIGNATION}.hh)
#ign_install_includes(${IGN_DESIGNATION} ${headers} ${qt_headers})

ign_install_all_headers()

