# Ignition gui

** Igntion gui classes and functions for robot applications.**

Ignition gui is a component in the ignition framework, a set
of libraries designed to rapidly develop robot applications.

  [http://ignitionrobotics.org](http://ignitionrobotics.org)

## Installation

Standard installation can be performed in UNIX systems using the following
steps:

    mkdir build/
    cd build/
    cmake ..
    sudo make install

## Uninstallation

To uninstall the software installed with the previous steps:

    cd build/
    sudo make uninstall

## Testing

Tests are built by default. After building, to run all tests:

    make test

To run one specific test:

    ./src/UNIT_Example_TEST

### Disable tests building

To build without tests, on the cmake step, do this instead:

    cmake .. -DENABLE_TESTS_COMPILATION=False

### Test coverage

FIXME: bad coverage when doing `make test`

To build test coverage, first install lcov:

    sudo apt-get install lcov

Configure coverage:

    cmake -DCMAKE_BUILD_TYPE=coverage ../; make

Run tests:

    make test # or individual test

Make coverage:

    make coverage

See coverage report:

    firefox coverage/index.html

## Code checker

To run the code checker:

    sh tools/code_check.sh

## Documentation generation

    # TODO

## Plugins

### Finding plugins

Ignition GUI will look for plugins on the following paths, in this order:

1. all paths set on the `IGN_GUI_PLUGIN_PATH` environment variable
1. all paths added by calling `ignition::gui::addPluginPath`
1. `~/.ignition/gui/plugins`
1. plugins which come built into Ignition GUI

### Built-in plugins

#### Image display

Display images from an Ignition Transport topic.

    ign gui -c examples/config/image.config

or

    ign gui -s libImageDisplay.so

#### Publisher

Publish messages on an Ignition Transport topic.

1. Listen to string messages on the `/echo` topic:

    ign topic -e -t /echo

1. On a new terminal, open the publisher GUI:

    ign gui -s libPublisher.so

1. The GUI is pre-filled to publish "Hello" messages on the `/echo` topic.
   Click "Publish" to publish a message and see it on the terminal.

#### Time panel

* Display sim / real time published on a topic
* Buttons to make play / pause service requests

    ign gui -c examples/config/time.config

#### Topic echo

Echo messages from an Ignition Transport topic.

    ign gui -c examples/config/pubsub.config

#### Requester and Responder

Make custom service requests and serve custom services with these two
plugins.

    ign gui -c examples/config/reqres.config

## Command line

If you have Ignition Tools installed, you can use the `ign gui` command line tool:

`ign gui -h`

    Ignition GUI tool.

      ign gui [options]

    Options:

      -l [ --list ]              List all available plugins.

      -s [ --standalone ] arg    Run a plugin as a standalone window.
                                 Give the plugin filename as an argument

      -c [ --config ] arg        Open the main window with a configuration file.
                                 Give the configuration file path as an argument

      -v [ --verbose ] arg       Adjust the level of console output (0~4).

      -h [ --help ]              Print this help message.

      --force-version <VERSION>  Use a specific library version.

      --versions                 Show the available versions.

## Examples

### Using as a library

Ignition GUI can be used as a C++ library by other applications.

An example is provided in which an empty window can be opened
programmatically.

Build it as follows:

    cd examples/standalone/window
    mkdir build
    cd build
    cmake ..
    make

Then run it:

    ./window

### Standalone plugin

It's possible to use the command line to load a custom plugin.

First, build the example plugin:

    cd examples/plugin/hello_plugin
    mkdir build
    cd build
    cmake ..
    make

Then copy the generated library to a directory where Ignition GUI can find it:

    cp libhello_plugin.so ~/.ignition/gui/plugins/

Now you can use the command line to open it:

    ign gui -s libhello_plugin.so

### Loading a configuration file

It's possible to use the command line to load a configuration file specifying
plugins to load.

Example configuration files are provided in the `/examples/config` folder.

To run the example `hello.config`:

1. First, build the `hello_plugin` above.

1. Then, load the GUI from the command line with the configuration file:

        ign gui -c examples/config/hello.config

