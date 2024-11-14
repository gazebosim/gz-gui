cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

project(gz-gui-custom-context-menu)

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
find_package(gz-gui10 REQUIRED)
set(GZ_GUI_VER ${gz-gui10_VERSION_MAJOR})

qt_add_resources(resources_RCC CustomContext.qrc)

# Generate examples
add_library(CustomContext SHARED ${headers_MOC}
  CustomContext.cc
  ${resources_RCC}
)
target_link_libraries(CustomContext
  gz-gui${GZ_GUI_VER}::gz-gui${GZ_GUI_VER}
)
