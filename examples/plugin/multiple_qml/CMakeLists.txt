cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

project(gz-gui-multiple-qml)

if(POLICY CMP0100)
  cmake_policy(SET CMP0100 NEW)
endif()

set (CMAKE_AUTOMOC ON)

find_package (Qt5
  COMPONENTS
    Core
    Quick
    QuickControls2
  REQUIRED
)

# Find the Gazebo GUI library
find_package(gz-gui REQUIRED)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GZ-GUI_CXX_FLAGS}")

qt_add_resources(resources_RCC MultipleQml.qrc)

# Generate examples
add_library(MultipleQml SHARED ${headers_MOC}
  MultipleQml.cc
  ${resources_RCC}
)
target_link_libraries(MultipleQml
  gz-gui::gz-gui
)
