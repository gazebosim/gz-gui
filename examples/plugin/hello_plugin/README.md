Minimal example for a plugin that connects QML to C++ and parses
configuration from XML.

## Build

    mkdir build
    cd build
    cmake ..
    make

## Run

Standalone:

    cd build
    export IGN_GUI_PLUGIN_PATH=`pwd`; ign gui -s HelloPlugin

Within a window where other plugins can also be inserted, using a custom
configuration:

    cd build
    export IGN_GUI_PLUGIN_PATH=`pwd`; ign gui -c ../HelloPlugin.config

Or open an empty window and insert from menu:

    cd build
    export IGN_GUI_PLUGIN_PATH=`pwd`; ign gui
    # Choose HelloPlugin from menu
