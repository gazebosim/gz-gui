cmake_minimum_required(VERSION 3.10.2 FATAL_ERROR)

project(gz-gui-hello-plugin)

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
find_package(gz-gui9 REQUIRED)
set(GZ_GUI_VER ${gz-gui9_VERSION_MAJOR})

qt_add_resources(resources_RCC hello.qrc)

# Generate examples
add_library(HelloPlugin SHARED ${headers_MOC}
  HelloPlugin.cc
  ${resources_RCC}
)
target_link_libraries(HelloPlugin
  gz-gui${GZ_GUI_VER}::gz-gui${GZ_GUI_VER}
)
