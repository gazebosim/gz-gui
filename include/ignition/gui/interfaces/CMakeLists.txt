include (${project_cmake_dir}/Utils.cmake)

set (headers
  Publisher.hh
)

ign_install_includes(${IGN_PROJECT_NAME}/interfaces ${headers})

add_subdirectory(ign)
