include (${project_cmake_dir}/Utils.cmake)

set (headers
  IgnPublisher.hh
)

ign_install_includes(${IGN_PROJECT_NAME}/interfaces/ign ${headers})
