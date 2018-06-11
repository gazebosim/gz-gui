Install {#install}
======

Instructions on how to install from source on Ubuntu Xenial or higher.

## Dependencies

### External

Install dependencies:

    sudo apt install -y
        build-essential
        wget
        cmake
        qtbase5-dev
        libtinyxml2-dev
        libprotoc-dev
        libprotobuf-dev
        cppcheck
        mercurial

TODO Add QML

### Ignition (from source)

At the moment, these are the dependencies which need to be built from source:

TODO
        libignition-cmake1-dev
        libignition-math4-dev
        libignition-common-dev
        libignition-msgs-dev
        libignition-transport4-dev

#### Ignition rendering

Install dependencies

    sudo apt install -y libogre-1.9-dev

Clone and install

    hg clone http://bitbucket.org/ignitionrobotics/ign-rendering -b default
    cd ign-rendering
    mkdir build
    cd build
    cmake .. -DENABLE_TESTS_COMPILATION=false
    make install
    cd ../..

## Install GUI

Standard installation can be performed in UNIX systems using the following
steps:

    mkdir build/
    cd build/
    cmake ..
    sudo make install

## Uninstall

To uninstall the software installed with the previous steps:

    cd build/
    sudo make uninstall

## Testing

Tests are built by default. After building, to run all tests:

    cd build
    make test

You can also run one specific test, for example:

    ./src/UNIT_MainWindow_TEST

### Disable tests building

To build without tests, on the cmake step, do this instead:

    cmake .. -DBUILD_TESTING=False

### Test coverage

To build test coverage, first install lcov:

    sudo apt-get install lcov

Configure coverage:

    cmake -DCMAKE_BUILD_TYPE=coverage ../; make

Important: remember to install first

    make install

Run tests:

    make test # or individual test

Make coverage:

    make coverage

See coverage report:

    firefox coverage/index.html

Upload to [codecov.io](https://codecov.io/bb/ignitionrobotics/ign-gui/branch/default)
(you'll need the secret token):

    bash <(curl -s https://codecov.io/bash) -t <CODECOV_TOKEN>

## Code checker

To run the code checker:

    sh tools/code_check.sh

## Build documentation

The following explains how to build both the
[API documentation](https://ignitionrobotics.org/api/gui/0.1/index.html) and the
[tutorials](https://ignitionrobotics.org/tutorials/gui/0.1/index.html).

1. Build all documentation

        cd build
        cmake ..
        make doc

1. View generated API documentation

        firefox doxygen/html/index.html

1. View generated tutorials

        firefox tutorials/html/index.html

1. Upload documentation to ignitionrobotics.org (you'll need appropriate Amazon
   S3 credentials)

        sh upload.sh

1. If you're creating a new release, then tell ignitionrobotics.org about
   the new version. For example:

        curl -k -X POST -d '{"libName":"gui", "version":"1.0.0", "releaseDate":"2017-10-09T12:10:13+02:00","password":"secret"}' https://api.ignitionrobotics.org/1.0/versions

