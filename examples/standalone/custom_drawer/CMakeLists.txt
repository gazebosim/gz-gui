cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

project(gz-gui-custom-drawer)

if(POLICY CMP0100)
  cmake_policy(SET CMP0100 NEW)
endif()

set (CMAKE_AUTOMOC ON)

# Find Qt5
find_package (Qt5
  COMPONENTS
    Core
    Quick
    QuickControls2
  REQUIRED
)

# Find the Gazebo GUI library
find_package(gz-gui REQUIRED)

qt_add_resources(resources_RCC custom_drawer.qrc)

# Generate example
add_executable(custom_drawer
  custom_drawer.cc
  ${resources_RCC}
)
target_link_libraries(custom_drawer
  gz-gui::gz-gui
)
