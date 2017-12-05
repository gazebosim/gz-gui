cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

set (CMAKE_AUTOMOC ON)

# Find Qt5
find_package (Qt5Widgets REQUIRED)
find_package (Qt5Core REQUIRED)

# Find the Ignition gui library
find_package(ignition-gui0 REQUIRED)
find_package(ignition-math4 REQUIRED)
find_package(ignition-common0 REQUIRED)
find_package(ignition-rendering0 REQUIRED)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${IGNITION-GUI_CXX_FLAGS}")

include_directories(SYSTEM
    ${IGNITION-COMMON_INCLUDE_DIRS}
    ${IGNITION-MATH_INCLUDE_DIRS}
#    ${IGNITION-GUI_INCLUDE_DIRS}
    ${IGNITION-RENDERING_INCLUDE_DIRS}
    ${Qt5Core_INCLUDE_DIRS})

link_directories(
  ${IGNITION-COMMON_LIBRARY_DIRS}
  ${IGNITION-GUI_LIBRARY_DIRS}
  ${IGNITION-MATH_LIBRARY_DIRS}
  ${IGNITION-RENDERING_LIBRARY_DIRS}
)

add_library(MovingRandomly SHARED ${headers_MOC}
  MovingRandomlyPlugin.cc)
target_link_libraries(MovingRandomly
  ${IGNITION-COMMON_LIBRARIES}
  ${IGNITION-GUI_LIBRARIES}
  ${IGNITION-MATH_LIBRARIES}
  ${IGNITION-RENDERING_LIBRARIES}
  ${Qt5Core_LIBRARIES}
  ${Qt5Widgets_LIBRARIES})
