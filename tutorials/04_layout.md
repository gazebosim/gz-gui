Layout {#layout}
======

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

## Example layout

Try an example layout:

    ign gui -c examples/config/layout.config

Compare it to the same plugins loaded without a layout:

    ign gui -c examples/config/no_layout.config

## Create a new layout

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
