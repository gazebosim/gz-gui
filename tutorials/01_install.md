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

    echo "deb http://packages.osrfoundation.org/gazebo/ubuntu-stable `lsb_release -cs` main" > /etc/apt/sources.list.d/gazebo-stable.list
    sudo apt-key adv --keyserver hkp://p80.pool.sks-keyservers.net:80 --recv-keys D2486D2DD83DB69272AFE98867170598AF249743
    sudo apt update

Install Ignition GUI:

    sudo apt install libignition-gui<#>-dev

Be sure to replace `<#>` with a number value, such as 1 or 2, depending on
which version you need.

### macOS

On macOS, add OSRF packages:

    ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
    brew tap osrf/simulation

Install Ignition GUI:

    brew install ignition-gui<#>

Be sure to replace `<#>` with a number value, such as 1 or 2, depending on
which version you need.

## Source Install

### Prerequisites

#### Ubuntu Bionic 18.04 or above

Add OSRF packages:

    sudo apt update
    sudo apt -y install wget lsb-release gnupg
    sh -c 'echo "deb http://packages.osrfoundation.org/gazebo/ubuntu-stable `lsb_release -cs` main" > /etc/apt/sources.list.d/gazebo-stable.list'
    wget http://packages.osrfoundation.org/gazebo.key -O - | apt-key add -
    sudo apt-add-repository -s "deb http://packages.osrfoundation.org/gazebo/ubuntu-stable $(lsb_release -c -s) main"

Clone source code:

    # This checks out the `master` branch. You can append `-b ign-gui#` (replace # with a number) to checkout a specific version
    git clone http://github.com/ignitionrobotics/ign-gui

Install dependencies

   sudo apt -y install $(sort -u $(find . -iname 'packages.apt') | tr '\n' ' ')

Only on Bionic, update the GCC compiler version:

    sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-8 800 --slave /usr/bin/g++ g++ /usr/bin/g++-8 --slave /usr/bin/gcov gcov /usr/bin/gcov-8


### Building from source

Build and install as follows:

    cd ign-gui
    mkdir build
    cd build
    cmake ..
    make -j4
    sudo make install

