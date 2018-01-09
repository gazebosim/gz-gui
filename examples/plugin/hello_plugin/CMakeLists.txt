cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set (CMAKE_AUTOMOC ON)

# Find Qt5
find_package (Qt5Widgets REQUIRED)
find_package (Qt5Core REQUIRED)

# Find the Ignition gui library
find_package(ignition-gui0 REQUIRED)
find_package(ignition-common1 REQUIRED)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${IGNITION-GUI_CXX_FLAGS}")

include_directories(SYSTEM
    ${IGNITION-COMMON_INCLUDE_DIRS}
    ${IGNITION-GUI_INCLUDE_DIRS}
    ${Qt5Core_INCLUDE_DIRS})

link_directories(
  ${IGNITION-COMMON_LIBRARY_DIRS}
  ${IGNITION-GUI_LIBRARY_DIRS}
)

# Generate examples
if (EXISTS "${CMAKE_SOURCE_DIR}/HelloPlugin.cc")
  add_library(hello_plugin SHARED ${headers_MOC}
    HelloPlugin.cc)
  target_link_libraries(hello_plugin
    ${IGNITION-GUI_LIBRARIES}
    ${Qt5Core_LIBRARIES}
    ${Qt5Widgets_LIBRARIES})
endif()
