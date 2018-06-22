Style {#style}
=====

Ignition GUI leverages [QtQuick Controls 2 Styles](https://doc.qt.io/qt-5.9/qtquickcontrols2-styles.html) for styling. The recommended and best supported style is the Material Style, but it is also possible to use others such as Default and Universal.

The default style is hardcoded into the
[qtquickcontrols2.conf](https://bitbucket.org/ignitionrobotics/ign-gui/src/gz11/include/ignition/gui/qtquickcontrols2.conf)
file.

There are a few ways to override the default style:

* Through the graphical interface (GUI)
* Setting environment variables (env)
* Through Ignition GUI config files (config)

If styles are set from multiple sources, this is the order in which they take precendence:

GUI > config > env

## GUI

To change the style at runtime using the graphical interface, on the left menu,
choose "Style settings".

A dialog will open, where you can change the theme (Light / Dark) and primary /
accent colors. From the color dropdown menu, it is possible to choose one of the
[pre-defined material colors](https://doc.qt.io/qt-5.9/qtquickcontrols2-material.html#pre-defined-material-colors),
and from the button next to it, it is possible to choose any custom color.

> **Note**: Custom colors won't be automatically shaded based on the theme.

## Environment variables

See
[Supported Environment Variables in Qt Quick Controls 2](https://doc.qt.io/qt-5.9/qtquickcontrols2-environment.html).

You can try running the following command for example:

    QT_QUICK_CONTROLS_MATERIAL_THEME=Dark QT_QUICK_CONTROLS_MATERIAL_PRIMARY=Blue QT_QUICK_CONTROLS_MATERIAL_ACCENT=Indigo ign gui -v 4

## Config files

Styles can be set on a configuration file's `<window><style>` tag. The supported
attributes are:

* `material_theme`
* `material_primary`
* `material_accent`

You can try an example:

    ign gui -c examples/config/style.config

> **Note**: When saving the configuration file through the GUI, the current style
will be saved as well.

