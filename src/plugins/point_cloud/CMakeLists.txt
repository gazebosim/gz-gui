set (point_cloud_test_sources PointCloud_TEST.cc)

# PointCloud_TEST requires a screen, which is not available on the Apple CI system
if (NOT APPLE)
  list (REMOVE_ITEM point_cloud_test_sources PointCloud_TEST.cc)
endif()

gz_gui_add_plugin(PointCloud
  SOURCES
    PointCloud.cc
  QT_HEADERS
    PointCloud.hh
  PUBLIC_LINK_LIBS
    gz-rendering${GZ_RENDERING_VER}::gz-rendering${GZ_RENDERING_VER}
  TEST_SOURCES
    ${point_cloud_test_sources}
)
