cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

project(gz-gui-dialogs)

if(POLICY CMP0100)
  cmake_policy(SET CMP0100 NEW)
endif()

set (CMAKE_AUTOMOC ON)

find_package (Qt5 5.15
  COMPONENTS
    Core
    Quick
    QuickControls2
  REQUIRED
)

# Find the Gazebo GUI library
find_package(gz-gui REQUIRED)

# Generate example
add_executable(dialogs
  dialogs.cc
)
target_link_libraries(dialogs
  gz-gui::gz-gui
)
