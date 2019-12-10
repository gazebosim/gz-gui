\page examples Examples

Next Tutorial: \ref examples
Previous Tutorial: \ref style

## Overview

Ignition GUI comes with several examples of plugins and other files
under the `examples` folder.

## Standalone applications

Ignition GUI can be used as a C++ library by other applications.

Examples provided under `examples/standalone` can be compiled and run without
the `ign gui` command line tool.

For example, build an example in which an empty window can be opened
programmatically as follows:

    cd examples/standalone/window
    mkdir build
    cd build
    cmake ..
    make

Then run it:

    ./window

## Custom plugins

Ignition GUI comes with several plugins, but its true power is loading custom
plugins developed by others. Under `examples/plugins`, there are several
examples on how to build different types of plugins.

For example, we can build the `hello_plugin` example plugin:

    cd examples/plugin/hello_plugin
    mkdir build
    cd build
    cmake ..
    make

Then create the plugins directory and copy the generated library to it so that Ignition GUI can find the plugin:

    mkdir -p ~/.ignition/gui/plugins
    cp libHelloPlugin.so ~/.ignition/gui/plugins/

Now you can use the command line to open it:

    ign gui -s HelloPlugin

### All example plugins

* `hello_plugin`: A button that prints hello on click.
* `custom_context_menu`: Overrides the default context menu.

## Configuration files

Configuration files are used to load an Ignition GUI window with several
plugins and custom window configurations.

Example configuration files are provided in the `/examples/config` folder.

Unlike example plugins and standalone programs, these files don't require
any extra installation step and can be run directly from the command line.

For example, you can run the example `pubsub.config` from the command line:

    ign gui -c examples/config/pubsub.config
