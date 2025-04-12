cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

project(gz-gui-point-cloud-example)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
  find_package(gz-transport QUIET REQUIRED OPTIONAL_COMPONENTS log)
  set(GZ_TRANSPORT_VER ${gz-transport15_VERSION_MAJOR})

  find_package(gz-common6 REQUIRED)
  set(GZ_COMMON_VER ${gz-common6_VERSION_MAJOR})

  find_package(gz-msgs11 REQUIRED)
  set(GZ_MSGS_VER ${gz-msgs11_VERSION_MAJOR})

  add_executable(point_cloud point_cloud.cc)
  target_link_libraries(point_cloud
    gz-transport${GZ_TRANSPORT_VER}::core
    gz-msgs${GZ_MSGS_VER}
    gz-common${GZ_COMMON_VER}::gz-common${GZ_COMMON_VER}
    )
endif()
