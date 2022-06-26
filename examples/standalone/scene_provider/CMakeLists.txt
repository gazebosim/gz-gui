cmake_minimum_required(VERSION 3.10.2 FATAL_ERROR)

project(gz-gui-scene-provider)

find_package(gz-msgs9 REQUIRED)
find_package(gz-transport12 REQUIRED)

add_executable(scene_provider
  scene_provider.cc
)
target_link_libraries(scene_provider
  gz-msgs9::core
  gz-transport12::core
)
