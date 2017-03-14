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

To build test coverage, first install lcov:

    sudo apt-get install lcov

Configure coverage:

    cmake -DCMAKE_BUILD_TYPE=coverage ../; make

Run tests:

    make test # or individual test

Make coverage:

    make coverage # FIXME: currently failing

See coverage report:

    firefox coverage/index.html

## Code checker

To run the code checker:

    sh tools/code_check.sh

## Documentation generation

    # TODO

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

