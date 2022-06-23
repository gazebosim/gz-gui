\page layout Layout

Next Tutorial: \ref style
Previous Tutorial: \ref plugins

## Overview

It's possible to define the layout of various plugins loaded to the main window
by adding a `<window>` element to the config file. The child elements are:

* `<width>`: Window's width in pixels
* `<height>`: Window's height in pixels
* `<menus>`: Configure menu options
    * `<drawer>`: Side drawer configuration.
        * `visible`: Set to false to hide the drawer and the button to trigger it.
    * `<plugins>`: Plugins menu configuration.
        * `visible`: Set to false to hide the plugins menu and the button to trigger it.
        * `from_paths`: Set to false to prevent filling the menu with all
                        plugins found on the paths. In that case, if no plugins
                        are given with `<show>` elements, the plugins menu will
                        be empty.
        * `<show>`: Add a show element for each plugin which should be shown on
                    the menu. If `from_paths` is true, all plugins will be shown
                    anyway, so adding `<show>` has no effect. For the plugin to
                    be shown, it must be on the path.
* `<default_exit_action>`: Default `CLOSE_GUI`. If set to `SHUTDOWN_SERVER` and
                           `<dialog_on_exit>` is `false`, closing the window will
                           emit a server shutdown request with `stop = true` to the
                           `<server_control_service>` topic. This can be used
                           in applications like Gazebo which can run a
                           server in a process separate from the GUI to stop both
                           the GUI and the server when the window is closed. The value is
                           case-insensitive.
* `<server_control_service>`: Default `/server_control`. This is the name of `msgs::ServerControl`
                              service that allows e.g. stopping the server. It is usually not needed
                              to alter this value.
* `<dialog_on_exit>`: If `true`, a confirmation dialog will show up when closing the window.
* `<dialog_on_exit_options>`: Configuration of the dialog shown before exit (with all elements
                              optional).
    * `<prompt_text>`: Text of the prompt in the confirmation dialog.
    * `<show_shutdown_button>`: Default `false`. If `true`, display a "Shutdown simulation"
                                button in the confirmation dialog, which shuts down the server, too.
                                Always set `<close_gui_button_text>` to a different string than "OK"
                                if both close GUI and shutdown buttons are shown, otherwise there
                                would be a dialog with options "OK", "Cancel" and "shutdown", which
                                is bad UX.
    * `<shutdown_button_text>`: Text of the "Shutdown simulation" button. If empty, a default text
                                is used.
    * `<show_close_gui_button>`: Default `true`. If `true`, display a "Close GUI" button in
                                 the confirmation dialog, which leaves server running.
    * `<close_gui_button_text>`: Text of the "Close GUI" button. If empty, a default text is used.
                                 When both shutdown and close GUI buttons are shown, always change
                                 the text of the close GUI button, otherwise there would be a dialog
                                 with options "OK", "Cancel" and "shutdown", which is bad UX.

## Example layout

Try an example layout:

    gz gui -c examples/config/layout.config

Compare it to the same plugins loaded without a layout:

    gz gui -c examples/config/no_layout.config

## Create a new layout

You can start from an empty window and insert plugins, or from an existing
config file. For example:

1. Open a config file loading several plugins, but without window
   configurations, for example:

        gz gui -c examples/config/no_layout.config

1. Drag the widgets as you wish.

1. Click on `File->Save configuration as` and save on a new file.

    > You can also click on `File -> Save configuration` to save that as the
      default configuration which is loaded whenever Gazebo GUI is opened
      without a config file.

1. Close the window, then load the new config file:

        gz gui -c <path to saved file>

You can also load the new config file from `File->Load configuration`.
