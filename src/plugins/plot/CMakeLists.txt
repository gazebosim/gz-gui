# No need to add plugins to sources, they will be found when building the
# shared libraries

set (gtest_local_sources
  Plot_TEST.cc
)

ign_build_tests(${gtest_local_sources})
