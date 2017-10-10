[![codecov](https://codecov.io/bb/ignitionrobotics/ign-gui/branch/default/graph/badge.svg)](https://codecov.io/bb/ignitionrobotics/ign-gui/branch/default)

# Ignition gui

** Igntion gui classes and functions for robot applications.**

Ignition gui is a component in the ignition framework, a set
of libraries designed to rapidly develop robot applications.

  [http://ignitionrobotics.org](http://ignitionrobotics.org)

## Dependencies

### External

    sudo apt install -y build-essential wget cmake qtbase5-dev libtinyxml2-dev libfreeimage-dev libgts-dev uuid-dev libswscale-dev libavutil-dev libavcodec-dev libavformat-dev libprotoc-dev libprotobuf-dev libzmq3-dev protobuf-compiler cppcheck mercurial


### Ignition

    # Ignition math
    hg clone http://bitbucket.org/ignitionrobotics/ign-math -b ign-math3
    cd ign-math
    mkdir build
    cd build
    cmake .. -DENABLE_TESTS_COMPILATION=false
    make install
    cd ../..
    # Ignition common
    hg clone http://bitbucket.org/ignitionrobotics/ign-common -b default
    cd ign-common
    mkdir build
    cd build
    cmake .. -DENABLE_TESTS_COMPILATION=false
    make install
    cd ../..
    # Ignition msgs
    hg clone http://bitbucket.org/ignitionrobotics/ign-msgs -b default
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

## Documentation generation

    # TODO

## Plugins

### Finding plugins

Ignition GUI will look for plugins on the following paths, in this order:

1. all paths set on the `IGN_GUI_PLUGIN_PATH` environment variable
1. all paths added by calling `ignition::gui::addPluginPath`
1. `~/.ignition/gui/plugins`
1. plugins which come built into Ignition GUI

### Plugin configuration

Ignition GUI supports loading configuration files (XML) which are passed to
plugins and can be parsed using TinyXml2 (TODO: consider alternative to
Tinyxml).

For example (`examples/config/image.config`):

    <plugin filename="ImageDisplay">
      <title>Without picker</title>
      <topic_picker>false</topic_picker>
      <topic>/rendering/image</topic>
    </plugin>

* Developers can read custom plugin configurations overriding the
  `Plugin::LoadConfig` function.

* Ignition GUI processes a few default parameters before passing the config to
  implemented plugins. These are the parameters handled:

    * `<title>`: The title to be displayed on the plugin's title bar

    * `<has_titlebar>`: Set this to false to remove the title bar

### Built-in plugins

#### Image display

Display images from an Ignition Transport topic.

    ign gui -c examples/config/image.config

or

    ign gui -s ImageDisplay

#### Publisher

Publish messages on an Ignition Transport topic.

1. Listen to string messages on the `/echo` topic:

        ign topic -e -t /echo

1. On a new terminal, open the publisher GUI:

        ign gui -s Publisher

1. The GUI is pre-filled to publish "Hello" messages on the `/echo` topic.
   Click "Publish" to publish a message and see it on the terminal.

#### Time panel

* Display sim / real time published on a topic
* Buttons to make play / pause service requests

        ign gui -c examples/config/time.config

#### Topic echo

Echo messages from an Ignition Transport topic.

    ign gui -c examples/config/pubsub.config

#### Topic viewer

Discover all Ignition Transport topics and introspect its fields.

    ign gui -c examples/config/topicViewer.config

#### Topics stats

Show basic stats of all Ignition Transport topics at the same time.

    ign gui -c examples/config/topicsStats.config

#### Requester and Responder

Make custom service requests and serve custom services with these two
plugins.

    ign gui -c examples/config/reqres.config

## Layout

It's possible to define the layout of various plugins loaded to the main window
by adding a `<window>` element to the config file. The child elements are:

* `<position_x>`: Horizontal position of window's top-left corner in pixels.
* `<position_y>`: Vertical position of window's top-left corner in pixels.
* `<width>`: Window's width in pixels
* `<height>`: Window's height in pixels
* `<state>`: The state of the widow's docks and tabs, described as a Base64
             encoded `QByteArray`. More information
             [here](http://doc.qt.io/qt-5/qmainwindow.html#saveState).
* `<menus>`: Configure menu options
    * `<file>`: File menu configuration.
        * `visible`: Set to false to hide the whole File menu.
    * `<plugins>`: Plugins menu configuration.
        * `visible`: Set to false to hide the whole Plugins menu.
        * `from_paths`: Set to false to prevent filling the menu with all
                        plugins found on the paths. In that case, if no plugins
                        are given with `<show>` elements, the plugins menu will
                        be empty.
        * `<show>`: Add a show element for each plugin which should be shown on
                    the menu. If `from_paths` is true, all plugins will be shown
                    anyway, so adding `<show>` has no effect. For the plugin to
                    be shown, it must be on the path.

### Example layout

Try an example layout:

    ign gui -c examples/config/layout.config

Compare it to the same plugins loaded without a layout:

    ign gui -c examples/config/no_layout.config

### Create a new layout

You can start from an empty window and insert plugins, or from an existing
config file. For example:

1. Open a config file loading several plugins, but without window
   configurations, for example:

        ign gui -c examples/config/no_layout.config

1. Drag the widgets as you wish.

1. Click on `File->Save configuration as` and save on a new file.

    > You can also click on `File -> Save configuration` to save that as the
      default configuration which is loaded whenever Ignition GUI is opened
      without a config file.

1. Close the window, then load the new config file:

        ign gui -c <path to saved file>

You can also load the new config file from `File->Load configuration`.

## Style

Ignition GUI comes with a reference style based on [material design](https://material.io/).

There are different ways to use custom [QSS](http://doc.qt.io/qt-5/stylesheet-syntax.html) stylesheets:

### Using cmake

Take a look at the
[style_cmake example](https://bitbucket.org/ignitionrobotics/ign-gui/src/default/examples/standalone/style_cmake/)
to see how downstream projects can load their own style by setting it in cmake.

### Command line

You can pass a stylesheet file on the command line with the `-t` option. Try:

    ign gui -s Publisher -t examples/standalone/style_cmake/style.qss

> Note: when loading a config file which has a stylesheet, this option will not
  override it.

### Through the GUI

At any time from the main window, you can choose `File -> Load stylesheet` and
choose a QSS file on the fly.

### Config files

You can embed your stylesheet inside the `<window><stylesheet>` tag inside your
configuration file. See an example:

    ign gui -c examples/config/stylesheet.config

>Note: When saving the configuration file through the GUI, the current stylesheet will
be added.

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

      -t [ --style ] arg         Apply a stylesheet to the whole application.
                                 Give a QSS file path as an argument.

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

First, build an example plugin:

    cd examples/plugin/hello_plugin
    mkdir build
    cd build
    cmake ..
    make

Then copy the generated library to a directory where Ignition GUI can find it:

    cp libhello_plugin.so ~/.ignition/gui/plugins/

Now you can use the command line to open it:

    ign gui -s hello_plugin

#### Example plugins

* `hello_plugin`: A button that prints hello on click.
* `custom_context_menu`: Overrides the default context menu.
* `designer_ui_file`: Use a Qt Designer UI file for a plugin's appearance.

### Loading a configuration file

It's possible to use the command line to load a configuration file specifying
plugins to load.

Example configuration files are provided in the `/examples/config` folder.

To run the example `hello.config`:

1. First, build the `hello_plugin` above.

1. Then, load the GUI from the command line with the configuration file:

        ign gui -c examples/config/hello.config

