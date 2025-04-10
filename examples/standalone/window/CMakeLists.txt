cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

project(gz-gui-window)

if(POLICY CMP0100)
  cmake_policy(SET CMP0100 NEW)
endif()

set (CMAKE_AUTOMOC ON)

# Find Qt5
find_package (Qt5 5.15
  COMPONENTS
    Core
    Quick
    QuickControls2
  REQUIRED
)

# Find the Gazebo GUI library
find_package(gz-gui REQUIRED)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GZ-GUI_CXX_FLAGS}")

# Generate example
add_executable(window
  window.cc
)
target_link_libraries(window
  gz-gui::gz-gui
)
