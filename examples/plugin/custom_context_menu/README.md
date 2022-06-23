## Build

    mkdir build
    cd build
    cmake ..
    make

## Run

Standalone

    cd build
    export GZ_GUI_PLUGIN_PATH=`pwd`; gz gui -s CustomContext

Or open an empty window and insert from menu

    cd build
    export GZ_GUI_PLUGIN_PATH=`pwd`; gz gui
    # Choose CustomContext from menu
