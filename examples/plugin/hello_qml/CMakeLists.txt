cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set (CMAKE_AUTOMOC ON)

# Find Qt5
find_package(Qt5
  COMPONENTS
    Widgets
    Qml
    Quick
    QuickControls2
    QuickWidgets
  REQUIRED)

# Find the Ignition gui library
find_package(ignition-gui0 REQUIRED)
find_package(ignition-common2 REQUIRED)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${IGNITION-GUI_CXX_FLAGS}")

include_directories(SYSTEM
  ${IGNITION-COMMON_INCLUDE_DIRS}
  ${IGNITION-GUI_INCLUDE_DIRS}
  ${Qt5Core_INCLUDE_DIRS}
  ${Qt5Widgets_INCLUDE_DIRS}
  ${Qt5Qml_INCLUDE_DIRS}
  ${Qt5Quick_INCLUDE_DIRS}
  ${Qt5QuickControls2_INCLUDE_DIRS}
  ${Qt5QuickWidgets_INCLUDE_DIRS}
)

link_directories(
  ${IGNITION-COMMON_LIBRARY_DIRS}
  ${IGNITION-GUI_LIBRARY_DIRS}
)

add_definitions(
  ${Qt5Widgets_DEFINITIONS}
  ${Qt5Qml_DEFINITIONS}
  ${Qt5Quick_DEFINITIONS}
  ${Qt5QuickControls2_DEFINITIONS}
  ${Qt5QuickWidgets_DEFINITIONS}
)

# TODO: make this work
#qt5_add_resources(resources_RCC resources.qrc)

set(plugin_name hello_qml)

add_library(${plugin_name}
  SHARED
#    ${resources_RCC}
    HelloQml.cc
)

target_link_libraries(${plugin_name}
  ${IGNITION-GUI_LIBRARIES}
  ${Qt5Core_LIBRARIES}
  ${Qt5Widgets_LIBRARIES}
  ${Qt5Qml_LIBRARIES}
  ${Qt5Quick_LIBRARIES}
  ${Qt5QuickControls2_LIBRARIES}
  ${Qt5QuickWidgets_LIBRARIES}
)
