cmake_minimum_required(VERSION 3.10.2 FATAL_ERROR)

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
find_package(gz-gui9 REQUIRED)
set(GZ_GUI_VER ${gz-gui9_VERSION_MAJOR})

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GZ-GUI_CXX_FLAGS}")

qt_add_resources(resources_RCC MultipleQml.qrc)

# Generate examples
add_library(MultipleQml SHARED ${headers_MOC}
  MultipleQml.cc
  ${resources_RCC}
)
target_link_libraries(MultipleQml
  gz-gui${GZ_GUI_VER}::gz-gui${GZ_GUI_VER}
)
