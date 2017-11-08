include (${project_cmake_dir}/Utils.cmake)

include_directories(
  ${Qt5Core_INCLUDE_DIRS}
  ${tinyxml_INCLUDE_DIRS}
)

set (CMAKE_AUTOMOC ON)

add_definitions(${Qt5Core_DEFINITIONS})

set (qt_headers
  BoolWidget.hh
  CollapsibleWidget.hh
  ColorWidget.hh
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

ign_add_library(${PROJECT_NAME_LOWER}${PROJECT_MAJOR_VERSION}
  ${sources}
  ${headers_MOC}
  ${resources_RCC}
)

target_link_libraries(${PROJECT_NAME_LOWER}${PROJECT_MAJOR_VERSION}
  ${IGNITION-COMMON_LIBRARIES}
  ${IGNITION-MSGS_LIBRARIES}
  ${IGNITION-TRANSPORT_LIBRARIES}
  ${Qt5Core_LIBRARIES}
  ${Qt5Widgets_LIBRARIES}
  ${tinyxml2_LIBRARIES}
)

ign_install_library(${PROJECT_NAME_LOWER}${PROJECT_MAJOR_VERSION})

add_subdirectory(plugins)

#################################################
### Do not change the following

set (ign_headers "" CACHE INTERNAL "Ignition headers" FORCE)
foreach (hdr ${headers} ${qt_headers})
  APPEND_TO_CACHED_STRING(ign_headers
    "Ignition headers" "#include <ignition/${IGN_PROJECT_NAME}/${hdr}>\n")
endforeach()

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/ign_auto_headers.hh.in
  ${CMAKE_CURRENT_BINARY_DIR}/${IGN_PROJECT_NAME}.hh)

ign_install_includes("" ${CMAKE_CURRENT_BINARY_DIR}/${IGN_PROJECT_NAME}.hh)
ign_install_includes(${IGN_PROJECT_NAME} ${headers} ${qt_headers})

