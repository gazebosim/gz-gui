set(TEST_TYPE "INTEGRATION")

set(tests
  Examples_TEST.cc
  ExamplesBuild_TEST.cc
)

link_directories(${PROJECT_BINARY_DIR}/test)

ign_build_tests(SOURCES ${tests})
