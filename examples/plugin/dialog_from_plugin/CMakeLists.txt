cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

project(gz-gui-dialog-from-plugin)

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
find_package(gz-gui10 REQUIRED)
set(GZ_GUI_VER ${gz-gui10_VERSION_MAJOR})

qt_add_resources(resources_RCC DialogFromPlugin.qrc)

# Generate examples
add_library(DialogFromPlugin SHARED ${headers_MOC}
  DialogFromPlugin.cc
  ${resources_RCC}
)
target_link_libraries(DialogFromPlugin
  gz-gui${GZ_GUI_VER}::gz-gui${GZ_GUI_VER}
)
