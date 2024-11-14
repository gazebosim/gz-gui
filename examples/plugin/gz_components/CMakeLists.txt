cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

project(gz-gui-gz-components)

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

find_package(gz-gui10 REQUIRED)
set(GZ_GUI_VER ${gz-gui10_VERSION_MAJOR})

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GZ-GUI_CXX_FLAGS}")

qt_add_resources(resources_RCC GzComponents.qrc)

# Generate examples
add_library(GzComponents SHARED ${headers_MOC}
  GzComponents.cc
  ${resources_RCC}
)
target_link_libraries(GzComponents
  gz-gui${GZ_GUI_VER}::gz-gui${GZ_GUI_VER}
)
