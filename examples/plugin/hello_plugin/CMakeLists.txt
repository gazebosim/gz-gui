cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set (CMAKE_AUTOMOC ON)

# Find Qt5
find_package (Qt5
  COMPONENTS
    Core
    Widgets
    Quick
    QuickControls2
  REQUIRED
)

# Find the Ignition gui library
find_package(ignition-gui0 REQUIRED)
find_package(ignition-common2 REQUIRED)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${IGNITION-GUI_CXX_FLAGS}")

QT5_ADD_RESOURCES(resources_RCC hello.qrc)

include_directories(SYSTEM
  ${IGNITION-COMMON_INCLUDE_DIRS}
  ${IGNITION-GUI_INCLUDE_DIRS}
  ${Qt5Core_INCLUDE_DIRS}
  ${Qt5Qml_INCLUDE_DIRS}
  ${Qt5Quick_INCLUDE_DIRS}
  ${Qt5QuickControls2_INCLUDE_DIRS}
)

link_directories(
  ${IGNITION-COMMON_LIBRARY_DIRS}
  ${IGNITION-GUI_LIBRARY_DIRS}
)

# Generate examples
add_library(hello_plugin SHARED ${headers_MOC}
  HelloPlugin.cc
  ${resources_RCC}
)
target_link_libraries(hello_plugin
  ${IGNITION-GUI_LIBRARIES}
  ${Qt5Core_LIBRARIES}
  ${Qt5Widgets_LIBRARIES}
  ${Qt5Qml_LIBRARIES}
  ${Qt5Quick_LIBRARIES}
  ${Qt5QuickControls2_LIBRARIES}
)
