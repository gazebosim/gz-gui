# Ignition GUI : Graphical interfaces for robotics applications

**Maintainer:** louise [AT] openrobotics [DOT] org

[![GitHub open issues](https://img.shields.io/github/issues-raw/ignitionrobotics/ign-gui.svg)](https://github.com/ignitionrobotics/ign-gui/issues)
[![GitHub open pull requests](https://img.shields.io/github/issues-pr-raw/ignitionrobotics/ign-gui.svg)](https://github.com/ignitionrobotics/ign-gui/pulls)
[![Discourse topics](https://img.shields.io/discourse/https/community.gazebosim.org/topics.svg)](https://community.gazebosim.org)
[![Hex.pm](https://img.shields.io/hexpm/l/plug.svg)](https://www.apache.org/licenses/LICENSE-2.0)

Build | Status
-- | --
Test coverage | [![codecov](https://codecov.io/gh/ignitionrobotics/ign-gui/branch/ign-gui3/graph/badge.svg)](https://codecov.io/gh/ignitionrobotics/ign-gui/branch/ign-gui3)
Ubuntu Bionic | [![Build Status](https://build.osrfoundation.org/buildStatus/icon?job=ignition_gui-ci-ign-gui3-bionic-amd64)](https://build.osrfoundation.org/job/ignition_gui-ci-ign-gui3-bionic-amd64)
Homebrew      | [![Build Status](https://build.osrfoundation.org/buildStatus/icon?job=ignition_gui-ci-ign-gui3-homebrew-amd64)](https://build.osrfoundation.org/job/ignition_gui-ci-ign-gui3-homebrew-amd64)
Windows       | [![Build Status](https://build.osrfoundation.org/buildStatus/icon?job=ign_gui-ign-3-win)](https://build.osrfoundation.org/job/ign_gui-ign-3-win)

Ignition GUI builds on top of [Qt](https://www.qt.io/) to provide widgets which are
useful when developing robotics applications, such as a 3D view, plots, dashboard, etc,
and can be used together in a convenient unified interface.

Ignition GUI ships with several widgets ready to use and offers a plugin interface
which can be used to add custom widgets.

# Table of Contents

[Features](#markdown-header-features)

[Install](#markdown-header-install)

[Usage](#markdown-header-usage)

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

See [the install tutorial](https://ignitionrobotics.org/api/gui/3.0/install.html).

# Usage

Take a look at the
[tutorials](https://ignitionrobotics.org/api/gui/3.0/tutorials.html)
for usage instructions and API documentation.

## Known issue of command line tools

In the event that the installation is a mix of Debian and from source, command
line tools from `ign-tools` may not work correctly.

A workaround for a single package is to define the environment variable
`IGN_CONFIG_PATH` to point to the location of the Ignition library installation,
where the YAML file for the package is found, such as
```
export IGN_CONFIG_PATH=/usr/local/share/ignition
```

However, that environment variable only takes a single path, which means if the
installations from source are in different locations, only one can be specified.

Another workaround for working with multiple Ignition libraries on the command
line is using symbolic links to each library's YAML file.
```
mkdir ~/.ignition/tools/configs -p
cd ~/.ignition/tools/configs/
ln -s /usr/local/share/ignition/fuel4.yaml .
ln -s /usr/local/share/ignition/transport7.yaml .
ln -s /usr/local/share/ignition/transportlog7.yaml .
...
export IGN_CONFIG_PATH=$HOME/.ignition/tools/configs
```

This issue is tracked [here](https://github.com/ignitionrobotics/ign-tools/issues/8/too-strict-looking-for-config-paths).

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
[CODE\_OF\_CONDUCT.md](https://github.com/ignitionrobotics/ign-gazebo/blob/main/CODE_OF_CONDUCT.md).

# Versioning

This library uses [Semantic Versioning](https://semver.org/). Additionally, this library is part of the [Ignition Robotics project](https://ignitionrobotics.org) which periodically releases a versioned set of compatible and complimentary libraries. See the [Ignition Robotics website](https://ignitionrobotics.org) for version and release information.

# License

This library is licensed under [Apache 2.0](https://www.apache.org/licenses/LICENSE-2.0). See also the [LICENSE](https://github.com/ignitionrobotics/ign-gui/blob/main/LICENSE) file.
