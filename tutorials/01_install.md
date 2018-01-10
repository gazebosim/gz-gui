Install {#install}
======

## Dependencies

### External

    sudo apt-get install -y build-essential wget cmake qtbase5-dev libtinyxml2-dev libfreeimage-dev libgts-dev uuid-dev libswscale-dev libavutil-dev libavcodec-dev libavformat-dev libprotoc-dev libprotobuf-dev libzmq3-dev protobuf-compiler cppcheck mercurial

### Ignition (from source)

```
# Ignition cmake
hg clone http://bitbucket.org/ignitionrobotics/ign-cmake -b default
cd ign-cmake
mkdir build
cd build
cmake .. -DENABLE_TESTS_COMPILATION=false
make install
cd ../..
# Ignition math
hg clone http://bitbucket.org/ignitionrobotics/ign-math -b ign-math4
cd ign-math
mkdir build
cd build
cmake .. -DENABLE_TESTS_COMPILATION=false
make install
cd ../..
# Ignition common
hg clone http://bitbucket.org/ignitionrobotics/ign-common -b ign-common1
cd ign-common
mkdir build
cd build
cmake .. -DENABLE_TESTS_COMPILATION=false
make install
cd ../..
# Ignition msgs
hg clone http://bitbucket.org/ignitionrobotics/ign-msgs -b ign-msgs1
cd ign-msgs
mkdir build
cd build
cmake .. -DENABLE_TESTS_COMPILATION=false
make install
cd ../..
# Ignition transport
hg clone http://bitbucket.org/ignitionrobotics/ign-transport -b default
cd ign-transport
mkdir build
cd build
cmake .. -DENABLE_TESTS_COMPILATION=false
make install
cd ../..
# Ignition rendering
hg clone http://bitbucket.org/ignitionrobotics/ign-rendering -b default
cd ign-rendering
mkdir build
cd build
cmake .. -DENABLE_TESTS_COMPILATION=false
make install
```

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

Important: remember to install first

    make install

Run tests:

    make test # or individual test

Make coverage:

    make coverage

See coverage report:

    firefox coverage/index.html

Upload to [codecov.io](https://codecov.io/bb/ignitionrobotics/ign-gui/branch/default):

    bash <(curl -s https://codecov.io/bash) -t <CODECOV_TOKEN>

## Code checker

To run the code checker:

    sh tools/code_check.sh

## Build documentation

1. Build documentation

        cd build
        cmake ..
        make doc

1. View documentation

        firefox build/doxygen/html/index.html

1. Upload documentation to ignitionrobotics.org (you'll need appropriate Amazon
   S3 credentials)

        cd build
        sh upload.sh

1. If you're creating a new release, then tell ignitionrobotics.org about
   the new version. For example:

        curl -k -X POST -d '{"libName":"gui", "version":"1.0.0", "releaseDate":"2017-10-09T12:10:13+02:00","password":"secret"}' https://api.ignitionrobotics.org/1.0/versions

