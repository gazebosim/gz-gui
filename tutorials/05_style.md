\page style Style

Next Tutorial: \ref examples
Previous Tutorial: \ref layout

## Overview

Ignition GUI leverages
[QtQuick Controls 2 Styles](https://doc.qt.io/qt-5.9/qtquickcontrols2-styles.html)
for styling. The recommended and best supported style is the Material Style,
but it is also possible to use others such as Default and Universal. This tutorial
focuses on customizing the Material style.

The default style is hardcoded into the
[qtquickcontrols2.conf](https://github.com/ignitionrobotics/ign-gui/blob/master/include/ignition/gui/qtquickcontrols2.conf)
file.

There are a few ways to override the default style:

* Through the graphical interface (GUI)
* Setting environment variables (env)
* Through Ignition GUI config files (config)

If styles are set from multiple sources, this is the order in which they take precendence:

**GUI > config > env**

## Variables

Users can customize the whole application's material style using just a few variables:

* Theme (Light / Dark)
* Primary color
* Accent color

QML types provided by
[QtQuick Controls 2](https://doc.qt.io/qt-5/qtquick-controls2-qmlmodule.html),
as well as QML types provided by Ignition GUI, use these variables as appropriate.
It's recommended that developers make sure their plugins use these variables
for a more integrated experience.

### Top toolbar

On top of those, Ignition GUI also offers a few variables specific to customizing
the top toolbar.

By default, the top toolbar will use the primary color, while the plugin toolbars will
use the accent color, according to the current theme. Users can override this behaviour
and decouple the toolbar colors from the rest of the controls with the following
variables:

* Top toolbar color, light theme
* Top toolbar color, dark theme
* Top toolbar text color, light theme
* Top toolbar text color, dark theme
* Plugin toolbar color, light theme
* Plugin toolbar color, dark theme
* Plugin toolbar text color, light theme
* Plugin toolbar text color, dark theme

## GUI

To change the style at runtime using the graphical interface, on the left menu,
choose "Style settings".

A dialog will open, where you can change the theme (Light / Dark) and primary /
accent colors. From the color dropdown menu, it is possible to choose one of the
[pre-defined material colors](https://doc.qt.io/qt-5.9/qtquickcontrols2-material.html#pre-defined-material-colors),
and from the button next to it, it is possible to choose any custom color.

\note Custom colors won't be automatically shaded based on the theme.

\note Toolbar colors can't be changed through the GUI yet.

## Environment variables

See
[Supported Environment Variables in Qt Quick Controls 2](https://doc.qt.io/qt-5.9/qtquickcontrols2-environment.html).

You can try running the following command for example:

    QT_QUICK_CONTROLS_MATERIAL_THEME=Dark QT_QUICK_CONTROLS_MATERIAL_PRIMARY=Blue ign gui -v 4

\note Any previously saved default configuration (usually stored in ~/.ignition/gui/default.config) will override
these environment variable preferences.

\note Toolbar colors can't be changed through environment variables yet.

\note Applications built on top of Ignition GUI may override custom environment variables.

## Config files

Styles can be set on a configuration file's `<window><style>` tag, using the following
attributes:

* `material_theme`
* `material_primary`
* `material_accent`
* `toolbar_color_light`
* `toolbar_text_color_light`
* `toolbar_color_dark`
* `toolbar_text_color_dark`
* `plugin_toolbar_color_light`
* `plugin_toolbar_text_color_light`
* `plugin_toolbar_color_dark`
* `plugin_toolbar_text_color_dark`

You can try an example just with the `material_` variables:

    ign gui -c examples/config/style.config

And an example also configuring the toolbars:

    ign gui -c examples/config/style_toolbars.config

\note When saving the configuration file through the GUI, the current style
will be saved as well.

