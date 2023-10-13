include_directories(
  ${Qt5Core_INCLUDE_DIRS}
  ${tinyxml_INCLUDE_DIRS}
  ${Qt5Qml_INCLUDE_DIRS}
  ${Qt5Quick_INCLUDE_DIRS}
  ${Qt5QuickControls2_INCLUDE_DIRS}
  ${Qt5Widgets_INCLUDE_DIRS}
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
  ${Qt5Widgets_DEFINITIONS}
)

set (qt_headers
  Application.hh
  Dialog.hh
  MainWindow.hh
  PlottingInterface.hh
  Plugin.hh
)

set (headers
  Conversions.hh
  DragDropModel.hh
  Enums.hh
  Helpers.hh
  gz.hh
  qt.h
  SearchModel.hh
  System.hh
)

set (resources resources.qrc)

QT5_WRAP_CPP(headers_MOC ${qt_headers})
QT5_ADD_RESOURCES(resources_RCC ${resources})

gz_create_core_library(SOURCES
  ${sources}
  ${headers_MOC}
  ${resources_RCC}
)
gz_add_get_install_prefix_impl(GET_INSTALL_PREFIX_FUNCTION gz::gui::getInstallPrefix
                               GET_INSTALL_PREFIX_HEADER gz/gui/InstallationDirectories.hh
                               OVERRIDE_INSTALL_PREFIX_ENV_VARIABLE GZ_GUI_INSTALL_PREFIX)

set_property(
  SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/../../../src/InstallationDirectories.cc
  PROPERTY COMPILE_DEFINITIONS
  GZ_GUI_PLUGIN_RELATIVE_INSTALL_DIR="${GZ_GUI_PLUGIN_RELATIVE_INSTALL_DIR}"
)

target_link_libraries(${PROJECT_LIBRARY_TARGET_NAME}
  PUBLIC
    gz-common${GZ_COMMON_VER}::events
    gz-math${GZ_MATH_VER}::gz-math${GZ_MATH_VER}
    gz-msgs${GZ_MSGS_VER}::gz-msgs${GZ_MSGS_VER}
    gz-plugin${GZ_PLUGIN_VER}::loader
    gz-transport${GZ_TRANSPORT_VER}::gz-transport${GZ_TRANSPORT_VER}
    ${Qt5Core_LIBRARIES}
    ${Qt5Qml_LIBRARIES}
    ${Qt5Quick_LIBRARIES}
    ${Qt5QuickControls2_LIBRARIES}
    ${Qt5Widgets_LIBRARIES}
    TINYXML2::TINYXML2
)

target_compile_definitions(${PROJECT_LIBRARY_TARGET_NAME} PRIVATE
  SHARED_LIBRARY_PREFIX=\"${CMAKE_SHARED_LIBRARY_PREFIX}\"
  SHARED_LIBRARY_SUFFIX=\"${CMAKE_SHARED_LIBRARY_SUFFIX}\")

gz_install_all_headers()

