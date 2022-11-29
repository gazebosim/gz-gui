cmake_minimum_required(VERSION 3.10.2 FATAL_ERROR)

project(ignition-gui-custom-context-menu)

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

# Find the Gazebo gui library
find_package(ignition-gui3 REQUIRED)
find_package(ignition-common3 REQUIRED)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${IGNITION-GUI_CXX_FLAGS}")

QT5_ADD_RESOURCES(resources_RCC CustomContext.qrc)

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
add_library(CustomContext SHARED ${headers_MOC}
  CustomContext.cc
  ${resources_RCC}
)
target_link_libraries(CustomContext
  ${IGNITION-GUI_LIBRARIES}
  ${Qt5Core_LIBRARIES}
  ${Qt5Qml_LIBRARIES}
  ${Qt5Quick_LIBRARIES}
  ${Qt5QuickControls2_LIBRARIES}
)
