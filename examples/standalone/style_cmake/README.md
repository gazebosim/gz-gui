This example shows how to use a custom stylesheet with Ignition GUI by configuring Cmake.

## Quickstart

    cd <path to>/examples/standalone/style_cmake/
    mkdir build
    cd build
    cmake ..
    make
    ./style_cmake

## Some pointers

1. Create a file named `style.qss`

1. Create a `qrc` file such as `resources.qrc`

1. Inside the `qrc` file, reference:

    * The qss stylesheet

    * Any images you may need

    Make sure you use the relative path with respect to the `qrc` file

1. On `CMakeLists.txt` use the `QT5_ADD_RESOURCES` macro on the `qrc` file

1. When adding the executable or library, be sure to add the result from the macro above

1. Compile as usual

