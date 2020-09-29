\page commandline Command Line

Next Tutorial: \ref plugins
Previous Tutorial: \ref install

## Overview

If you have Ignition Tools installed, you can use the `ign gui` command line tool:

`ign gui -h`

    Ignition GUI tool.

      ign gui [options]

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
