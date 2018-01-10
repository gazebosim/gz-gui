Style {#style}
=====

Ignition GUI comes with a reference style based on [material design](https://material.io/).

There are different ways to use custom [QSS](http://doc.qt.io/qt-5/stylesheet-syntax.html) stylesheets:

## Using cmake

Take a look at the
[style_cmake example](https://bitbucket.org/ignitionrobotics/ign-gui/src/default/examples/standalone/style_cmake/)
to see how downstream projects can load their own style by setting it in cmake.

## Command line

You can pass a stylesheet file on the command line with the `-t` option. Try:

    ign gui -s Publisher -t examples/standalone/style_cmake/style.qss

> Note: when loading a config file which has a stylesheet, this option will not override it.

## Through the GUI

At any time from the main window, you can choose `File -> Load stylesheet` and
choose a QSS file on the fly.

## Config files

You can embed your stylesheet inside the `<window><stylesheet>` tag inside your
configuration file. See an example:

    ign gui -c examples/config/stylesheet.config

> Note: When saving the configuration file through the GUI, the current stylesheet will be saved as well.
