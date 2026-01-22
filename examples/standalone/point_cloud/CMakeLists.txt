cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

project(gz-gui-point-cloud-example)

if((${CMAKE_SYSTEM_NAME} STREQUAL "Linux") OR
   (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin"))
  find_package(gz-transport QUIET REQUIRED OPTIONAL_COMPONENTS log)

  find_package(gz-common REQUIRED)

  find_package(gz-msgs REQUIRED)

  add_executable(point_cloud point_cloud.cc)
  target_link_libraries(point_cloud
    gz-transport::core
    gz-msgs
    gz-common::gz-common
    )
endif()
