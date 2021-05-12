cmake_minimum_required(VERSION 3.10.2 FATAL_ERROR)

find_package(ignition-msgs8 REQUIRED)
find_package(ignition-transport11 REQUIRED)

add_executable(scene_provider
  scene_provider.cc
)
target_link_libraries(scene_provider
  ignition-msgs8::core
  ignition-transport11::core
)

