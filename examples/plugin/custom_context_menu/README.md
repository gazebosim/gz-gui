## Build

    mkdir build
    cd build
    cmake ..
    make

## Run

Standalone

    cd build
    export IGN_GUI_PLUGIN_PATH=`pwd`; ign gui -s CustomContext

Or open an empty window and insert from menu

    cd build
    export IGN_GUI_PLUGIN_PATH=`pwd`; ign gui
    # Choose CustomContext from menu
