This example shows how to compose a single plugin from multiple QML files.

## Build

    mkdir build
    cd build
    cmake ..
    make

## Run

Quickly check try your plugin as follows:

    cd build
    export IGN_GUI_PLUGIN_PATH=`pwd`; ign gui -s MultipleQml

