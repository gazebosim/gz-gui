include_directories(
  ${Qt5Core_INCLUDE_DIRS}
  ${tinyxml_INCLUDE_DIRS}
  ${Qt5Qml_INCLUDE_DIRS}
  ${Qt5Quick_INCLUDE_DIRS}
  ${Qt5QuickControls2_INCLUDE_DIRS}
)

if(POLICY CMP0100)
  cmake_policy(SET CMP0100 NEW)
endif()

set (CMAKE_AUTOMOC ON)

add_definitions(
  ${Qt5Core_DEFINITIONS}
  ${Qt5Qml_DEFINITIONS}
  ${Qt5Quick_DEFINITIONS}
  ${Qt5QuickControls2_DEFINITIONS}
)

set (qt_headers
  Application.hh
  Dialog.hh
  MainWindow.hh
  Plugin.hh
)

set (headers
  Conversions.hh
  DragDropModel.hh
  Enums.hh
  Helpers.hh
  ign.hh
  qt.h
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
  CXX_STANDARD 17
)

target_link_libraries(${PROJECT_LIBRARY_TARGET_NAME}
  PUBLIC
    ${IGNITION-COMMON_LIBRARIES}
    ${IGNITION-MATH_LIBRARIES}
    ${IGNITION-MSGS_LIBRARIES}
    ignition-plugin${IGN_PLUGIN_VER}::loader
    ${IGNITION-TRANSPORT_LIBRARIES}
    ${Qt5Core_LIBRARIES}
    ${Qt5Qml_LIBRARIES}
    ${Qt5Quick_LIBRARIES}
    ${Qt5QuickControls2_LIBRARIES}
    TINYXML2::TINYXML2
)

ign_install_all_headers()

