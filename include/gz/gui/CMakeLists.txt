if(POLICY CMP0100)
  cmake_policy(SET CMP0100 NEW)
endif()

set (CMAKE_AUTOMOC ON)

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

qt_wrap_cpp(headers_MOC ${qt_headers})
qt_add_resources(resources_RCC ${resources})

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
    gz-common::events
    gz-math::gz-math
    gz-msgs::gz-msgs
    gz-plugin::loader
    gz-transport::gz-transport
    Qt::Core
    Qt::Qml
    Qt::Quick
    Qt::QuickControls2
    Qt::Widgets
    TINYXML2::TINYXML2
)

target_compile_definitions(${PROJECT_LIBRARY_TARGET_NAME} PRIVATE
  SHARED_LIBRARY_PREFIX=\"${CMAKE_SHARED_LIBRARY_PREFIX}\"
  SHARED_LIBRARY_SUFFIX=\"${CMAKE_SHARED_LIBRARY_SUFFIX}\")

gz_install_all_headers()
