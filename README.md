# Ignition GUI : Graphical interfaces for robotics applications

**Maintainer:** louise [AT] openrobotics [DOT] org

[![GitHub open issues](https://img.shields.io/github/issues-raw/ignitionrobotics/ign-gui.svg)](https://github.com/ignitionrobotics/ign-gui/issues)
[![GitHub open pull requests](https://img.shields.io/github/issues-pr-raw/ignitionrobotics/ign-gui.svg)](https://github.com/ignitionrobotics/ign-gui/pulls)
[![Discourse topics](https://img.shields.io/discourse/https/community.gazebosim.org/topics.svg)](https://community.gazebosim.org)
[![Hex.pm](https://img.shields.io/hexpm/l/plug.svg)](https://www.apache.org/licenses/LICENSE-2.0)

Build | Status
-- | --
Test coverage | [![codecov](https://codecov.io/gh/ignitionrobotics/ign-gui/branch/master/graph/badge.svg)](https://codecov.io/gh/ignitionrobotics/ign-gui/branch/default)
Ubuntu Bionic | [![Build Status](https://build.osrfoundation.org/buildStatus/icon?job=ignition_gui-ci-master-bionic-amd64)](https://build.osrfoundation.org/job/ignition_gui-ci-master-bionic-amd64)
Homebrew      | [![Build Status](https://build.osrfoundation.org/buildStatus/icon?job=ignition_gui-ci-master-homebrew-amd64)](https://build.osrfoundation.org/job/ignition_gui-ci-master-homebrew-amd64)
Windows 7     | [![Build Status](https://build.osrfoundation.org/buildStatus/icon?job=ign_gui-ign-1-win)](https://build.osrfoundation.org/job/ign_gui-ign-1-win)

Ignition GUI builds on top of [Qt](https://www.qt.io/) to provide widgets which are
useful when developing robotics applications, such as a 3D view, plots, dashboard, etc,
and can be used together in a convenient unified interface.

Ignition GUI ships with several widgets ready to use and offers a plugin interface
which can be used to add custom widgets.

# Table of Contents

[Features](#markdown-header-features)

[Install](#markdown-header-install)

* [Binary Install](#markdown-header-binary-install)

* [Source Install](#markdown-header-source-install)

    * [Prerequisites](#markdown-header-prerequisites)

    * [Building from Source](#markdown-header-building-from-source)

[Usage](#markdown-header-usage)

[Documentation](#markdown-header-documentation)

[Testing](#markdown-header-testing)

[Folder Structure](#markdown-header-folder-structure)

[Code of Conduct](#markdown-header-code-of-conduct)

[Contributing](#markdown-header-code-of-contributing)

[Versioning](#markdown-header-versioning)

[License](#markdown-header-license)

# Features

* Qt-based widgets, with support for both Qt5 widgets and QtQuick
* Plugin-based interface, so it's easy to add new widgets
* Several plugins ready to be used
* 3D scene integration using [Ignition Rendering](https://github.com/ignitionrobotics/ign-rendering/)

# Install

We recommend following the [Binary Install](#markdown-header-binary-install) instructions to get up and running as quickly and painlessly as possible.

The [Source Install](#markdown-header-source-install) instructions should be used if you need the very latest software improvements, you need to modify the code, or you plan to make a contribution.

## Binary Install

On Ubuntu Bionic, it's possible to install Ignition GUI's version 1 as follows:

Add OSRF packages:

    echo "deb http://packages.osrfoundation.org/gazebo/ubuntu-stable bionic main" > /etc/apt/sources.list.d/gazebo-stable.list
    sudo apt-key adv --keyserver hkp://p80.pool.sks-keyservers.net:80 --recv-keys D2486D2DD83DB69272AFE98867170598AF249743
    sudo apt update

Install Ignition GUI:

    sudo apt install libignition-gui-dev

## Source Install

There are currently two versions under active development:

* **Version 0**: It makes use of `QWidgets`, which are common on traditional
  desktop-oriented interfaces. Requires Ubuntu Xenial or Higher.

* **Version 1**: It makes use of `QtQuick`, which allows developing more
  modern-looking apps with animations and a mobile/web feel. Requires Ubuntu
  Bionic or higher.

### Prerequisites

#### Version 0

Install dependencies:

    sudo apt update
    sudo apt -y install wget lsb-release gnupg
    sh -c 'echo "deb http://packages.osrfoundation.org/gazebo/ubuntu-stable `lsb_release -cs` main" > /etc/apt/sources.list.d/gazebo-stable.list'
    sh -c 'echo "deb http://packages.osrfoundation.org/gazebo/ubuntu-prerelease `lsb_release -cs` main" > /etc/apt/sources.list.d/gazebo-prerelease.list'
    wget http://packages.osrfoundation.org/gazebo.key -O - | apt-key add -
    sudo apt update
    sudo apt install -y \
        build-essential \
        cmake \
        qtbase5-dev \
        libtinyxml2-dev \
        libprotoc-dev \
        libprotobuf-dev \
        mercurial \
        libignition-cmake1-dev \
        libignition-common-dev \
        libignition-math4-dev \
        libignition-msgs-dev \
        libignition-rendering-dev \
        libignition-tools-dev \
        libignition-transport4-dev

Clone source code, note you'll need the `default` branch:

    git clone http://github.com/ignitionrobotics/ign-gui -b master

#### Version 1

Install dependencies:

    sudo apt update
    sudo apt -y install wget lsb-release gnupg
    sh -c 'echo "deb http://packages.osrfoundation.org/gazebo/ubuntu-stable `lsb_release -cs` main" > /etc/apt/sources.list.d/gazebo-stable.list'
    sh -c 'echo "deb http://packages.osrfoundation.org/gazebo/ubuntu-prerelease `lsb_release -cs` main" > /etc/apt/sources.list.d/gazebo-prerelease.list'
    wget http://packages.osrfoundation.org/gazebo.key -O - | apt-key add -
    sudo apt update
    sudo apt install -y \
        g++-8 \
        libignition-cmake2-dev \
        libignition-common3-dev \
        libignition-math6-dev \
        libignition-msgs3-dev \
        libignition-plugin-dev \
        libignition-rendering-dev \
        libignition-tools-dev \
        libignition-transport6-dev \
        libtinyxml2-dev \
        qtbase5-dev \
        qtdeclarative5-dev \
        qtquickcontrols2-5-dev \
        qml-module-qtquick2 \
        qml-module-qtquick-controls \
        qml-module-qtquick-controls2 \
        qml-module-qtquick-dialogs \
        qml-module-qtquick-layouts \
        qml-module-qt-labs-folderlistmodel \
        qml-module-qt-labs-settings
    sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-8 800 --slave /usr/bin/g++ g++ /usr/bin/g++-8 --slave /usr/bin/gcov gcov /usr/bin/gcov-8

Clone source code, note you'll need the `ign-gui1` branch:

    git clone http://github.com/ignitionrobotics/ign-gui -b ign-gui1

### Building from source

Build and install as follows:

    cd ign-gui
    mkdir build
    cd build
    cmake ..
    make -j4
    sudo make install

# Usage

Take a look at the
[tutorials](https://ignitionrobotics.org/api/gui/1.0/tutorials.html)
for usage instructions and API documentation.


# Testing

After building, to run all tests:

    cd build
    make test

See [this tutorial](https://ignitionrobotics.org/api/gui/1.0/install.html) for
further instructions on how to disable testing, run test coverage, etc.

# Folder Structure

* `include/ignition/gui`: Contains all the files which will be installed, such as
  C++ headers, `.qrc`, `.qss` and `qml` files. Plugin-specific resources go under
  `/plugins`; everything in the top-level is meant to be of common use across
  plugins.

* `src`: Contains all the C++ source code and headers which are not installed.
  Plugin-specific code goes under `/plugins`.

* `test`: All integration, performance and regression tests go here, under their
  specific folders.

* `examples/standalone`: Example code for standalone applications using Ignition GUI
  as a library. Each example has instructions in a README file.

* `examples/config`: Example configuration files which can be loaded using
  `ign gui -c <path to file>`

* `examples/plugin`: Example plugins which can be compiled and loaded as explained
  in [this tutorial](https://ignitionrobotics.org/api/gui/1.0/plugins.html).

* `tutorials`: Markdown files for the [tutorials](https://ignitionrobotics.org/api/gui/1.0/tutorials.html).

* `conf`: Files needed by [ign-tools](https://github.com/ignitionrobotics/ign-tools).

* `doc`: Files used by Doxygen when generating documentation.

# Contributing

Please see
[CONTRIBUTING.md](https://ignitionrobotics.org/docs/all/contributing).

# Code of Conduct

Please see
[CODE\_OF\_CONDUCT.md](https://github.com/ignitionrobotics/ign-gazebo/blob/master/CODE_OF_CONDUCT.md).

# Versioning

This library uses [Semantic Versioning](https://semver.org/). Additionally, this library is part of the [Ignition Robotics project](https://ignitionrobotics.org) which periodically releases a versioned set of compatible and complimentary libraries. See the [Ignition Robotics website](https://ignitionrobotics.org) for version and release information.

# License

This library is licensed under [Apache 2.0](https://www.apache.org/licenses/LICENSE-2.0). See also the [LICENSE](https://github.com/ignitionrobotics/ign-gui/blob/master/LICENSE) file.

