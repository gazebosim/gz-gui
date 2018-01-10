# No need to add plugins to sources, they will be found when building the
# shared libraries

set (gtest_local_sources
  Curve_TEST.cc
  IncrementalPlot_TEST.cc
  Plot_TEST.cc
)

ign_build_tests(TYPE UNIT SOURCES ${gtest_local_sources} LIB_DEPS Plot)
