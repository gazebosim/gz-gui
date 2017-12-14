# Animated box tracking example

This example shows a simple plugin and configuration file using features from
Ignition Rendering and Ignition Common.

* The configuration file
    * Uses the built-in `Scene3D` plugin to create an Ignition Rendering scene and add
      user cameras
    * Uses the built-in `Grid3D` plugin to add a grid to the scene
    * Uses the built-in `CameraTrack3D` plugin which provides advanced options to
      track nodes in the scene
* The custom plugin
    * Adds a directional light to the scene
    * Adds a green box to the scene
    * Animates the box using Ignition Common's `Animation` class

## How to test it

Build the plugin:

    mkdir build
    cd build
    cmake ..
    make

Add it to the path:

     export IGN_GUI_PLUGIN_PATH=`pwd`

Run the example:

    ign gui -v 2 -c ../camera_tracking_box.config

Try out different tracking settings.

