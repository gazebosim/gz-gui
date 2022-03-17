\page install Installation

Next Tutorial: \ref commandline

# Install

These instructions are for installing only Ignition GUI. If you're interested
in using all the Ignition libraries, not only Igniton GUI, check out this
[Ignition installation](https://ignitionrobotics.org/docs/latest/install).

We recommend following the binary install instructions to get up and running as
quickly and painlessly as possible.

The source install instructions should be used if you need the very latest
software improvements, if you need to modify the code, or if you plan to make a
contribution.

## Binary Install

### Ubuntu

On Ubuntu, it's possible to install Ignition GUI as follows:

Add OSRF packages:
  ```
  echo "deb http://packages.osrfoundation.org/gazebo/ubuntu-stable `lsb_release -cs` main" > /etc/apt/sources.list.d/gazebo-stable.list
  sudo apt-key adv --keyserver hkp://p80.pool.sks-keyservers.net:80 --recv-keys D2486D2DD83DB69272AFE98867170598AF249743
  sudo apt update
  ```

Install Ignition GUI:
  ```
  sudo apt install libignition-gui<#>-dev
  ```

Be sure to replace `<#>` with a number value, such as 1 or 2, depending on
which version you need.

### macOS

On macOS, add OSRF packages:
  ```
  ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
  brew tap osrf/simulation
  ```

Install Ignition GUI:
  ```
  brew install ignition-gui<#>
  ```

Be sure to replace `<#>` with a number value, such as 1 or 2, depending on
which version you need.

### Windows

Binary install is pending `ignition-rendering` and `ignition-gui` being added to conda-forge.

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
  git clone http://github.com/ignitionrobotics/ign-gui
  ```

Install dependencies
  ```
  sudo apt -y install $(sort -u $(find . -iname 'packages.apt') | tr '\n' ' ')
  ```

#### Build from source

Build and install as follows:
  ```
  cd ign-gui
  mkdir build
  cd build
  cmake ..
  make -j4
  sudo make install
  ```

### macOS

1. Clone the repository
  ```
  git clone https://github.com/ignitionrobotics/ign-gui -b ign-gui<#>
  ```
  Be sure to replace `<#>` with a number value, such as 5 or 6, depending on
  which version you need.

2. Install dependencies
  ```
  brew install --only-dependencies ignition-gui<#>
  ```
  Be sure to replace `<#>` with a number value, such as 5 or 6, depending on
  which version you need.

3. Configure and build
  ```
  cd ign-gui
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

First, follow the [ign-cmake](https://github.com/ignitionrobotics/ign-cmake) tutorial for installing Conda, Visual Studio, CMake, etc., prerequisites, and creating a Conda environment.

Navigate to `condabin` if necessary to use the `conda` command (i.e., if Conda is not in your `PATH` environment variable. You can find the location of `condabin` in Anaconda Prompt, `where conda`).

Create if necessary, and activate a Conda environment:
```
conda create -n ign-ws
conda activate ign-ws
```

Install dependencies:
```
conda install qt --channel conda-forge
```

Install Ignition dependencies, replacing `<#>` with the desired versions:
```
conda install libignition-cmake<#> libignition-common<#> libignition-math<#> libignition-transport<#> libignition-msgs<#> libignition-plugin<#> libignition-tools<#> --channel conda-forge
```

Before [ign-rendering](https://github.com/ignitionrobotics/ign-rendering) becomes available on conda-forge, follow its tutorial to build it from source.

#### Build from source

1. Activate the Conda environment created in the prerequisites:
  ```
  conda activate ign-ws
  ```

1. Navigate to where you would like to build the library, and clone the repository.
  ```
  # Optionally, append `-b ign-gui#` (replace # with a number) to check out a specific version
  git clone https://github.com/ignitionrobotics/ign-gui.git
  ```

1. Configure and build
  ```
  cd ign-gui
  mkdir build
  cd build
  ```

    As `ign-rendering` is not yet available on conda-forge, we need to build it from source and specify the path containing `ignition-rendering<#>-config.cmake` and `ignition-rendering<#>-ogre-config.cmake` in `CMAKE_PREFIX_PATH`. That path could be `ign-rendering-install-path\lib\cmake`, for example.
  ```
  cmake .. -DBUILD_TESTING=OFF -DCMAKE_PREFIX_PATH=path\containing\ignition-rendering-config  # Optionally, -DCMAKE_INSTALL_PREFIX=path\to\install
  cmake --build . --config Release
  ```

1. Optionally, install. You will likely need to run a terminal with admin privileges for this call to succeed.
  ```
  cmake --install . --config Release
  ```

# Documentation

API documentation and tutorials can be accessed at
[https://ignitionrobotics.org/libs/gui](https://ignitionrobotics.org/libs/gui)

You can also generate the documentation from a clone of this repository by following these steps.

1. You will need [Doxygen](http://www.doxygen.org/). On Ubuntu Doxygen can be installed using
  ```
  sudo apt-get install doxygen
  ```

2. Clone the repository
  ```
  git clone https://github.com/ignitionrobotics/ign-gui
  ```

3. Configure and build the documentation.
  ```
  cd ign-gui
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

See the [Writing Tests section of the contributor guide](https://ignitionrobotics.org/docs/all/contributing#writing-tests) for help creating or modifying tests.
