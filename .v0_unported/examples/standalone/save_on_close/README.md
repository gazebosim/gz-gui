# Standalone example: Save on close

This example shows how a 3rd party application can automatically save the
window configuration to a file on disk when the main window is closed.

## How to test it

Build:

    mkdir build
    cd build
    cmake ..
    make

Run the example:

    ./save_on_close

This will open an empty window. Now you can insert plugins, reposition
them, change the style, resize the window and so on.

Then close the window. On the console, you'll see that the configuration was
saved to the default location, for example:

    [GUI] [Msg] Saved configuration [/home/louise/.ignition/gui/default.config]

Now the next time you run the example, the window will be configured the way
you arranged it before.

    ./save_on_close

In case you don't change the default path, this configuration is also used if
you run Ignition GUI:

    ign gui

