# No need to add plugins to sources, they will be found when building the
# shared libraries

set (gtest_local_sources
  Canvas_TEST.cc
  Curve_TEST.cc
  ExportDialog_TEST.cc
  IncrementalPlot_TEST.cc
  Plot_TEST.cc
)

ign_build_tests(SOURCES ${gtest_local_sources} LIB_DEPS Plot)
