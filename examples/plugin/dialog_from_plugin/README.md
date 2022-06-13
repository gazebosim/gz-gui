Demo showing how to get window properties from a plugin, as well as opening
a modal dialog attached to the main window from a plugin.

## Build

    mkdir build
    cd build
    cmake ..
    make

## Run

Standalone

    cd build
    export GZ_GUI_PLUGIN_PATH=`pwd`; gz gui -s DialogFromPlugin

Or open an empty window and insert from menu

    cd build
    export GZ_GUI_PLUGIN_PATH=`pwd`; gz gui
    # Choose DialogFromPlugin from menu
