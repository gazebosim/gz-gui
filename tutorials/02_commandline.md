\page commandline Command Line

Next Tutorial: \ref plugins
Previous Tutorial: \ref install

## Overview

If you have Gazebo Tools installed, you can use the `gz gui` command line tool:

`gz gui -h`

    Gazebo GUI tool.

      gz gui [options]

    Options:

      -l [ --list ]              List all available plugins.

      -s [ --standalone ] arg    Run a plugin as a standalone window.
                                 Give the plugin filename as an argument.

      -c [ --config ] arg        Open the main window with a configuration file.
                                 Give the configuration file path as an argument.

      -v [ --verbose ] [arg]     Adjust the level of console output (0~4).
                                 The default verbosity is 1, use -v without
                                 arguments for level 3.

      -h [ --help ]              Print this help message.

      --force-version <VERSION>  Use a specific library version.

      --versions                 Show the available versions.

When using the command line tool, all console messages are logged to
`$HOME/.gz/gui/log/<timestamp>`.
