# Ignition Visualization Marker Load Test Example

This example applies massive load to the MarkerManager.

## Build Instructions

From this directory:

    mkdir build
    cd build
    cmake ..
    make

## Execute Instructions

Launch ign gazebo unpaused then from the build directory above:

    ./marker_loadtest

The terminal will output messages indicating visualization changes that
will occur in Ignition GUI's render window.
