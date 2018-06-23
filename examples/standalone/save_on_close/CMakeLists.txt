cmake_minimum_required(VERSION 3.10.2 FATAL_ERROR)

set (CMAKE_AUTOMOC ON)

# Find Qt5
find_package (Qt5Widgets REQUIRED)
find_package (Qt5Core REQUIRED)

# Find the Ignition gui library
find_package(ignition-gui1 REQUIRED)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${IGNITION-GUI_CXX_FLAGS}")

include_directories(SYSTEM
    ${IGNITION-GUI_INCLUDE_DIRS}
    ${Qt5Core_INCLUDE_DIRS})

link_directories(${IGNITION-GUI_LIBRARY_DIRS})

# Generate examples
if (EXISTS "${CMAKE_SOURCE_DIR}/save_on_close.cc")
  add_executable(save_on_close save_on_close.cc)
  target_link_libraries(save_on_close
    ${IGNITION-GUI_LIBRARIES}
    ${Qt5Core_LIBRARIES}
    ${Qt5Widgets_LIBRARIES})
endif()
