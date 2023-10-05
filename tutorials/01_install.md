\page install Installation

Next Tutorial: \ref commandline

# Install

These instructions are for installing only Gazebo GUI. If you're interested
in using all the Gazebo libraries, not only Gazebo GUI, check out this
[Gazebo installation](https://gazebosim.org/docs/latest/install).

We recommend following the binary install instructions to get up and running as
quickly and painlessly as possible.

The source install instructions should be used if you need the very latest
software improvements, if you need to modify the code, or if you plan to make a
contribution.

**Note:** If the version is 6 or downwards replace `gz-gui<#>` for `ign-gui<#>` in the command line. For more information take a look [here](https://community.gazebosim.org/t/a-new-era-for-gazebo/1356).

## Binary Install

### Ubuntu

On Ubuntu, it's possible to install Gazebo GUI as follows:

Add OSRF packages:
  ```
  echo "deb http://packages.osrfoundation.org/gazebo/ubuntu-stable `lsb_release -cs` main" > /etc/apt/sources.list.d/gazebo-stable.list
  sudo apt-key adv --keyserver hkp://p80.pool.sks-keyservers.net:80 --recv-keys D2486D2DD83DB69272AFE98867170598AF249743
  sudo apt update
  ```

Install Gazebo GUI:
  ```
  sudo apt install libgz-gui<#>-dev
  ```

Be sure to replace `<#>` with a number value, such as 1 or 2, depending on
which version you need.

### macOS

On macOS, add OSRF packages:
  ```
  ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
  brew tap osrf/simulation
  ```

Install Gazebo GUI:
  ```
  brew install gz-gui<#>
  ```

Be sure to replace `<#>` with a number value, such as 1 or 2, depending on
which version you need.

### Windows

Binary install is pending `gz-rendering` and `gz-gui` being added to conda-forge.

## Source Install

### Ubuntu Focal 20.04 or above

#### Install Prerequisites

Add OSRF packages:
  ```
  sudo apt update
  sudo apt -y install wget lsb-release gnupg
  sh -c 'echo "deb http://packages.osrfoundation.org/gazebo/ubuntu-stable `lsb_release -cs` main" > /etc/apt/sources.list.d/gazebo-stable.list'
  wget http://packages.osrfoundation.org/gazebo.key -O - | apt-key add -
  sudo apt-add-repository -s "deb http://packages.osrfoundation.org/gazebo/ubuntu-stable $(lsb_release -c -s) main"
  ```

Clone source code:
  ```
  # This checks out the default branch. You can append `-b ign-gui#` (replace # with a number) to checkout a specific version
  git clone http://github.com/gazebosim/gz-gui
  ```

Install dependencies
  ```
  sudo apt -y install $(sort -u $(find . -iname 'packages.apt') | tr '\n' ' ')
  ```

#### Build from source

Build and install as follows:
  ```
  cd gz-gui
  mkdir build
  cd build
  cmake ..
  make -j4
  sudo make install
  ```

### macOS

1. Clone the repository
  ```
  git clone https://github.com/gazebosim/gz-gui -b gz-gui<#>
  ```
  Be sure to replace `<#>` with a number value, such as 7 or 8, depending on
  which version you need.

2. Install dependencies
  ```
  brew install --only-dependencies gz-gui<#>
  ```
  Be sure to replace `<#>` with a number value, such as 7 or 8, depending on
  which version you need.

3. Configure and build
  ```
  cd gz-gui
  mkdir build
  cd build
  cmake ..
  make
  ```

4. Optionally, install
  ```
  sudo make install
  ```

### Windows

#### Install Prerequisites

First, follow the [gz-cmake](https://github.com/gazebosim/gz-cmake) tutorial for installing Conda, Visual Studio, CMake, etc., prerequisites, and creating a Conda environment.

Navigate to `condabin` if necessary to use the `conda` command (i.e., if Conda is not in your `PATH` environment variable. You can find the location of `condabin` in Anaconda Prompt, `where conda`).

Create if necessary, and activate a Conda environment:
```
conda create -n gz-ws
conda activate gz-ws
```

Install dependencies:
```
conda install qt --channel conda-forge
```

Install Gazebo dependencies, replacing `<#>` with the desired versions:
```
conda install libgz-cmake<#> libgz-common<#> libgz-math<#> libgz-transport<#> libgz-msgs<#> libgz-plugin<#> libgz-tools<#> --channel conda-forge
```

Before [gz-rendering](https://github.com/gazebosim/gz-rendering) becomes available on conda-forge, follow its tutorial to build it from source.

#### Build from source

1. Activate the Conda environment created in the prerequisites:
  ```
  conda activate gz-ws
  ```

1. Navigate to where you would like to build the library, and clone the repository.
  ```
  # Optionally, append `-b ign-gui#` (replace # with a number) to check out a specific version
  git clone https://github.com/gazebosim/gz-gui.git
  ```

1. Configure and build
  ```
  cd gz-gui
  mkdir build
  cd build
  ```

    As `gz-rendering` is not yet available on conda-forge, we need to build it from source and specify the path containing `gz-rendering<#>-config.cmake` and `gz-rendering<#>-ogre-config.cmake` in `CMAKE_PREFIX_PATH`. That path could be `gz-rendering-install-path\lib\cmake`, for example.
  ```
  cmake .. -DBUILD_TESTING=OFF -DCMAKE_PREFIX_PATH=path\containing\gz-rendering-config  # Optionally, -DCMAKE_INSTALL_PREFIX=path\to\install
  cmake --build . --config Release
  ```

1. Optionally, install. You will likely need to run a terminal with admin privileges for this call to succeed.
  ```
  cmake --install . --config Release
  ```

# Documentation

API documentation and tutorials can be accessed at
[https://gazebosim.org/libs/gui](https://gazebosim.org/libs/gui)

You can also generate the documentation from a clone of this repository by following these steps.

1. You will need [Doxygen](http://www.doxygen.org/). On Ubuntu Doxygen can be installed using
  ```
  sudo apt-get install doxygen
  ```

2. Clone the repository
  ```
  git clone https://github.com/gazebosim/gz-gui
  ```

3. Configure and build the documentation.
  ```
  cd gz-gui
  mkdir build
  cd build
  cmake ..
  make doc
  ```

4. View the documentation by running the following command from the `build` directory.
  ```
  firefox doxygen/html/index.html
  ```

# Testing

Follow these steps to run tests and static code analysis in your clone of this repository.

1. Follow the [source install instruction](#source-install).

2. Run tests.
  ```
  make test
  ```

3. Static code checker.
  ```
  make codecheck
  ```

See the [Writing Tests section of the contributor guide](https://gazebosim.org/docs/all/contributing#writing-tests) for help creating or modifying tests.
