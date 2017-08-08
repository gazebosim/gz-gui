cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set (CMAKE_AUTOMOC ON)

# Find Qt5
find_package (Qt5Widgets REQUIRED)
find_package (Qt5Core REQUIRED)

# Find the Ignition gui library
find_package(ignition-gui0 REQUIRED)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${IGNITION-GUI_CXX_FLAGS}")

# Custom style
QT5_ADD_RESOURCES(resources_RCC resources.qrc)

include_directories(SYSTEM
    ${IGNITION-GUI_INCLUDE_DIRS}
    ${Qt5Core_INCLUDE_DIRS})

link_directories(${IGNITION-GUI_LIBRARY_DIRS})

# Generate examples
if (EXISTS "${CMAKE_SOURCE_DIR}/custom_style.cc")
  add_executable(custom_style
    custom_style.cc
    ${resources_RCC}
  )
  target_link_libraries(custom_style
    ${IGNITION-GUI_LIBRARIES}
    ${Qt5Core_LIBRARIES}
    ${Qt5Widgets_LIBRARIES})
endif()
