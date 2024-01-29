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
)

set(qml_files
  qml/Chart.qml
  qml/GzCard.qml
  qml/GzCardSettings.qml
  qml/GzColor.qml
  qml/GzHelpers.qml
  qml/GzPlotIcon.qml
  qml/GzPose.qml
  qml/GzRulers.qml
  qml/GzSnackBar.qml
  qml/GzSortFilterModel.qml
  qml/GzSpinBox.qml
  qml/GzSplit.qml
  qml/GzVector3.qml
  qml/Main.qml
  qml/PlottingInterface.qml
  qml/PluginMenu.qml
  qml/SideDrawer.qml
  qml/StandaloneDialog.qml
  qml/StyleDialog.qml
)

set(resources
  qml/images/drawer.png
  qml/images/export_icon.png
  qml/images/gazebo_logo.png
  qml/images/menu.png
  qml/images/plottable_icon.svg
  qml/images/search.svg
)

if (QT_MAJOR_VERSION EQUAL 6)
  qt_add_qml_module(${PROJECT_LIBRARY_TARGET_NAME}
    URI gz.gui
    VERSION 9.0.0
    SOURCES ${sources} ${qt_headers} ${headers}
    QML_FILES ${qml_files}
    RESOURCES ${resources}
  )

  include(GenerateExportHeader)
  generate_export_header(${PROJECT_LIBRARY_TARGET_NAME}
    BASE_NAME GZ_GUI
    EXPORT_FILE_NAME ${PROJECT_BINARY_DIR}/include/gz/gui/detail/Export.hh
    EXPORT_MACRO_NAME DETAIL_GZ_GUI_VISIBLE
    NO_EXPORT_MACRO_NAME DETAIL_GZ_GUI_HIDDEN
    DEPRECATED_MACRO_NAME GZ_DEPRECATED_ALL_VERSIONS)

  set(include_dir "gz/gui")
  set(export_base "GZ_GUI")
  configure_file(
    "${GZ_CMAKE_DIR}/Export.hh.in"
    "${PROJECT_BINARY_DIR}/include/gz/gui/Export.hh")

  install(
    FILES "${PROJECT_BINARY_DIR}/include/gz/gui/Export.hh"
    DESTINATION ${GZ_INCLUDE_INSTALL_DIR_FULL}/Export.hh
    COMPONENT headers)

  install(
    FILES "${PROJECT_BINARY_DIR}/include/gz/gui/detail/Export.hh"
    DESTINATION ${GZ_INCLUDE_INSTALL_DIR_FULL}/detail/Export.hh
    COMPONENT headers)

  target_include_directories(${PROJECT_LIBRARY_TARGET_NAME}
    PUBLIC
    $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>
    $<INSTALL_INTERFACE:include>
  )

else()
  set (resources resources.qrc)
  qt_wrap_cpp(headers_MOC ${qt_headers})
  qt_add_resources(resources_RCC ${resources})

  gz_create_core_library(SOURCES
    ${sources}
    ${headers_MOC}
    ${resources_RCC}
  )
endif()

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
    Qt::Core
    Qt::Qml
    Qt::Quick
    Qt::QuickControls2
    Qt::Widgets
    TINYXML2::TINYXML2
)

gz_install_all_headers()
