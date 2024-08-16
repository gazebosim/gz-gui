cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

project(gz-gui-scene-provider)

find_package(gz-msgs11 REQUIRED)
find_package(gz-transport14 REQUIRED)

add_executable(scene_provider
  scene_provider.cc
)
target_link_libraries(scene_provider
  gz-msgs11::core
  gz-transport14::core
)
