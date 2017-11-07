# No need to add plugins to sources, they will be found when building the
# shared libraries

set (gtest_local_sources
  IncrementalPlot_TEST.cc
  Plot_TEST.cc
  PlotCurve_TEST.cc
)

ign_build_tests(${gtest_local_sources})
