# Point cloud visualization example

This example publishes a point cloud that can be visualized with the
`PointCloud` plugin. The example publishes:

* `ignition::msgs::PointCloudPacked` messages on `/point_cloud` with XYZ
  positions of points.
* `ignition::msgs::FloatV` messages on 3 different topics:
    * `/flat`: Array of 1s
    * `/sum`: Each value in the array corresponds to the sum of its point's
       coordinates (X + Y + Z).
    * `/product`: Each value in the array corresponds to the product of its
       point's  coordinates (X * Y * Z).

## Build Instructions

Navigate to this directory:

    cd <path to ign-gui>/examples/standalone/point_cloud

Build:

    mkdir build
    cd build
    cmake ..
    make

## Execute Instructions

1. Navigate to this directory:

        cd <path to ign-gui>/examples/standalone/point_cloud

1. Launch the example config file:

        ign gui -c point_cloud.config

3. From the build directory above, publish clouds to be visualized:

        ./point_cloud

4. Try out different options on the plugin.

