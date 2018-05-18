Example showing how to use Qt Quick Controls 2 in an Ignition GUI plugin.

## Dependencies

This has been tested on Ubuntu Bionic with the following dependencies:

    sudo apt-get install -y \
        qtbase5-dev \
        qtdeclarative5-dev \
        qml-module-qtquick2 \
        qtquickcontrols2-5-dev \
        qml-module-qtquick-controls\
        qml-module-qtquick-controls2

## Build

    mkdir build
    cd build
    cmake ..
    make

## Run

Make sure the plugin is in the path, for example to load the plugin standalone:

    IGN_GUI_PLUGIN_PATH=`pwd` ign gui -v 4 -s libhello_qml.so

## TODO

* Find QML file from a path that's not absolute, using resources

