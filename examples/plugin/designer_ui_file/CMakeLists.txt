cmake_minimum_required(VERSION 3.10.2 FATAL_ERROR)

set (CMAKE_AUTOMOC ON)

# Tell Qt to process UI files referenced in source files
set (CMAKE_AUTOUIC ON)

# ui_DegignerPlugin.h will be generated in the build folder
set (CMAKE_INCLUDE_CURRENT_DIR ON)

# Find Qt5
find_package (Qt5Widgets REQUIRED)
find_package (Qt5Core REQUIRED)

# Find the Ignition gui library
find_package(ignition-gui2 REQUIRED)
find_package(ignition-common3 REQUIRED)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${IGNITION-GUI_CXX_FLAGS}")

include_directories(SYSTEM
  ${IGNITION-COMMON_INCLUDE_DIRS}
  ${IGNITION-GUI_INCLUDE_DIRS}
  ${Qt5Core_INCLUDE_DIRS}
)

link_directories(
  ${IGNITION-COMMON_LIBRARY_DIRS}
  ${IGNITION-GUI_LIBRARY_DIRS}
)

# Generate example
add_library(DesignerUI SHARED
  DesignerPlugin.cc
)
target_link_libraries(DesignerUI
  ${IGNITION-GUI_LIBRARIES}
  ${Qt5Core_LIBRARIES}
  ${Qt5Widgets_LIBRARIES}
)
