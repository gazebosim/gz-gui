cmake_minimum_required(VERSION 3.10.2 FATAL_ERROR)

project(gz-gui-custom-context-menu)

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
find_package(gz-gui8 REQUIRED)
set(GZ_GUI_VER ${gz-gui8_VERSION_MAJOR})

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GZ-GUI_CXX_FLAGS}")

QT5_ADD_RESOURCES(resources_RCC CustomContext.qrc)

# Generate examples
add_library(CustomContext SHARED ${headers_MOC}
  CustomContext.cc
  ${resources_RCC}
)
target_link_libraries(CustomContext
  gz-gui${GZ_GUI_VER}::gz-gui${GZ_GUI_VER}
  ${Qt5Core_LIBRARIES}
  ${Qt5Qml_LIBRARIES}
  ${Qt5Quick_LIBRARIES}
  ${Qt5QuickControls2_LIBRARIES}
)
