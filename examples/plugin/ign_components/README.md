Demo of QML components provided by Ignition GUI which can be
useful for downstream developers.

## Build

    mkdir build
    cd build
    cmake ..
    make

## Run

Standalone:

    cd build
    export GZ_GUI_PLUGIN_PATH=`pwd`; ign gui -s IgnComponents

Within a window where other plugins can also be inserted, using a custom
configuration:

    cd build
    export GZ_GUI_PLUGIN_PATH=`pwd`; ign gui -c ../IgnComponents.config

